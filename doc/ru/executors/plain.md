# inplace::plain

Исполнитель с прямым хранением значения в памяти, без каких-либо накладных расходов.

- Заголовок: `#include <scl/feature/inplace/plain.h>`
- Объявление: `template <typename T> struct plain;`
- Пространство имён: `scl::feature::inplace`

---

## Описание

`inplace::plain<T>` — исполнитель, применяемый в `scl::wrapper` по умолчанию.
Хранит объект `T` непосредственно как поле данных: без косвенного обращения,
без аллокаций в куче, без синхронизации.

`execute(self, func, args...)` немедленно вызывает `func(args...)` без какого-либо
перехвата. Поскольку `plain` не предоставляет `guard()`/`unguard()`, утилиты
блокировки обрабатывают его как прозрачный слой.

## Интерфейс

```cpp
template <typename T>
struct plain
{
    using value_type = T;

    T m_value;

    // Возвращает ссылку на хранимое значение, сохраняя cv-ref квалификаторы.
    template <typename Self>
    static constexpr decltype(auto) value(Self && self);

    // Немедленно вызывает func(args...) и возвращает результат.
    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args);
};
```

## Примеры

### Прямое использование

```cpp
#include <scl/feature/inplace/plain.h>

scl::feature::inplace::plain<int> e{42};

int & ref = scl::feature::inplace::plain<int>::value(e);         // ref == 42
int   val = scl::feature::inplace::plain<int>::execute(e, [&]{ return e.m_value * 2; }); // 84
```

### В качестве исполнителя обёртки

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/inplace/plain.h>

scl::wrapper<std::string, scl::feature::inplace::plain> w{"hello"};
```

## См. также

- [inplace::uninitialized](uninitialized.md)
- [wrapper](../wrapper/wrapper.md)
