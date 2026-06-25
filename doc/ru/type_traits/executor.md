# Свойства типов исполнителей

Предикаты времени компиляции для обнаружения интерфейса исполнителя
и проверки noexcept.

- Заголовок: `#include <scl/feature/type_traits/executor.h>`
- Пространство имён: `scl::feature`

Содержание:
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

`true`, если `T` (без cv-ref-квалификаторов) удовлетворяет интерфейсу исполнителя:
`T::access(Self&&)` и `T::execute(Self&&, Func&&)` должны быть вызываемы для всех
трёх основных категорий значения (`T&`, `T&&`, `T const&`).
Для не-классовых типов всегда `false`.

```cpp
static_assert( is_executor_v<inplace::plain<int>>);
static_assert( is_executor_v<inplace::plain<int> const &>);  // cv-ref снимаются
static_assert(!is_executor_v<int>);
static_assert(!is_executor_v<void>);
```

---

## has_access_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_access_v;
```

`true`, если `ExecutorType` имеет статический метод `access()`, первый (и единственный)
параметр которого — в точности `ExecutorRefer`. Для обнаружения используется
приведение к указателю на функцию, что исключает неявные преобразования.

---

## has_execute_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_execute_v;
```

`true`, если `ExecutorType` имеет статический метод `execute()`, первый параметр
которого — в точности `ExecutorRefer`. Вызываемый параметр зондируется
через `void(*&)()`.

---

## has_guard_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_guard_v;
```

`true`, если `ExecutorType` имеет статический метод `guard()`, первый параметр
которого — в точности `ExecutorRefer`.

---

## has_unguard_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool has_unguard_v;
```

`true`, если `ExecutorType` имеет статический метод `unguard()`, первый параметр
которого — в точности `ExecutorRefer`.

---

## is_guard_noexcept_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool is_guard_noexcept_v;
```

`true`, если `ExecutorType::guard(ExecutorRefer)` отсутствует или помечен `noexcept`.
Используется для распространения `noexcept` через утилиты блокировки.

---

## is_unguard_noexcept_v

```cpp
template <typename ExecutorType, typename ExecutorRefer>
inline constexpr bool is_unguard_noexcept_v;
```

`true`, если `ExecutorType::unguard(ExecutorRefer)` отсутствует или помечен `noexcept`.

---

## executor_trait

```cpp
template <typename T>
struct executor_trait;
```

Точка настройки для доступа к исполнителю внутри wrapper-типа.
Реализации по умолчанию нет — каждый wrapper-тип должен предоставить
специализацию со статическим методом `executor(Self&&)`, возвращающим
cv-ref-квалифицированную ссылку на исполнитель.

`detail::wrapper` предоставляет эту специализацию автоматически.

```cpp
// Пример специализации для пользовательского wrapper:
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

## См. также

- [Свойства типов wrapper](wrapper.md)
- [Концепты](../concepts/concepts.md)
