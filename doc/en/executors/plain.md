# inplace::plain

In-place executor that stores the value directly with zero overhead.

- Header: `#include <scl/feature/executor/inplace/plain.h>`
- Declaration: `template <typename T> struct plain;`
- Namespace: `scl::feature::inplace`

---

## Description

`inplace::plain<T>` is the default executor used by `scl::wrapper` when no
executor is specified. It stores a `T` directly as a data member with no
additional indirection, no heap allocation, and no synchronisation.

Calling `execute(self, func, args...)` simply invokes `func(args...)` immediately
in the caller's context — no interception takes place. Because `plain` does not
provide `guard()`/`unguard()`, locking utilities treat it as a no-op layer.

## Interface

```cpp
template <typename T>
struct plain
{
    using value_type = T;

    T m_value;

    // Returns a reference to the held value, preserving cv-ref qualifiers.
    template <typename Self>
    static constexpr decltype(auto) access(Self && self);

    // Invokes func(args...) immediately and returns the result.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);

    // Copy and move assignment — 8 overloads, one per cv-ref qualification.
    // Each overload is constrained on T being assignable from the corresponding
    // qualification of T and is noexcept when the underlying assignment is.
    constexpr plain & operator=(plain cv_ref other);  // × 8 cv-ref qualifications
};
```

## Examples

### Direct use

```cpp
#include <scl/feature/executor/inplace/plain.h>

scl::feature::inplace::plain<int> e{42};

int & ref = scl::feature::inplace::plain<int>::access(e);        // ref == 42
int   val = scl::feature::inplace::plain<int>::execute(e, [&]{ return e.m_value * 2; }); // 84
```

### As wrapper executor

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/plain.h>

scl::wrapper<std::string, scl::feature::inplace::plain> w{"hello"};
```

## See also

- [inplace::uninitialized](uninitialized.md)
- [wrapper](../wrapper/wrapper.md)
