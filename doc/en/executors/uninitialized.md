# inplace::uninitialized

In-place executor that holds the value in correctly sized and aligned raw storage,
enabling deferred (lazy) construction.

- Header: `#include <scl/feature/executor/inplace/uninitialized.h>`
- Declaration: `template <typename T> struct uninitialized;`
- Namespace: `scl::feature::inplace`

---

## Description

`inplace::uninitialized<T>` reserves the storage for a `T` without constructing it.
Construction is the responsibility of the user — typically performed via
placement-new into `m_storage`. Once constructed, `value()` provides access through
a `reinterpret_cast` on the raw storage.

This executor is useful for types that are expensive to default-construct or that
must be initialised in a specific order.

Like `inplace::plain`, it does not provide `guard()`/`unguard()` and has no
synchronisation overhead.

## Interface

```cpp
template <typename T>
struct uninitialized
{
    using value_type = T;

    alignas(T) std::byte m_storage[sizeof(T)];

    // Returns a reference to the stored object via reinterpret_cast.
    // Behaviour is undefined if the object has not been constructed.
    template <typename Self>
    static constexpr decltype(auto) value(Self && self);

    // Invokes func(args...) immediately and returns the result.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);

    // Raw-storage copy — 8 overloads, one per cv-ref qualification.
    // Copies the underlying bytes via std::ranges::copy regardless of whether T
    // has been constructed. Does not call T's assignment operator.
    constexpr uninitialized & operator=(uninitialized cv_ref other);  // × 8
};
```

## Examples

### Deferred construction

```cpp
#include <scl/feature/executor/inplace/uninitialized.h>
#include <string>

scl::feature::inplace::uninitialized<std::string> e;

// Construct the object manually when ready:
new (&e.m_storage) std::string{"hello"};

std::string & s = scl::feature::inplace::uninitialized<std::string>::value(e);
// s == "hello"

// Destroy manually when done:
s.~basic_string();
```

### As wrapper executor

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/uninitialized.h>

scl::wrapper<std::string, scl::feature::inplace::uninitialized> w{"world"};
```

## See also

- [inplace::plain](plain.md)
- [wrapper](../wrapper/wrapper.md)
