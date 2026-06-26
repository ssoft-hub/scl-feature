# implicit::indirect

Heap-allocating executor with copy-on-write semantics, backed by an intrusive,
allocator-aware control block.

- Header: `#include <scl/feature/executor/implicit/indirect.h>`
- Declaration: `template <typename Value, typename Allocator = std::allocator<Value>, typename Counter = std::atomic<std::size_t>> class indirect;`
- Namespace: `scl::feature::implicit`

---

## Description

`implicit::indirect<Value>` stores the value on the heap in a single intrusive
control block — a reference counter placed next to the value. Copying the
executor (and therefore copying a `wrapper` that uses it) shares that block; no
value copy takes place. The value is cloned lazily the first time it is reached
for mutation through a **non-const** executor while still shared (reference count
greater than one). Access through a const executor never clones.

Compared with a `std::shared_ptr`-based design this keeps the executor **one
pointer wide** (with an empty allocator), performs **one** allocation per
distinct value, and lets the reference counter type be chosen: `std::atomic` for
a thread-safe count (the default) or a plain integer for single-threaded use
with no atomic overhead.

The clone is performed by `guard()`. It runs before any mutable access: `execute()`
calls it on the reflection dispatch path, and the locking utilities (`wrapper_guard`,
`value_lock`) call it on their guard phase. `access()` itself is a pure accessor and
never clones; copy-on-write works because the dispatch resolves `access()` *inside*
`execute()`, after `guard()` has detached, so every reflected method call operates on
an object owned by no other copy.

`execute(self, func, args...)` calls `guard()` and then invokes `func(args...)`. The
copy-on-write logic lives in `guard()`; `access()` does not clone on its own, so a
direct, non-const `access()` that bypasses `execute()`/`guard()` returns a reference
into the still-shared allocation — call `guard()` first (or reach the value through a
reflected call) to detach.

> **Note.** An executor cannot observe whether the *method* being called mutates;
> it only sees the cv-qualification of the wrapper at the call site. Therefore any
> access through a **non-const** wrapper clones a shared value, even a read-only
> method. Reach the value through a const wrapper (or const reference) to read
> without cloning.

> **Warning.** The clone copy-constructs `Value`. A polymorphic `Value` holding a
> derived object would be sliced; `Value` must be the concrete stored type. Use
> `implicit::polymorphic` for value-semantic polymorphism.

`indirect` is not provided for `volatile`-qualified executors — the shared counter
has no volatile-qualified operations, so volatile reflected overloads are discarded
rather than ill-formed.

## Template parameters

| Parameter | Default | Role |
|-----------|---------|------|
| `Value` | — | the wrapped value type |
| `Allocator` | `std::allocator<Value>` | allocator for the control block; rebound internally |
| `Counter` | `std::atomic<std::size_t>` | reference counter type; `std::atomic` for a thread-safe count |

### Choosing a custom allocator

`wrapper` takes the executor as a one-parameter template. Bind a custom allocator
through a one-parameter alias:

```cpp
template <typename T>
using my_indirect = scl::feature::implicit::indirect<T, my_allocator<T>>;

scl::wrapper<Foo, my_indirect> w{ /* ... */ };
```

A stateful allocator instance can be supplied at construction with the
`std::allocator_arg` overload.

## Interface

```cpp
template <typename Value,
          typename Allocator = std::allocator<Value>,
          typename Counter   = std::atomic<std::size_t>>
class indirect
{
public:
    using value_type     = Value;
    using allocator_type = Allocator;
    using counter_type   = Counter;

    // Constructs the held value in-place on the heap (default-constructed allocator).
    template <typename... Args>
    explicit indirect(Args &&... args);

    // Constructs the held value in-place using the given allocator instance.
    template <typename... Args>
    explicit indirect(std::allocator_arg_t, allocator_type const & allocator, Args &&... args);

    // Copy/move constructors and assignments. Copy shares the block (bumps the
    // counter); move transfers ownership. Volatile qualifications are constrained away.
    indirect(indirect cv_ref other);              // copy categories share, &&-category steals
    indirect & operator=(indirect cv_ref other);

    // Detaches (clones) a shared value. Runs before any mutable access:
    // called by execute() on the dispatch path and by the locking utilities.
    template <typename Self>
    static void guard(Self && self);

    // Returns a reference to the held value, preserving cv-ref qualifiers.
    // Pure accessor — never clones; detaching is done by guard()/execute().
    template <typename Self>
    static decltype(auto) access(Self && self);

    // Invokes func(args...) and returns the result.
    template <typename Self, typename Func, typename... Args>
    static decltype(auto) execute(Self && self, Func && func, Args &&... args);
};
```

## Examples

### As wrapper executor

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/implicit/indirect.h>

scl::wrapper<std::vector<int>, scl::feature::implicit::indirect> a{1, 2, 3};
auto b = a;          // shares the allocation — no value copy
b->push_back(4);     // b clones lazily; a still holds {1, 2, 3}
```

### Direct use

```cpp
#include <scl/feature/executor/implicit/indirect.h>

using Indirect = scl::feature::implicit::indirect<int>;

Indirect a{42};
Indirect b{a};                                 // shares the allocation

Indirect::guard(a);                            // detach the shared value (copy-on-write)
Indirect::access(a) = 7;                        // now mutating a's own copy
// Indirect::access(std::as_const(b)) still == 42 (b untouched)
```

### Single-threaded counter

```cpp
// No atomic overhead when the wrapper never crosses threads.
using Indirect = scl::feature::implicit::indirect<int, std::allocator<int>, std::size_t>;
```

## See also

- [inplace::plain](plain.md)
- [inplace::uninitialized](uninitialized.md)
- [wrapper](../wrapper/wrapper.md)
