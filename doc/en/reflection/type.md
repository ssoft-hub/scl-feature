# SCL_REFLECT_TYPE

Declares the wrapper and executor type aliases required by reflection macros.

- Header: `#include <scl/feature/reflection/type.h>`

---

## Description

```cpp
#define SCL_REFLECT_TYPE(type, executor)
```

Establishes the internal type context required by all subsequent reflection macros.
The generated aliases are implementation details — do not reference them directly.

Must appear **inside the class body**, before any other reflection macros
(`SCL_REFLECT_METHOD`, `SCL_REFLECT_OPERATOR`, `SCL_REFLECT_PROPERTY`, …).

The second argument is an **executor type name** (e.g. `executor_type` or
`scl::feature::inplace::plain<T>`), not a member expression.

A specialisation of `scl::feature::executor_trait` must also be provided for
the wrapper type so that reflection macros can locate the executor at runtime.
`detail::wrapper` provides this specialisation automatically.

## Example

```cpp
// Inside a wrapper class or a reflect<> specialisation:
using executor_type = scl::feature::inplace::plain<int>;
executor_type m_executor;

SCL_REFLECT_TYPE(MyWrapper, executor_type)
// Followed by SCL_REFLECT_METHOD / SCL_REFLECT_OPERATOR / ...
```

## See also

- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
