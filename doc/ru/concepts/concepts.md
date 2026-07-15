# Концепты

Концепты времени компиляции для проверки исполнителей и обёрток.

- Заголовок: `#include <scl/feature/concepts.h>`
- Пространство имён: `scl::feature::concepts`

Содержание:
- [`executor<T>`](#executor)
- [`wrapper<T>`](#wrapper)
- [`convertible_from<Target, Refer>`](#convertible_from)
- [`compatible_with<Expected, T>`](#compatible_with)
- [`compatible_with_part_of<Expected, T>`](#compatible_with_part_of)
- [`part_compatible_with<Expected, T>`](#part_compatible_with)

---

## executor

```cpp
template <typename T>
concept executor;
```

Истинен, если `T` (без cv-ref-квалификаторов) удовлетворяет интерфейсу исполнителя:
`access()` и `execute()` должны быть вызываемы для всех трёх основных категорий значения.
Надстройка над `is_executor_v<T>`.

```cpp
static_assert( concepts::executor<inplace::plain<int>>);
static_assert(!concepts::executor<int>);
```

**Контракт реентерабельности захвата.** Необязательные методы `guard()` /
`unguard()` обязаны допускать повторный (вложенный) захват. Одно выражение может
захватить блокировку одного исполнителя несколько раз — `w == w`, `a = a` или
одна обёртка, переданная несколькими аргументами, захватывают свой исполнитель
независимо, а отражённые операторы захватывают оба операнда. Реализуйте захват
счётчиком или рекурсивной блокировкой; голый нерекурсивный `std::mutex` приводит
к самоблокировке. Если для данной cv-ref-квалификации метода `unguard()` нет,
`guard()` обязан быть самодостаточным (освобождать нечего).

---

## wrapper

```cpp
template <typename T>
concept wrapper;
```

Истинен, если `T` (без cv-ref-квалификаторов) является специализацией `wrapper`.
Надстройка над `is_wrapper_v<T>`.

```cpp
static_assert( concepts::wrapper<scl::wrapper<int, inplace::plain>>);
static_assert(!concepts::wrapper<int>);
```

---

## convertible_from

```cpp
template <typename Target, typename Refer>
concept convertible_from;
```

Истинен, если `Target` достижим из `Refer` напрямую или через обход любого числа
слоёв wrapper. Надстройка над `is_convertible_from_v<Target, Refer>`.

Используется как ограничение в `value_lock::lock_for<Target>()`
и `wrapper_caster::to<Target>()`.

```cpp
using W  = scl::wrapper<int, inplace::plain>;
using WW = scl::wrapper<W,   inplace::plain>;

static_assert( concepts::convertible_from<int &, WW &>);  // два слоя
static_assert(!concepts::convertible_from<double &, WW &>);
```

---

## compatible_with

```cpp
template <typename Expected, typename T>
concept compatible_with;
```

Истинен, если `T` может использоваться вместо `Expected`: совпадение типов,
открытое наследование или — для специализаций wrapper с одним исполнителем —
рекурсивная совместимость типов значений. Надстройка над `is_compatible_with_v`.

---

## compatible_with_part_of

```cpp
template <typename Expected, typename T>
concept compatible_with_part_of;
```

Истинен, если `Expected` (wrapper) рекурсивно содержит значение, совместимое
с целым `T`. Надстройка над `is_compatible_with_part_of_v<Expected, T>`.

---

## part_compatible_with

```cpp
template <typename Expected, typename T>
concept part_compatible_with;
```

Истинен, если `T` (wrapper) рекурсивно содержит значение, совместимое
с целым `Expected`. Роли аргументов обратны по отношению к `compatible_with_part_of`.
Надстройка над `is_part_compatible_with_v<Expected, T>`.

---

## См. также

- [Свойства типов wrapper](../type_traits/wrapper.md)
- [Свойства типов исполнителей](../type_traits/executor.md)
