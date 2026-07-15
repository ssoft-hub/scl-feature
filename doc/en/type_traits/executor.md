# Executor type traits

Compile-time predicates for executor interface detection and noexcept inspection.

- Header: `#include <scl/feature/type_traits/executor.h>`
- Namespace: `scl::feature`

Contents:
- [`is_executor_v<T>`](#is_executor_v)
- [`has_access_v<E, Self>`](#has_access_v)
- [`has_execute_v<E, Self>`](#has_execute_v)
- [`has_guard_v<E, Self>`](#has_guard_v)
- [`has_unguard_v<E, Self>`](#has_unguard_v)
- [`is_guard_noexcept_v<E, Self>`](#is_guard_noexcept_v)
- [`is_unguard_noexcept_v<E, Self>`](#is_unguard_noexcept_v)
- [`executor_trait<T>`](#executor_trait)

---

## is_executor_v

```cpp
template <typename T>
inline constexpr bool is_executor_v;
```

`true` if `T` (after stripping cv-ref qualifiers) satisfies the executor
interface: `T::access(Self&&)` and `T::execute(Self&&, Func&&)` must be callable
for all three primary value categories (`T&`, `T&&`, `T const&`). Non-class
types always yield `false`.

```cpp
static_assert( is_executor_v<inplace::plain<int>>);
static_assert( is_executor_v<inplace::plain<int> const &>);  // cv-ref stripped
static_assert(!is_executor_v<int>);
static_assert(!is_executor_v<void>);
```

> **Guard reentrancy contract.** The optional `guard()` / `unguard()` methods
> must allow nested (reentrant) acquisition: a single expression may guard the
> same executor more than once (`w == w`, `a = a`, one wrapper passed as several
> arguments), so a bare non-recursive `std::mutex` self-deadlocks — use a
> counting or recursive lock. When no `unguard()` exists for a cv-ref
> qualification, `guard()` must be self-contained.

---

## has_access_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_access_v;
```

`true` if `ExecutorType` has a static `access()` method whose first (and only)
parameter type is exactly `ExecutorRefer`. Detection uses a function-pointer cast
that rejects implicit conversions.

---

## has_execute_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_execute_v;
```

`true` if `ExecutorType` has a static `execute()` method whose first parameter
type is exactly `ExecutorRefer`. The callable parameter is probed as
`void(*&)()`.

---

## has_guard_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_guard_v;
```

`true` if `ExecutorType` has a static `guard()` method whose first parameter
type is exactly `ExecutorRefer`.

---

## has_unguard_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_unguard_v;
```

`true` if `ExecutorType` has a static `unguard()` method whose first parameter
type is exactly `ExecutorRefer`.

---

## is_guard_noexcept_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool is_guard_noexcept_v;
```

`true` if `ExecutorType::guard(ExecutorRefer)` either does not exist or is marked
`noexcept`. Used to propagate `noexcept` through locking utilities.

---

## is_unguard_noexcept_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool is_unguard_noexcept_v;
```

`true` if `ExecutorType::unguard(ExecutorRefer)` either does not exist or is
marked `noexcept`.

---

## executor_trait

```cpp
template <typename T>
struct executor_trait;
```

Customisation point that provides access to the executor inside a wrapper type.
No default implementation — every wrapper type must provide a specialisation with
a static `executor(Self&&)` method returning a reference to the executor with
preserved cv-ref qualifiers.

`detail::wrapper` provides this specialisation automatically.

```cpp
// Example specialisation for a custom wrapper:
template <>
struct scl::feature::executor_trait<MyWrapper>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    {
        return ::scl::forward_like<Self>(self.m_executor);
    }
};
```

## See also

- [Wrapper traits](wrapper.md)
- [Concepts](../concepts/concepts.md)
