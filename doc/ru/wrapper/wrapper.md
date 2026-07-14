# wrapper

Компонуемая обёртка: хранит значение внутри исполнителя и делегирует ему все вызовы
методов.

- Заголовок: `#include <scl/feature/wrapper.h>`
- Объявление: `template <typename Value, template <typename> class Executor> class wrapper;`
- Пространство имён: `scl::feature::detail` (псевдоним `scl::wrapper<Value, Executor>`)

---

## Описание

`scl::wrapper<Value, Executor>` хранит объект `Value` внутри `Executor<Value>` и
предоставляет его интерфейс, делегируя каждый вызов исполнителю. Исполнитель определяет
способ хранения и доступа к значению — это позволяет встраивать сквозную логику
(потокобезопасность, копирование при записи, отложенную инициализацию) без изменения
типа `Value`.

Параметр `Executor` должен удовлетворять концепту `concepts::executor`: предоставлять
статические методы `access()` и `execute()` для всех трёх основных категорий значения
(`E&`, `E&&`, `E const&`).

`wrapper` наследует от `scl::feature::reflect<wrapper, Executor<Value>, wrapper>`,
который автоматически подключает все методы-посредники, отражённые для хранимого типа.

## Типы-члены

| Тип | Описание |
|-----|----------|
| `value_type` | Тип хранимого значения (`Value`) |
| `executor_type` | Тип исполнителя (`Executor<Value>`) |

## Конструкторы

```cpp
// Прямое конструирование — передаёт все аргументы в исполнитель.
template <typename... Args>
constexpr explicit wrapper(Args &&... args);

// Копирование или перемещение из того же типа wrapper (все 8 cv-ref-квалификаций).
// Создаётся макросом SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
constexpr wrapper(wrapper cv_ref other);

// Преобразующий конструктор из любого другого типа wrapper.
// Создаётся макросом SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER.
// Использует wrapper_constructor_resolver для выбора способа конструирования.
template <typename Other>
constexpr wrapper(Other && other);
```

## Операторы присваивания

У исполнителя нет собственного оператора присваивания: концепт
[executor](../type_traits/executor.md) требует, чтобы исполнитель был непригоден ни к
копирующему, ни к перемещающему присваиванию. Поэтому обёртка никогда не присваивает
один исполнитель другому. Вместо этого `w = x` **отражает присваивание через
исполнитель** — тем же двухвариантным механизмом выбора, что и любой другой отражённый
оператор (см. [рефлексия операторов](../reflection/operator.md)): при наличии
переопределения `operator_assign(self, rhs)` вызывается оно (получая неизменённый
операнд — то же соглашение, что у переопределений `operator_<name>`), иначе обёрнутое
значение присваивается через `execute()` как выражение `access(self) = source`.

```cpp
// Копирующее, перемещающее и преобразующее присваивание из того же типа wrapper
// (все 8 cv-ref-квалификаций источника). Создаётся SCL_WRAPPER_ASSIGNMENT_FOR_SELF.
// Это явно объявленные операторы копирующего и перемещающего присваивания: их
// объявление подавляет неявно удалённые (исполнитель-член непригоден к присваиванию).
constexpr decltype(auto) operator=(wrapper cv_ref other);  // × 8

// Присваивание из любого другого операнда: обычного значения или другого типа
// wrapper (читается через его собственный исполнитель, поэтому типы значений могут
// различаться). Создаётся SCL_WRAPPER_ASSIGNMENT_FOR_OPERAND; исключается для
// собственного типа wrapper (он обрабатывается перегрузками выше).
template <typename Other>
constexpr decltype(auto) operator=(Other && other);
```

- **Принимается любой операнд** — обычное значение (`w = 42`), другой тип wrapper или
  тот же тип wrapper. Именно поэтому `w = value` работает так же, как любой другой
  отражённый оператор.
- **Фундаментальные и классовые типы обрабатываются одинаково** — путь значения
  использует выражение `access(self) = source` (а не вызов `.operator=()`), поэтому
  присваивает `int`, `double` и классы с оператором-членом `operator=` одинаково, как и
  отражённые составные присваивания.
- **Тип результата** — `decltype(auto)`, как у любого отражённого оператора: тип
  результата переопределения на пути переопределения либо результат
  `access(self) = source` (обычно `value_type &`) на пути значения. Не обязательно
  `*this`.
- **Ограничение и `noexcept`** — каждая перегрузка ограничена присваиваемостью
  обёрнутого значения из допустимого операнда и пробрасывает `noexcept` выбранного
  пути. Обёртка над типом значения, непригодным к присваиванию, присваивания не
  предоставляет.
- **Замечание о неполном типе** — перегрузки для собственного типа не являются
  шаблонными, поэтому их ограничения вычисляются, пока `wrapper` ещё неполный тип; они
  получают значение источника через его исполнитель (`executor_type::access`), а не
  через саму обёртку.

## Доступ к исполнителю

Исполнитель-член закрыт; доступ выполняется через
`scl::feature::detail::executor_access::get(w)` и через специализацию
`scl::feature::executor_trait<wrapper<V,E>>`, используемую `SCL_REFLECT_METHOD`.

## Примеры

### Базовое использование

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/plain.h>

using IntWrapper = scl::wrapper<int, scl::feature::inplace::plain>;

IntWrapper w{42};
```

### С рефлексией

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/executor/inplace/plain.h>

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
