# value_lock

Recursive lazy lock through the entire wrapper chain.

- Header: `#include <scl/feature/value_lock.h>`
- Declaration: `template <typename Refer> class value_lock;`
- Namespace: `scl`

---

## Description

`scl::value_lock<Refer>` captures references to every executor in the wrapper
chain at construction — without acquiring any guard. Guards are activated
layer-by-layer on demand via `lock_for<Target>()`, which acquires only the guards
needed to reach `Target`. The value is then retrieved via `value_as<Target>()`.

This allows a single `value_lock` to serve multiple access patterns: accessing the
outermost wrapper requires no guards at all; reaching the innermost value locks
every layer.

`Refer` must be a reference type. `Target` must satisfy
`concepts::convertible_from<Target, Refer>`.
The object is non-copyable and non-movable.

## Interface

```cpp
template <typename Refer>
class value_lock
{
public:
    value_lock(value_lock &&) = delete;
    value_lock(value_lock const &) = delete;

    // Captures all executor references. No guard acquired.
    constexpr explicit value_lock(Refer ref) noexcept(...);

    constexpr ~value_lock() = default;

    // Activates guards for all layers needed to reach Target.
    // If Target is directly convertible from Refer, no guard is acquired.
    template <typename Target>
    constexpr void lock_for() noexcept(...)
        requires concepts::convertible_from<Target, Refer>;

    // Returns the value at the level matching Target.
    // lock_for<Target>() must have been called before this.
    template <typename Target>
    [[nodiscard]]
    constexpr Target value_as() noexcept(...)
        requires concepts::convertible_from<Target, Refer>;
};
```

## Examples

### Accessing a nested value

```cpp
#include <scl/feature/value_lock.h>
#include <scl/feature/wrapper.h>

using Inner = scl::wrapper<int, mutex_executor>;
using Outer = scl::wrapper<Inner, scl::feature::inplace::plain>;

Outer w{42};

scl::value_lock<Outer &> vl{w};    // no guard acquired

// Access the inner int — locks the mutex_executor layer:
vl.lock_for<int &>();
int & v = vl.value_as<int &>();    // v == 42

// Access the outer wrapper — no lock needed:
vl.lock_for<Outer &>();
Outer & ref = vl.value_as<Outer &>();
```

### Accessing intermediate layers

```cpp
// Access Inner directly (only the plain layer, no mutex needed):
vl.lock_for<Inner &>();
Inner & inner = vl.value_as<Inner &>();
```

## See also

- [wrapper_lock](wrapper_lock.md)
- [wrapper_guard](wrapper_guard.md)
- [wrapper_cast](../casts/wrapper_cast.md)
- [concepts::convertible_from](../concepts/concepts.md)
