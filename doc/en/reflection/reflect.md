# reflect

CRTP mixin base for the reflection inheritance chain.

- Header: `#include <scl/feature/reflection/reflect.h>`
- Declaration: `template <typename Wrapper, typename Executor, typename Type> class reflect;`
- Namespace: `scl::feature`

---

## Description

`scl::feature::reflect<Wrapper, Executor, Type>` is the primary (terminal) template
of the reflection chain. `detail::wrapper` inherits from it automatically:

```cpp
wrapper<Value, Executor>
    : reflect<wrapper<Value,Executor>, Executor<Value>, wrapper<Value,Executor>>
```

Two built-in partial specialisations peel off `detail::wrapper` layers from the
third parameter, delegating the chain downward until a concrete `Type` (not a
wrapper) is reached:

1. **cv/ref-strip** — collapses all 12 cv/ref variants of a qualified inner wrapper
   into a single rule.
2. **Chain delegation** — `reflect<Wrapper, E, wrapper<Value, E2>>` inherits from
   `reflect<Wrapper, E, Value>`, propagating all methods reflected for `Value` up
   to the outer wrapper.

The **primary template** (this file) has an empty body and serves as the terminal
case. User-provided partial specialisations for concrete types extend the chain.

## Extending reflect for a custom type

```cpp
// Specialise for the value type you want to reflect.
// SCL_REFLECT_TYPE must appear first; SCL_REFLECT_METHOD follows.
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(my_method)
    SCL_REFLECT_METHOD(another_method)
};
```

After this specialisation, any `wrapper<MyValue, E>` (or `wrapper<wrapper<MyValue, E1>, E2>`)
automatically exposes `my_method` and `another_method` as proxy members.

## Examples

### Reflecting a value type globally

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Temperature {
    double celsius() const &;
    double fahrenheit() const &;
    void   set_celsius(double v);
};

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Temperature>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(celsius)
    SCL_REFLECT_METHOD(fahrenheit)
    SCL_REFLECT_METHOD(set_celsius)
};

// Now works for any executor:
scl::wrapper<Temperature, scl::feature::inplace::plain> t{20.0};
t.set_celsius(100.0);
double f = t.fahrenheit();  // 212.0
```

### Nested wrapper chain

```cpp
using Inner = scl::wrapper<Temperature, locking_executor>;
using Outer = scl::wrapper<Inner, scl::feature::inplace::plain>;

Outer w{20.0};
// The reflection chain traverses wrapper layers automatically:
double c = w.celsius();
```

## See also

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [wrapper](../wrapper/wrapper.md)
- [executor_trait](../type_traits/executor.md)
