# wrapper_lock

Lazy RAII lock for a single wrapper layer — the guard is acquired only when
`lock()` is called explicitly.

- Header: `#include <scl/feature/wrapper_lock.h>`
- Declaration: `template <typename Refer> class wrapper_lock;`
- Namespace: `scl`

---

## Description

`scl::wrapper_lock<Refer>` captures a reference to a wrapper at construction
without acquiring any guard. The guard is activated lazily by calling `lock()`
and released by `unlock()` or by destruction (if the lock is held).

`lock()` is idempotent — a second call while already locked is a no-op.
`unlock()` releases the guard; if not locked, it is also a no-op.

For executors that do not provide `guard()`/`unguard()`, `lock()`/`unlock()` are
no-ops. For non-wrapper types the entire class is a pass-through.

`Refer` must be a reference type. The object is non-copyable and non-movable.

## Interface

```cpp
template <typename Refer>
class wrapper_lock
{
public:
    wrapper_lock(wrapper_lock &&) = delete;
    wrapper_lock(wrapper_lock const &) = delete;

    // Captures the reference; does NOT acquire the guard.
    constexpr explicit wrapper_lock(Refer ref) noexcept;

    // Releases the guard if currently held.
    constexpr ~wrapper_lock() noexcept(...);

    // Acquires the guard (calls executor::guard). Idempotent.
    constexpr void lock() noexcept(...);

    // Releases the guard (calls executor::unguard). Idempotent.
    constexpr void unlock() noexcept(...);

    // Returns the wrapper reference (does not require the lock).
    [[nodiscard]] constexpr Refer wrapper_value() const noexcept;

    // Returns the inner value through the executor (lock should be held).
    [[nodiscard]] constexpr decltype(auto) value() const noexcept(...);
};
```

## Examples

### Conditional locking

```cpp
#include <scl/feature/wrapper_lock.h>
#include <scl/feature/wrapper.h>

scl::wrapper<int, mutex_executor> w{0};

scl::wrapper_lock<decltype(w) &> lk{w};  // no guard yet

if (needs_write) {
    lk.lock();               // guard acquired here
    lk.value() = 42;
    lk.unlock();             // guard released here
}
// or let destructor release it
```

### Used inside value_lock

`wrapper_lock` is the building block for `value_lock`: each layer of the wrapper
chain gets its own `wrapper_lock`; `value_lock::lock_for<Target>()` activates
only the layers needed to reach the target type.

## See also

- [wrapper_guard](wrapper_guard.md)
- [value_lock](value_lock.md)
