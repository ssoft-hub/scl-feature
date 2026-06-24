# wrapper_cast

Converts a wrapper to any type reachable in its chain; pass-through for
non-wrapper types.

- Header: `#include <scl/feature/wrapper_cast.h>`
- Declaration: `template <typename Type> constexpr decltype(auto) wrapper_cast(Type && w) noexcept;`
- Namespace: `scl`

---

## Description

`scl::wrapper_cast(x)` returns a `wrapper_caster<Refer>` proxy that exposes
implicit conversion operators to every type reachable by traversing the wrapper
chain. For non-wrapper arguments the function is a pass-through returning the
forwarded reference unchanged with no overhead.

The conversion operators are `&&`-qualified — the proxy must be an rvalue at the
point of conversion (either a temporary returned directly from `wrapper_cast()`,
or an explicit `std::move(proxy)`). An explicit named conversion is also available
via `.to<T>()`.

The primary use case is generic code that must accept both plain values and
wrappers uniformly — most notably the argument forwarding inside
`SCL_REFLECT_METHOD`, which calls `wrapper_cast(arg)` on every argument so that
a `wrapper<T>` can be passed wherever a `T` is expected.

## wrapper_caster

`wrapper_caster<Refer>` is a `[[nodiscard]]` non-copyable, non-movable proxy.

```cpp
template <typename Refer>
class [[nodiscard]] wrapper_caster
{
public:
    wrapper_caster(wrapper_caster &&) = delete;
    wrapper_caster(wrapper_caster const &) = delete;

    // Implicit conversion to any type reachable from Refer.
    // Operators are &&-qualified.
    [[nodiscard]] operator TargetType() &&;

    // Explicit named conversion.
    template <typename T>
    [[nodiscard]] T to() &&
        requires concepts::convertible_from<T, Refer>;
};
```

## Examples

### Passing a wrapper to a function expecting the value type

```cpp
#include <scl/feature/wrapper_cast.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/inplace/plain.h>

void increment(int & v) { ++v; }

scl::wrapper<int, scl::feature::inplace::plain> w{41};

// Implicit conversion: wrapper<int> → int&
increment(scl::wrapper_cast(w));  // w now holds 42
```

### Explicit conversion via .to<T>()

```cpp
int & ref = scl::wrapper_cast(w).to<int &>();
```

### Pass-through for non-wrapper types

```cpp
int x = 7;
int & r = scl::wrapper_cast(x);  // r == x; no allocation, no overhead
```

### In generic code

```cpp
template <typename Arg>
void process(Arg && arg)
{
    // Works uniformly: wrapper → value, non-wrapper → as-is.
    use(scl::wrapper_cast(std::forward<Arg>(arg)));
}
```

## See also

- [value_lock](../locking/value_lock.md)
- [concepts::convertible_from](../concepts/concepts.md)
- [SCL_REFLECT_METHOD](../reflection/macros.md)
