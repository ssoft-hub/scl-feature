# SCL_REFLECT_METHOD

Generates proxy method overloads that forward calls from a wrapper to the held
object through the executor, for all 8 cv-ref qualifier combinations.

- Header: `#include <scl/feature/reflection/method.h>`

---

## Description

```cpp
#define SCL_REFLECT_METHOD(method)
```

Generates **24 proxy overloads** for `method` (3 per cv-ref × 8 qualifications):

| Overload kind | Condition | Dispatch |
|---------------|-----------|----------|
| *executor-override* | `Executor::method_##method(exec, args...)` exists | calls that static member directly |
| *execute-path* | no executor override | calls `Executor::execute(exec, callable, args...)` |
| *explicit-template-args* | always generated | exposes `wrapper.method<T>(args...)` |

The executor-override and execute-path overloads are mutually exclusive — at most
16 are active for any given executor.

`SCL_REFLECT_TYPE` must be declared in the same class body before this macro.

## Argument forwarding

Arguments are passed through `scl::wrapper_cast()` before being forwarded to the
wrapped method. This allows a `wrapper<T>` to be passed wherever a `T` is expected.

## Constraint: distinct return types

`SCL_REFLECT_METHOD` relies on return-type discrimination to distinguish overloads
with different cv-ref qualifiers. Overloads that return the same type are
indistinguishable:

```cpp
struct Good {
    short get() &;       // short ≠ int — distinguishable
    int   get() const &;
};

struct Bad {
    int get() &;         // int == int — macro cannot distinguish
    int get() const &;
};
```

## Executor-override dispatch

If the executor provides a static `method_foo(exec, args...)` member, calls to
`wrapper.foo(args...)` are routed to it directly, bypassing `execute()`. This
lets an executor supply a custom implementation for specific methods.

## Template methods

If the wrapped method is a template, explicit template arguments can be passed
through the reflected proxy:

```cpp
struct Value {
    template <typename T>
    T convert() const &;
};

// After SCL_REFLECT_METHOD(convert):
scl::wrapper<Value, inplace::plain> w{...};
double d = w.convert<double>();
```

## Examples

### With reflect specialisation

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/executor/inplace/plain.h>

struct Point {
    short x() &;
    int   x() const &;
    float x() &&;
};

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Point>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(x)
};

using PointWrapper = scl::wrapper<Point, scl::feature::inplace::plain>;

PointWrapper p;
short  a = p.x();                // Point::x() &
int    b = std::as_const(p).x(); // Point::x() const &
float  c = std::move(p).x();     // Point::x() &&
```

### In a custom wrapper class

```cpp
struct MyWrapper;
template <>
struct scl::feature::executor_trait<MyWrapper> {
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_executor); }
};

struct MyWrapper {
    using executor_type = scl::feature::inplace::plain<Point>;
    executor_type m_executor;

    SCL_REFLECT_TYPE(MyWrapper, executor_type)
    SCL_REFLECT_METHOD(x)
};
```

## See also

- [SCL_REFLECT_TYPE](type.md)
- [reflect](reflect.md)
- [wrapper_cast](../casts/wrapper_cast.md)
- [executor_trait](../type_traits/executor.md)
