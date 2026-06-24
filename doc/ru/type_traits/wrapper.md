# Свойства типов wrapper

Предикаты времени компиляции для проверки специализаций `wrapper` и их взаимосвязей.

- Заголовок: `#include <scl/feature/type_traits/wrapper.h>`
- Пространство имён: `scl::feature`

Содержание:
- [`is_wrapper_v<T>`](#is_wrapper_v)
- [`is_compatible_with_v<Expected, Test>`](#is_compatible_with_v)
- [`is_compatible_with_part_of_v<Expected, Test>`](#is_compatible_with_part_of_v)
- [`is_part_compatible_with_v<Expected, Test>`](#is_part_compatible_with_v)
- [`is_convertible_from_v<Target, Refer>`](#is_convertible_from_v)

---

## is_wrapper_v

```cpp
template <typename T>
inline constexpr bool is_wrapper_v;
```

`true`, если `T` (без cv-ref-квалификаторов) является специализацией `wrapper`,
иначе `false`.

```cpp
static_assert( is_wrapper_v<wrapper<int, inplace::plain>>);
static_assert( is_wrapper_v<wrapper<int, inplace::plain> const &>);  // cv-ref снимаются
static_assert(!is_wrapper_v<int>);
static_assert(!is_wrapper_v<void>);
```

---

## is_compatible_with_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_compatible_with_v;
```

`true`, если `Test` может использоваться вместо `Expected`. Для не-wrapper типов
проверяется совпадение или открытое наследование. Для специализаций wrapper с
одинаковым шаблоном исполнителя проверка применяется рекурсивно к типам значений.
cv-ref-квалификаторы обоих аргументов отбрасываются.

```cpp
using P = wrapper<int, inplace::plain>;
using D = wrapper<Derived, inplace::plain>;   // Derived : Base

static_assert( is_compatible_with_v<P, P>);
static_assert( is_compatible_with_v<int, int>);
static_assert( is_compatible_with_v<wrapper<Base, inplace::plain>, D>);
static_assert(!is_compatible_with_v<P, wrapper<int, inplace::uninitialized>>); // разные исполнители
```

---

## is_compatible_with_part_of_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_compatible_with_part_of_v;
```

`true`, если `Expected` (wrapper) рекурсивно содержит значение, совместимое
с целым `Test` (тоже wrapper). Если один из аргументов не является wrapper —
`false`. cv-ref-квалификаторы отбрасываются.

```cpp
using W   = wrapper<int,  inplace::plain>;
using WW  = wrapper<W,    inplace::plain>;
using WWW = wrapper<WW,   inplace::plain>;

static_assert( is_compatible_with_part_of_v<WW,  W>);   // значение WW — это W
static_assert( is_compatible_with_part_of_v<WWW, W>);   // два уровня вглубь
static_assert(!is_compatible_with_part_of_v<W,  WW>);   // W не содержит WW
```

---

## is_part_compatible_with_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_part_compatible_with_v;
```

`true`, если `Test` (wrapper) рекурсивно содержит значение, совместимое
с целым `Expected` (тоже wrapper). Роли аргументов обратны по отношению к
`is_compatible_with_part_of_v`.

```cpp
static_assert( is_part_compatible_with_v<W,  WW>);    // значение WW — это W
static_assert( is_part_compatible_with_v<W,  WWW>);   // два уровня вглубь
static_assert(!is_part_compatible_with_v<WW, W>);     // W не содержит WW
```

---

## is_convertible_from_v

```cpp
template <typename Target, typename Refer>
inline constexpr bool is_convertible_from_v;
```

`true`, если `Target` достижим из `Refer` напрямую (`std::is_convertible`) или
через обход любого числа слоёв wrapper. Является предусловием для вызовов
`value_lock<Refer>::lock_for<Target>()` и `value_lock<Refer>::value_as<Target>()`.

```cpp
using W  = wrapper<int, inplace::plain>;
using WW = wrapper<W,   inplace::plain>;

static_assert( is_convertible_from_v<W  &, WW &>);   // один слой
static_assert( is_convertible_from_v<int&, WW &>);   // два слоя
static_assert(!is_convertible_from_v<double&, WW &>);
```

## См. также

- [Свойства исполнителей](executor.md)
- [Концепты](../concepts/concepts.md)
