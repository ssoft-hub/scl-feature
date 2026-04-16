# wrapper

Компонуемая обёртка: хранит значение внутри исполнителя и делегирует
ему все вызовы методов.

- Заголовок: `#include <scl/feature/wrapper.h>`
- Объявление: `template <typename Value, template <typename> class Executor> class wrapper;`
- Пространство имён: `scl::feature::detail` (алиас `scl::wrapper<Value, Executor>`)

---

## Описание

`scl::wrapper<Value, Executor>` хранит объект `Value` внутри `Executor<Value>` и
предоставляет его интерфейс, делегируя каждый вызов исполнителю. Исполнитель
определяет способ хранения и доступа к значению — это позволяет встраивать
сквозную логику (потокобезопасность, copy-on-write, отложенная инициализация)
без изменения типа `Value`.

Параметр `Executor` должен удовлетворять концепту `concepts::executor`: иметь
статические методы `value()` и `execute()` для всех трёх основных категорий
значения (`E&`, `E&&`, `E const&`).

`wrapper` наследует от `scl::feature::reflect<wrapper, Executor<Value>, wrapper>`,
который автоматически подключает все прокси-методы, отражённые для хранимого типа.

## Типы-члены

| Тип | Описание |
|-----|----------|
| `value_type` | Тип хранимого значения (`Value`) |
| `executor_type` | Экземпляр исполнителя (`Executor<Value>`) |

## Конструкторы

```cpp
// Прямое конструирование — передаёт все аргументы в исполнитель.
template <typename... Args>
constexpr explicit wrapper(Args &&... args);

// Копирование/перемещение из того же типа wrapper (все 8 cv-ref квалификаций).
// Генерируется макросом SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
constexpr wrapper(wrapper cv_ref other);

// Конвертирующий конструктор из любого другого типа wrapper.
// Генерируется макросом SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER.
// Использует wrapper_constructor_resolver для выбора стратегии конструирования.
template <typename Other>
constexpr wrapper(Other && other);
```

## Доступ к исполнителю

Член-исполнитель приватный; доступ осуществляется через
`scl::feature::detail::executor_access::get(w)` и через специализацию
`scl::feature::executor_trait<wrapper<V,E>>`, используемую `SCL_REFLECT_METHOD`.

## Примеры

### Базовое использование

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/inplace/plain.h>

using IntWrapper = scl::wrapper<int, scl::feature::inplace::plain>;

IntWrapper w{42};
```

### С рефлексией

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Counter {
    int value() const & { return m_value; }
    void increment()    { ++m_value; }
private:
    int m_value = 0;
};

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Counter>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(value)
    SCL_REFLECT_METHOD(increment)
};

using CounterWrapper = scl::wrapper<Counter, scl::feature::inplace::plain>;

CounterWrapper c;
c.increment();
c.increment();
int v = c.value();  // 2
```

### Вложенные обёртки

```cpp
// Внешний слой: plain (без накладных расходов). Внутренний: блокирующий исполнитель.
using Inner = scl::wrapper<int, locking_executor>;
using Outer = scl::wrapper<Inner, scl::feature::inplace::plain>;

Outer w{42};
// wrapper_cast разворачивает до int через оба слоя:
int val = scl::wrapper_cast(w);
```

## См. также

- [inplace::plain](../executors/plain.md)
- [inplace::uninitialized](../executors/uninitialized.md)
- [reflect](../reflection/reflect.md)
- [SCL_REFLECT_TYPE](../reflection/type.md)
- [SCL_REFLECT_METHOD](../reflection/method.md)
- [wrapper_cast](../casts/wrapper_cast.md)
