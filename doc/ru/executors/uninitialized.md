# inplace::uninitialized

Исполнитель с выровненным сырым хранилищем и отложенной конструкцией объекта.

- Заголовок: `#include <scl/feature/inplace/uninitialized.h>`
- Объявление: `template <typename T> struct uninitialized;`
- Пространство имён: `scl::feature::inplace`

---

## Описание

`inplace::uninitialized<T>` резервирует память для объекта `T`, не конструируя его.
Конструирование возлагается на пользователя — как правило через placement-new
в `m_storage`. После этого `value()` предоставляет доступ к объекту через
`reinterpret_cast` сырого хранилища.

Исполнитель удобен для типов с дорогостоящим конструктором по умолчанию или
для объектов, порядок инициализации которых строго задан.

Как и `inplace::plain`, не предоставляет `guard()`/`unguard()` и не несёт
накладных расходов на синхронизацию.

## Интерфейс

```cpp
template <typename T>
struct uninitialized
{
    using value_type = T;

    alignas(T) std::byte m_storage[sizeof(T)];

    // Возвращает ссылку на хранимый объект через reinterpret_cast.
    // Поведение не определено, если объект не был сконструирован.
    template <typename Self>
    static constexpr decltype(auto) value(Self && self);

    // Немедленно вызывает func(args...) и возвращает результат.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);

    // Побайтовое копирование хранилища — 8 перегрузок на cv-ref квалификацию.
    // Копирует байты через std::ranges::copy вне зависимости от того,
    // был ли T сконструирован. Оператор присваивания T не вызывается.
    constexpr uninitialized & operator=(uninitialized cv_ref other);  // × 8
};
```

## Примеры

### Отложенное конструирование

```cpp
#include <scl/feature/inplace/uninitialized.h>
#include <string>

scl::feature::inplace::uninitialized<std::string> e;

// Конструируем объект вручную, когда готово:
new (&e.m_storage) std::string{"hello"};

std::string & s = scl::feature::inplace::uninitialized<std::string>::value(e);
// s == "hello"

// Вручную уничтожаем, когда больше не нужно:
s.~basic_string();
```

### В качестве исполнителя обёртки

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/inplace/uninitialized.h>

scl::wrapper<std::string, scl::feature::inplace::uninitialized> w{"world"};
```

## См. также

- [inplace::plain](plain.md)
- [wrapper](../wrapper/wrapper.md)
