# SCL_REFLECT_TYPE

Объявляет псевдонимы типов wrapper и исполнителя, необходимые для макросов рефлексии.

- Заголовок: `#include <scl/feature/reflection/type.h>`

---

## Описание

```cpp
#define SCL_REFLECT_TYPE(type, executor)
```

Генерирует два псевдонима типа внутри объемлющего класса:

- `S_c_L_type_` — имя класса wrapper (`type`)
- `S_c_L_executor_type_` — тип исполнителя (`executor`)

Должен располагаться **внутри тела класса**, перед любыми другими макросами
рефлексии (`SCL_REFLECT_METHOD`, `SCL_REFLECT_OPERATOR`, `SCL_REFLECT_PROPERTY`, …).

Второй аргумент — **имя типа исполнителя** (например, `executor_type` или
`scl::feature::inplace::plain<T>`), а не выражение с членом класса.

Для wrapper-типа также необходима специализация `scl::feature::executor_trait` —
она позволяет макросам рефлексии находить экземпляр исполнителя при вызове метода.
`detail::wrapper` предоставляет её автоматически.

## Пример

```cpp
// Внутри класса wrapper или специализации reflect<>:
using executor_type = scl::feature::inplace::plain<int>;
executor_type m_executor;

SCL_REFLECT_TYPE(MyWrapper, executor_type)
// Далее SCL_REFLECT_METHOD / SCL_REFLECT_OPERATOR / ...
```

## См. также

- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
