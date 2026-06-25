# reflect

CRTP-примесь — базовый класс цепочки рефлексии.

- Заголовок: `#include <scl/feature/reflection/reflect.h>`
- Объявление: `template <typename Wrapper, typename Executor, typename Type> class reflect;`
- Пространство имён: `scl::feature`

---

## Описание

`scl::feature::reflect<Wrapper, Executor, Type>` — основной (терминальный) шаблон
цепочки рефлексии. `detail::wrapper` наследует от него автоматически:

```cpp
wrapper<Value, Executor>
    : reflect<wrapper<Value,Executor>, Executor<Value>, wrapper<Value,Executor>>
```

Две встроенные частичные специализации снимают слои `detail::wrapper` с третьего
параметра, делегируя цепочку вглубь до конкретного (не-wrapper) типа:

1. **Снятие cv/ref** — сворачивает все 12 cv/ref вариантов квалифицированного
   внутреннего wrapper в одно правило.
2. **Делегирование цепочки** — `reflect<Wrapper, E, wrapper<Value, E2>>` наследует
   от `reflect<Wrapper, E, Value>`, распространяя все методы, отражённые для
   `Value`, до внешней обёртки.

**Основной шаблон** — терминальный случай с пустым телом.
Пользовательские частичные специализации для конкретных типов значений расширяют цепочку.

## Расширение reflect для пользовательского типа

```cpp
// Специализация для типа значения, который нужно отразить.
// SCL_REFLECT_TYPE должен идти первым; за ним следуют SCL_REFLECT_METHOD.
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(my_method)
    SCL_REFLECT_METHOD(another_method)
};
```

После такой специализации любой `wrapper<MyValue, E>` (в том числе вложенный —
`wrapper<wrapper<MyValue, E1>, E2>`) автоматически получает
`my_method` и `another_method` в качестве прокси-членов.

## Примеры

### Глобальное отражение типа значения

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/executor/inplace/plain.h>

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

// Работает для любого исполнителя:
scl::wrapper<Temperature, scl::feature::inplace::plain> t{20.0};
t.set_celsius(100.0);
double f = t.fahrenheit();  // 212.0
```

### Вложенная цепочка обёрток

```cpp
using Inner = scl::wrapper<Temperature, locking_executor>;
using Outer = scl::wrapper<Inner, scl::feature::inplace::plain>;

Outer w{20.0};
// Цепочка рефлексии автоматически обходит слои wrapper:
double c = w.celsius();
```

## См. также

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [wrapper](../wrapper/wrapper.md)
- [executor_trait](../type_traits/executor.md)
