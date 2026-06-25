# SCL_REFLECT_PROPERTY

Generates a `wrapper` data member that proxies a single field of the wrapped
object through the executor, for both read and write access.

- Header: `#include <scl/feature/reflection/property.h>`

---

## Description

```cpp
#define SCL_REFLECT_PROPERTY(prop)
```

Generates one data member inside the enclosing class: a `scl::wrapper` over the
property's field type, bound to the field of the wrapped object and driven by the
executor established by `SCL_REFLECT_TYPE`. An internal tag struct is also emitted
for executor-override detection. The exact expansion relies on aliases that are
implementation details — do not reference them directly.

The macro may be used in any class that satisfies the requirements above — it is
not limited to classes that are themselves `scl::wrapper` instantiations.

`SCL_REFLECT_TYPE` must be declared in the same class body before this macro.

The reflected field `prop` **must exist as a data member** on the wrapped type,
even when an executor override is provided.

## Access

| Expression | Behaviour |
|------------|-----------|
| `int v = wrapper.prop` | implicit conversion via `operator T() &` / `operator T const&() const&` |
| `wrapper.prop = value` | assignment via `operator=(T)` |
| `wrapper.prop = other_wrapper.prop` | cross-wrapper assignment via `wrapper_cast` |

## Executor override

If the executor provides a static member:

```cpp
static R property_prop(Executor cv_ref exec);
```

whose first parameter matches a cv-ref qualification **exactly** (detected via
the same function-pointer cast technique as `has_execute_v`), access for that
qualification calls the override directly, bypassing `execute()`.

| Overload kind | Condition | Dispatch |
|---------------|-----------|----------|
| *executor-override* | `Executor::property_prop(exec)` exists | calls that static member directly |
| *execute-path* | no executor override for that cv-ref | calls `Executor::execute(exec, lambda, value)` |

The two paths are not mutually exclusive across qualifications: a mutable-lvalue
override coexists with a const-lvalue execute-path on the same property.

## noexcept propagation

`noexcept` is propagated from the executor override when one is active.
The execute path makes no `noexcept` guarantee.

## Copy / move safety

The inner executor stores a byte offset from itself to the outer executor.
Because this distance is invariant across instances, memberwise copy and move
work correctly without custom constructors.

## Limitations

The offset is computed with `reinterpret_cast` (the `container_of` idiom), which
is not a constant expression. A reflected property member therefore cannot be
used in a constant expression, and a wrapper carrying `SCL_REFLECT_PROPERTY`
members is not `constexpr`-constructible.

The executor data member must be declared **before** any `SCL_REFLECT_PROPERTY`,
so that it is constructed first (member initialization follows declaration
order).

## Example

### Without executor override

```cpp
#include <scl/feature/reflection/property.h>

struct Point { int x = 0; int y = 0; };

struct PointWrapper;
template <>
struct scl::feature::executor_trait<PointWrapper> {
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_exec); }
};

struct PointWrapper {
    scl::feature::inplace::plain<Point> m_exec;
    SCL_REFLECT_TYPE(PointWrapper, scl::feature::inplace::plain<Point>)
    SCL_REFLECT_PROPERTY(x)
    SCL_REFLECT_PROPERTY(y)
};

PointWrapper p{Point{3, 7}};
int a = p.x;  // 3 — execute path
p.x = 42;     // write via execute path
```

### With executor override

```cpp
struct SpecialExecutor {
    Point m_value;
    int   m_shadow_x = 0;

    // Mutable-lvalue access redirected to m_shadow_x:
    static int & property_x(SpecialExecutor & self) noexcept
    { return self.m_shadow_x; }

    template <typename Self, typename Func, typename... Args>
    static decltype(auto) execute(Self &&, Func && f, Args &&... args)
    { return f(::std::forward<Args>(args)...); }

    template <typename Self>
    static decltype(auto) access(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_value); }
};
```

For this executor, mutable access to `wrapper.x` calls `property_x` directly
(noexcept, reads `m_shadow_x`), while const access falls through to `execute()`
(reads `m_value.x`).

## See also

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
