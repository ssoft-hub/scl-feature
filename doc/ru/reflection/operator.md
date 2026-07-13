# Макросы рефлексии операторов

Генерируют прокси-перегрузки операторов, перенаправляя вызовы из wrapper к хранимому
объекту через исполнитель.

- Заголовок: `#include <scl/feature/reflection/operator.h>`

---

## Семейства макросов

| Макрос | Отражает | Назначение |
|--------|----------|------------|
| `SCL_REFLECT_BINARY_OPERATOR(op, name)` | `w op x`, `x op w`, `w1 op w2` | симметричные бинарные операторы (`+`, `<`, …) |
| `SCL_REFLECT_EQUALITY_OPERATOR(op, name)` | `w op x`, `x op w` с типом `bool` | `==`, `!=` |
| `SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)` | `op w` | префиксные унарные (`-`, `~`, `++`, …) |
| `SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` | постфиксные унарные (`++`, `--`) |
| `SCL_REFLECT_SUBSCRIPT_OPERATOR(op, name)` | `w[i]` | `operator[]`, в том числе для указателя |
| `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)` | `w op x` | обязательно-member операторы (`=`, `*=`, …) |
| `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(op, name)` | `op w` | обязательно-member префиксные (`->`, `&`, `*`) |
| `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` | обязательно-member постфиксные |
| `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)` | `w op x` только через ADL | ADL-доступные бинарные |
| `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(op, name)` | `op w` только через ADL | ADL-доступные префиксные |
| `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(op, name)` | `w op` только через ADL | ADL-доступные постфиксные |
| `SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name)` | `w(...)` | `()`, `->*` (сторона значения — только member) |

`SCL_REFLECT_TYPE` должен быть объявлен в том же теле класса до любого макроса операторов.

---

## Что отражается

Отражённый оператор применяет оператор к **обёрнутому значению** как выражение —
`value op arg`, `op value`, `value[index]`, — а не вызывает конкретный member. Обычное
разрешение перегрузки этого выражения затем выбирает собственный оператор значения:

- **member**-оператор значения, если он есть (то есть member-оператор имеет приоритет
  над свободным); либо
- **свободный или встроенный** оператор в противном случае.

Поскольку встроенные операторы фундаментальных и указательных типов достигаются через это
же выражение, они тоже отражаются: `wrapper<int>{} + 5`, `-w`, `w1 * w2` и
`wrapper<int*>{arr}[2]` работают. cv-ref-квалификатор wrapper передаётся значению, поэтому
перегрузки значения `&` / `const &` / … выбираются согласно квалификации wrapper.

Аргумент бинарного/индексного оператора достигается на один уровень вниз под guard'ом
исполнителя: аргумент-wrapper (`w1 op w2`) разворачивается один раз, а `guard()`/`unguard()`
срабатывают вокруг чтения; обычный аргумент проходит без изменений.

---

## Направление (бинарные операторы)

| Выражение | Перегрузка | Отражает |
|-----------|------------|----------|
| `w op x` (wrapper слева) | member левого wrapper | `value op x` |
| `x op w` (wrapper справа, `x` — не wrapper) | реверсный hidden-friend | `x op value` |
| `w1 op w2` (оба wrapper) | member левого wrapper | `value1 op value2` |

Реверсный friend отсекается, когда левый операнд сам является wrapper (`is_wrapper_v`),
поэтому `w1 op w2` разрешается в перегрузку левого wrapper без неоднозначности.

```cpp
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, MyValue>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_BINARY_OPERATOR(+, plus)        // w + x, x + w, w1 + w2
    SCL_REFLECT_EQUALITY_OPERATOR(==, equal_to) // w == x, x == w (bool)
};
```

У унарных операторов один операнд, поэтому реверсного случая нет. `SCL_REFLECT_EQUALITY_OPERATOR`
возвращает `bool`, чтобы реверсное сравнение C++20 `x == w` формировалось из member обёртки.

---

## Путь (override исполнителя или execute)

| Путь | Условие | Вызывает | Возвращает |
|------|---------|----------|------------|
| *executor-override* | существует `Executor::operator_<name>(exec, args...)` | этот статический член напрямую, с **сырыми** операндами | тип возврата override |
| *execute-path* | override отсутствует | `Executor::execute(exec, callable, exec, args...)`; callable применяет оператор к значению, разрешённому внутри `execute()` | тип отражённого выражения |

Соглашение override применяется только к форме «wrapper слева» / унарной; у реверсного friend
пути override нет, но он проходит через `Executor::execute`, поэтому сквозное поведение работает
в обе стороны. Каждая перегрузка объявлена `decltype(auto)` и возвращает результат дословно —
не переоборачивая.

---

## Только member-варианты (`SCL_REFLECT_MEMBER_*`)

Стандарт C++ ([over.oper]) требует, чтобы `operator=`, `operator->`, `operator()`, `operator[]`
и составные присваивания были нестатическими member-функциями **на wrapper**; сторона *значения*
при этом может оставаться свободным или встроенным оператором. `operator&` и `operator*` тоже
оставлены только-member — свободный fallback для `&` сделал бы `&wrapper` рефлексией `&value`
и перехватил бы синтаксис «указатель на wrapper».

```cpp
SCL_REFLECT_MEMBER_BINARY_OPERATOR(+=, plus_assign)
SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(->, arrow)
```

`reflect_operators` (базовый класс `scl::wrapper` по умолчанию) использует их для `*=`, `/=`,
`%=`, `+=`, `-=`, `<<=`, `>>=`, `&=`, `|=`, `^=`, `->`, `&`, `*`.

---

## Только friend-варианты (`SCL_REFLECT_FRIEND_*`)

Отражают **свободный** оператор значения как hidden-friend обёртки: находится через ADL
(`w op x`), но не через синтаксис member `w.operator op(...)`. Не совмещайте с макросом
`SCL_REFLECT_*_OPERATOR` для того же `name` — оба генерируют одни и те же классовые хелперы,
что даёт дублирование определений.

---

## Примеры

### Симметричное сравнение

```cpp
struct Length { bool operator==(Length) const &; };

template <typename W, typename E>
class scl::feature::reflect<W, E, Length>
{
    SCL_REFLECT_TYPE(W, E)
    SCL_REFLECT_EQUALITY_OPERATOR(==, equal_to)
};

scl::wrapper<Length> a, b;
bool r1 = (a == b);          // value == value (оба wrapper)
bool r2 = (a == Length{});   // value == x     (wrapper слева)
bool r3 = (Length{} == a);   // x == value     (реверсный friend)
```

### Фундаментальный тип значения

```cpp
scl::wrapper<int> n{40};
int  s = n + 2;   // 42 — встроенный int + int, отражён
bool c = n < 50;  // встроенное сравнение
int  m = -n;      // встроенное отрицание

scl::wrapper<int *> p{arr};
int e = p[2];     // встроенный индекс указателя
```

### Обязательно-member составное присваивание

```cpp
scl::wrapper<int> c{0};
c += 42;   // отражает int += int через собственный operator+= обёртки
```

> Обычный `=` — другое дело. У `scl::wrapper` копирующее/перемещающее/преобразующее
> присваивание — **собственный** оператор wrapper (он обязан подавить неявно удалённые
> copy/move-присваивания; см. [wrapper](../wrapper/wrapper.md)), который скрывает любой
> отражённый `operator=`.

---

## См. также

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
