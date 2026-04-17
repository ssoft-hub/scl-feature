# wrapper_guard

RAII-охранник: вызывает `guard()` у исполнителя при создании
и `unguard()` при уничтожении.

- Заголовок: `#include <scl/feature/wrapper_guard.h>`
- Объявление: `template <typename Refer> class wrapper_guard;`
- Пространство имён: `scl`

---

## Описание

`scl::wrapper_guard<Refer>` — некопируемый и неперемещаемый RAII-объект.
При конструировании из ссылки на wrapper (или обычное значение) немедленно
вызывает `guard()` у исполнителя, если тот его предоставляет; при уничтожении
симметрично вызывает `unguard()`.

Для обычных значений и для исполнителей без `guard()`/`unguard()` охранник
является холостой операцией без накладных расходов.

`Refer` должен быть ссылочным типом.

## Интерфейс

```cpp
template <typename Refer>
class wrapper_guard
{
public:
    wrapper_guard(wrapper_guard &&) = delete;
    wrapper_guard(wrapper_guard const &) = delete;

    // Сразу вызывает guard() у исполнителя.
    constexpr explicit wrapper_guard(Refer ref)
        noexcept(is_guard_noexcept_v<...>);

    // Вызывает unguard() у исполнителя.
    constexpr ~wrapper_guard()
        noexcept(is_unguard_noexcept_v<...>);

    // Возвращает ссылку на хранимое значение через исполнитель.
    [[nodiscard]]
    constexpr decltype(auto) value() const;
};
```

## Примеры

### Охранник для wrapper с блокирующим исполнителем

```cpp
#include <scl/feature/wrapper_guard.h>
#include <scl/feature/wrapper.h>

scl::wrapper<int, mutex_executor> w{42};

{
    scl::wrapper_guard<decltype(w) &> guard{w};
    // мьютекс захвачен
    int & v = guard.value();  // доступ к значению
    v = 100;
}   // мьютекс освобождён
```

### Холостая операция для plain-исполнителя

```cpp
scl::wrapper<int, scl::feature::inplace::plain> w{7};
scl::wrapper_guard<decltype(w) &> guard{w};  // вызовов guard/unguard нет
int & v = guard.value();
```

## См. также

- [wrapper_lock](wrapper_lock.md)
- [value_lock](value_lock.md)
