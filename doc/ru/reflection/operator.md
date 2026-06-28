# Макросы рефлексии операторов

Генерируют прокси-перегрузки операторов, перенаправляя вызовы из wrapper к хранимому
объекту через исполнитель.

- Заголовок: `#include <scl/feature/reflection/operator.h>`

---

## Семейства макросов

| Макрос | Генерируемые перегрузки | Назначение |
|--------|------------------------|------------|
| `SCL_REFLECT_BINARY_OPERATOR(op, name)` | 24 member + 8 reverse-operand friend | симметричные бинарные операторы (`+`, `==`, `<`, …) |
| `SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)` | 8 member | префиксные унарные операторы (`-`, `++`, `&`, …) |
| `SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)` | 8 member | постфиксные унарные операторы (`++`, `--`) |
| `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)` | 24 member | обязательно-member операторы (`=`, `*=`, …) |
| `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(op, name)` | 8 member | обязательно-member префиксные унарные (`->`) |
| `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR(op, name)` | 8 member | обязательно-member постфиксные унарные |
| `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)` | 24 friend | только ADL-доступные бинарные операторы |
| `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(op, name)` | 8 friend | только ADL-доступные префиксные унарные |
| `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(op, name)` | 8 friend | только ADL-доступные постфиксные унарные |
| `SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name)` | 24 member | `[]`, `()` (обязаны быть member в C++20) |

`SCL_REFLECT_TYPE` должен быть объявлен в том же теле класса до любого макроса операторов.

---

## Симметричные бинарные операторы (`SCL_REFLECT_BINARY_OPERATOR`)

`SCL_REFLECT_BINARY_OPERATOR` генерирует:

1. **Member-перегрузки** (24) — выбираются, когда wrapper стоит **слева** (`w op x`);
   отражают `value.operator op(x)`.
2. **Reverse-operand hidden-friend-перегрузки** (8, по одной на cv-ref-квалификатор) —
   находятся через ADL, когда wrapper стоит **справа** (`x op w`); отражают `x op value`.

Reverse-friend исключается, когда левый операнд сам является wrapper (`is_wrapper_v`),
поэтому `w1 op w2` разрешается в member-перегрузку без неоднозначности. Не-wrapper
левый операнд не может связаться с wrapper-параметром member-перегрузки, поэтому в каждом
направлении ровно один применимый кандидат.

```cpp
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_BINARY_OPERATOR(+, plus)        // w + x  (member)  И  x + w  (reverse friend)
    SCL_REFLECT_BINARY_OPERATOR(==, equal_to)   // x == w (reverse friend); w == x может также
                                                // работать через C++20 reversed candidates (зависит от компилятора)
};
```

> Примечание: reverse-executor-override отсутствует — соглашение `operator_<name>(exec, …)`
> применяется только к member-перегрузкам (wrapper слева). Reverse-направление по-прежнему
> проходит через `Executor::execute`, поэтому сквозное поведение работает в обе стороны.
>
> Унарные операторы (`SCL_REFLECT_PREFIX_UNARY_OPERATOR` /
> `SCL_REFLECT_POSTFIX_UNARY_OPERATOR`) имеют один операнд и потому не имеют reverse-случая;
> они генерируют только member-перегрузки. Для ADL-доступного унарного оператора используйте
> `SCL_REFLECT_FRIEND_*`.

---

## Только member-варианты (`SCL_REFLECT_MEMBER_*`)

Стандарт C++ ([over.oper]) требует, чтобы `operator=`, `operator->` и все составные
операторы присваивания были нестатическими member-функциями. Генерировать для них
hidden-friend свободные функции недопустимо.

Используйте `SCL_REFLECT_MEMBER_*` для этих операторов:

```cpp
SCL_REFLECT_MEMBER_BINARY_OPERATOR(=,   assign)
SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=,  plus_assign)
SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(->, arrow)
```

`reflect_operators` (базовый класс `scl::wrapper` по умолчанию) использует `SCL_REFLECT_MEMBER_*`
для `=`, `*=`, `/=`, `%=`, `+=`, `-=`, `<<=`, `>>=`, `&=`, `|=`, `^=` и `->`.

---

## Только friend-варианты (`SCL_REFLECT_FRIEND_*`)

Генерируют исключительно hidden-friend свободные функции без member-перегрузок.

Используйте, когда оператор должен быть доступен через ADL, но вызов
`w.operator op(...)` через синтаксис member должен быть запрещён. Не совмещайте
с `SCL_REFLECT_*_OPERATOR` для того же `name` в одном теле класса — оба макроса
генерируют одинаковые вспомогательные static-члены, что приведёт к дублированию.

