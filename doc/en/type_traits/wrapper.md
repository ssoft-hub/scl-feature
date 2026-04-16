# Wrapper type traits

Compile-time predicates for `wrapper` specialisations and their relationships.

- Header: `#include <scl/feature/type_traits/wrapper.h>`
- Namespace: `scl::feature`

Contents:
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

`true` if `T` (after stripping cv-ref qualifiers) is a `wrapper` specialisation,
`false` otherwise.

```cpp
static_assert( is_wrapper_v<wrapper<int, inplace::plain>>);
static_assert( is_wrapper_v<wrapper<int, inplace::plain> const &>);  // cv-ref stripped
static_assert(!is_wrapper_v<int>);
static_assert(!is_wrapper_v<void>);
```

---

## is_compatible_with_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_compatible_with_v;
```

`true` if `Test` can be used in place of `Expected`. For non-wrapper types the
check is same-type or public derivation. For wrapper specialisations sharing the
same executor template the check recurses into the value types. cv-ref qualifiers
are stripped from both arguments.

```cpp
using P = wrapper<int, inplace::plain>;
using D = wrapper<Derived, inplace::plain>;   // Derived : Base

static_assert( is_compatible_with_v<P, P>);
static_assert( is_compatible_with_v<int, int>);
static_assert( is_compatible_with_v<wrapper<Base, inplace::plain>, D>);
static_assert(!is_compatible_with_v<P, wrapper<int, inplace::uninitialized>>); // different executor
```

---

## is_compatible_with_part_of_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_compatible_with_part_of_v;
```

`true` if `Expected` (a wrapper) recursively contains a value that is compatible
with the whole of `Test` (also a wrapper). Returns `false` if either argument is
not a wrapper. cv-ref qualifiers are stripped.

```cpp
using W   = wrapper<int,  inplace::plain>;
using WW  = wrapper<W,    inplace::plain>;
using WWW = wrapper<WW,   inplace::plain>;

static_assert( is_compatible_with_part_of_v<WW,  W>);   // WW's value IS W
static_assert( is_compatible_with_part_of_v<WWW, W>);   // two levels deep
static_assert(!is_compatible_with_part_of_v<W,  WW>);   // W does not contain WW
```

---

## is_part_compatible_with_v

```cpp
template <typename Expected, typename Test>
inline constexpr bool is_part_compatible_with_v;
```

`true` if `Test` (a wrapper) recursively contains a value compatible with the
whole of `Expected` (also a wrapper). The roles of the two parameters are swapped
compared to `is_compatible_with_part_of_v`.

```cpp
static_assert( is_part_compatible_with_v<W,  WW>);    // WW's value IS W
static_assert( is_part_compatible_with_v<W,  WWW>);   // two levels deep
static_assert(!is_part_compatible_with_v<WW, W>);     // W does not contain WW
```

---

## is_convertible_from_v

```cpp
template <typename Target, typename Refer>
inline constexpr bool is_convertible_from_v;
```

`true` if `Target` is reachable from `Refer` directly (`std::is_convertible`) or
by traversing any number of wrapper layers. This is the condition under which
`value_lock<Refer>::lock_for<Target>()` and `value_lock<Refer>::value_as<Target>()`
are well-formed.

```cpp
using W  = wrapper<int, inplace::plain>;
using WW = wrapper<W,   inplace::plain>;

static_assert( is_convertible_from_v<W  &, WW &>);   // one layer
static_assert( is_convertible_from_v<int&, WW &>);   // two layers
static_assert(!is_convertible_from_v<double&, WW &>);
```

## See also

- [Executor traits](executor.md)
- [Concepts](../concepts/concepts.md)
