# wrapper_guard

RAII guard that calls `guard()` on the executor at construction and `unguard()`
at destruction.

- Header: `#include <scl/feature/wrapper_guard.h>`
- Declaration: `template <typename Refer> class wrapper_guard;`
- Namespace: `scl`

---

## Description

`scl::wrapper_guard<Refer>` is a non-copyable, non-movable RAII handle.
When constructed with a reference to a wrapper (or a plain value), it immediately
calls the executor's `guard()` if one is present. On destruction it calls
`unguard()` symmetrically.

For plain value references (non-wrapper types) and for executors that do not
provide `guard()`/`unguard()`, the guard is a no-op — no overhead is introduced.

`Refer` must be a reference type.

## Interface

```cpp
template <typename Refer>
class wrapper_guard
{
public:
    wrapper_guard(wrapper_guard &&) = delete;
    wrapper_guard(wrapper_guard const &) = delete;

    // Acquires the guard immediately.
    constexpr explicit wrapper_guard(Refer ref)
        noexcept(is_guard_noexcept_v<...>);

    // Releases the guard.
    constexpr ~wrapper_guard()
        noexcept(is_unguard_noexcept_v<...>);

    // Returns a reference to the held value through the executor.
    [[nodiscard]]
    constexpr decltype(auto) value() const;
};
```

## Examples

### Guarding a wrapper with a locking executor

```cpp
#include <scl/feature/wrapper_guard.h>
#include <scl/feature/wrapper.h>

scl::wrapper<int, mutex_executor> w{42};

{
    scl::wrapper_guard<decltype(w) &> guard{w};
    // mutex locked here
    int & v = guard.value();  // access held value
    v = 100;
}   // mutex unlocked here
```

### No-op for plain executor

```cpp
scl::wrapper<int, scl::feature::inplace::plain> w{7};
scl::wrapper_guard<decltype(w) &> guard{w};  // no guard/unguard calls
int & v = guard.value();
```

## See also

- [wrapper_lock](wrapper_lock.md)
- [value_lock](value_lock.md)