---

## Выбор перегрузки и тип возвращаемого значения

Вызов разрешается в два шага: сначала выбирается **направление** (какая
сгенерированная перегрузка жизнеспособна), затем **путь** (как эта перегрузка
достигает значения).

### Направление (бинарные операторы)

| Выражение | Выбранная перегрузка | Отражает |
|-----------|----------------------|----------|
| `w op x` (wrapper слева) | member | `value op x` |
| `x op w` (wrapper справа, `x` — не wrapper) | reverse-operand friend | `x op value` |
| `w1 op w2` (оба wrapper) | member — reverse-friend отсекается через `is_wrapper_v` | `value1 op w2` |

Member-перегрузка (wrapper слева) жизнеспособна **только если у обёрнутого
значения действительно есть подходящий `operator op`** для данного cv-ref-квалификатора;
иначе она отсекается. У фундаментальных типов (`int`, `double`, …) операторов-членов
нет, поэтому для них формы с wrapper слева — включая `w op x` и все составные
присваивания `w op= x` — **не** генерируются; применимы только reverse-friend'ы
(`x op w`), т.к. `x op value` — валидный встроенный синтаксис.

> Собственное копирующее/перемещающее присваивание `=` у `scl::wrapper` —
> единственное исключение из правила о фундаментальных типах: `w = value`
> присваивает обёрнутое значение как для фундаментальных, так и для классовых
> типов, поскольку это не отражённый оператор-член, а отдельный оператор wrapper,
> использующий присваивание-выражение. См. [wrapper](../wrapper/wrapper.md).

### Путь (внутри выбранной перегрузки)

| Путь | Условие | Вызывает | Возвращает |
|------|---------|----------|------------|
| *executor-override* | существует `Executor::operator_<name>(exec, args...)` | этот статический член напрямую, с **сырыми** операндами | тип возврата override |
| *execute-path* | нет override у исполнителя | `Executor::execute(exec, callable, exec, args...)`; callable применяет `scl::wrapper_cast` к wrapper-аргументам и разрешает значение внутри `execute()` | тип возврата отражённого вызова |

Каждая перегрузка объявлена как `decltype(auto)`: wrapper возвращает результат
override (путь override) или результат оператора обёрнутого значения (execute-path)
дословно — он никогда не переоборачивает результат. Для reverse-operand friend'ов
пути executor-override нет; соглашение `operator_<name>` применяется только к
member-перегрузкам с wrapper слева.

---

## Ограничение: совпадающие типы возвращаемых значений

Как и `SCL_REFLECT_METHOD`: перегрузки `operator op` типа-значения для разных
cv-ref-квалификаторов должны возвращать разные типы, иначе `SCL_HAS_QUALIFIED_METHOD`
не сможет различить их по типу возврата при выборе перегрузки под квалификатор.

---

## Примеры

### Симметричное сравнение через комбинированный макрос

```cpp
struct Length { bool operator==(Length) const &; };

template <typename W, typename E>
class scl::feature::reflect<W, E, Length>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_BINARY_OPERATOR(==, equal_to)
};

scl::wrapper<Length> a, b;
bool r1 = (a == b);          // member-перегрузка
bool r2 = (a == Length{});   // member-перегрузка (wrapper слева)
// Length{} == a — также доступно через ADL-friend
```

### Обязательно-member составное присваивание

```cpp
struct Counter {
    Counter & operator+=(int) &;
};

template <typename W, typename E>
class scl::feature::reflect<W, E, Counter>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=, plus_assign)
};

scl::wrapper<Counter> c;
c += 42;   // OK — отражает Counter::operator+=
```

> Обычный `=` — другое дело. У `scl::wrapper` копирующее/перемещающее/преобразующее
> присваивание — **собственный** оператор wrapper (он обязан подавить неявно
> удалённые copy/move-присваивания; см. [wrapper](../wrapper/wrapper.md)), который
> скрывает любой отражённый `operator=`. Поэтому `SCL_REFLECT_MEMBER_BINARY_OPERATOR(=, assign)`
> управляет присваиванием только для кастомного прокси-типа, наследующего
> отражённые операторы без объявления собственного `operator=`, но не для
> `scl::wrapper`. Составные присваивания (`+=`, `*=`, …) и `->` отражаются на любом
> wrapper штатно.

---

## См. также

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
