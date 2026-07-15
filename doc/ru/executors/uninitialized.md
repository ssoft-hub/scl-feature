# inplace::uninitialized

Исполнитель с выровненным сырым хранилищем и отложенной конструкцией объекта.

- Заголовок: `#include <scl/feature/executor/inplace/uninitialized.h>`
- Объявление: `template <typename T> struct uninitialized;`
- Пространство имён: `scl::feature::inplace`

---

## Описание

`inplace::uninitialized<T>` резервирует корректно выровненное хранилище нужного
размера под объект `T`, не конструируя его. `access()` возвращает ссылку на это
хранилище через `reinterpret_cast`; поведение не определено до тех пор, пока в
хранилище не начнётся время жизни объекта `T`.

Хранилище **приватное**, и исполнитель **не** предоставляет публичных
`construct()` / `destroy()` и конструктора, принимающего значение. Как следствие:

- Для типа с неявным временем жизни (например, тривиально копируемого `int`)
  значение помещается присваиванием через обёртку — это начинает время жизни
  объекта.
- Для нетривиального типа (например, `std::string`) публичного способа
  сконструировать или разрушить хранимый объект сейчас **нет**, поэтому такая
  специализация осмысленна только на уровне типов — вложенность обёрток и запросы
  свойств, — но не для хранения живого значения. (Отсутствие API конструирования —
  известное ограничение.)

Как и `inplace::plain`, не предоставляет `guard()`/`unguard()` и не несёт
накладных расходов на синхронизацию.

## Интерфейс

```cpp
template <typename T>
struct uninitialized
{
    using value_type = T;

    // Конструктор по умолчанию — хранилище остаётся неинициализированным.
    constexpr uninitialized() noexcept = default;

    // Побайтовый копирующий конструктор — 8 перегрузок на cv-ref квалификацию.
    // Копирует байты через std::ranges::copy; ограничен std::is_trivially_copyable_v<T>.
    // Конструктор T НЕ вызывается.
    constexpr explicit uninitialized(uninitialized cv_ref other) noexcept;  // × 8

    // Собственного присваивания у исполнителя нет — операторы присваивания удалены.
    uninitialized & operator=(uninitialized const &) = delete;
    uninitialized & operator=(uninitialized &&) = delete;

    // Возвращает ссылку на хранимый объект через reinterpret_cast.
    // Поведение не определено, если время жизни объекта ещё не началось.
    template <typename Self>
    static constexpr decltype(auto) access(Self && self);

    // Немедленно вызывает func(args...) и возвращает результат.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);

private:
    alignas(T) std::byte m_storage[sizeof(T)];  // приватное
};
```

## Примеры

### Резервирование хранилища

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/uninitialized.h>
#include <string>

// Хранилище зарезервировано; значение не сконструировано.
scl::wrapper<int, scl::feature::inplace::uninitialized> w_int{};
scl::wrapper<std::string, scl::feature::inplace::uninitialized> w_string{};
```

Для типа с неявным временем жизни (например, `int`) значение помещается
присваиванием через обёртку, что начинает время жизни объекта; чтение до записи
значения — неопределённое поведение. Для нетривиального типа (например,
`std::string`) публичного способа конструирования нет, поэтому такая специализация
осмысленна только на уровне типов (вложенность, запросы свойств).

## См. также

- [inplace::plain](plain.md)
- [wrapper](../wrapper/wrapper.md)
