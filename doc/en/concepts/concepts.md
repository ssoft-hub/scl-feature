# Concepts

Compile-time concepts for executor and wrapper validation.

- Header: `#include <scl/feature/concepts.h>`
- Namespace: `scl::feature::concepts`

Contents:
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

Satisfied when `T` (after stripping cv-ref qualifiers) satisfies the executor
interface: `access()` and `execute()` callable for all three primary value
categories. Wraps `is_executor_v<T>`.

```cpp
static_assert( concepts::executor<inplace::plain<int>>);
static_assert(!concepts::executor<int>);
```

---

## wrapper

```cpp
template <typename T>
concept wrapper;
```

Satisfied when `T` (after stripping cv-ref qualifiers) is a `wrapper`
specialisation. Wraps `is_wrapper_v<T>`.

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

Satisfied when `Target` is reachable from `Refer` directly or by traversing
any number of wrapper layers. Wraps `is_convertible_from_v<Target, Refer>`.

Used as the constraint on `value_lock::lock_for<Target>()` and
`wrapper_caster::to<Target>()`.

```cpp
using W  = scl::wrapper<int, inplace::plain>;
using WW = scl::wrapper<W,   inplace::plain>;

static_assert( concepts::convertible_from<int &, WW &>);  // two layers
static_assert(!concepts::convertible_from<double &, WW &>);
```

---

## compatible_with

```cpp
template <typename Expected, typename T>
concept compatible_with;
```

Satisfied when `T` can be used in place of `Expected`: same type, public
derivation, or — for wrapper specialisations with the same executor — recursive
compatibility of value types. Wraps `is_compatible_with_v<Expected, T>`.

---

## compatible_with_part_of

```cpp
template <typename Expected, typename T>
concept compatible_with_part_of;
```

Satisfied when `Expected` (a wrapper) recursively contains a value compatible
with the whole of `T`. Wraps `is_compatible_with_part_of_v<Expected, T>`.

---

## part_compatible_with

```cpp
template <typename Expected, typename T>
concept part_compatible_with;
```

Satisfied when `T` (a wrapper) recursively contains a value compatible with the
whole of `Expected`. Wraps `is_part_compatible_with_v<Expected, T>`.

---

## See also

- [Wrapper traits](../type_traits/wrapper.md)
- [Executor traits](../type_traits/executor.md)
