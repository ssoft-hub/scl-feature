# wrapper_lock

Ленивая RAII-блокировка одного слоя обёртки: `guard()` вызывается только
при явном вызове `lock()`.

- Заголовок: `#include <scl/feature/wrapper_lock.h>`
- Объявление: `template <typename Refer> class wrapper_lock;`
- Пространство имён: `scl`

---

## Описание

`scl::wrapper_lock<Refer>` при создании сохраняет ссылку на обёртку, не устанавливая
блокировку. Блокировка устанавливается по требованию вызовом `lock()` и снимается
вызовом `unlock()` или деструктором (если к этому моменту не снята).

`lock()` идемпотентен: повторный вызов при уже установленной блокировке — холостая
операция. `unlock()` тоже идемпотентен.

Для исполнителей без `guard()`/`unguard()` оба метода — холостые операции;
для не-wrapper типов весь класс прозрачен.

`Refer` должен быть ссылочным типом. Объект некопируемый и неперемещаемый.

## Интерфейс

```cpp
template <typename Refer>
class wrapper_lock
{
public:
    wrapper_lock(wrapper_lock &&) = delete;
    wrapper_lock(wrapper_lock const &) = delete;

    // Сохраняет ссылку. Блокировка не устанавливается.
    constexpr explicit wrapper_lock(Refer ref) noexcept;

    // Снимает блокировку, если она установлена.
    constexpr ~wrapper_lock() noexcept(...);

    // Устанавливает блокировку (вызывает executor::guard). Идемпотентен.
    constexpr void lock() noexcept(...);

    // Снимает блокировку (вызывает executor::unguard). Идемпотентен.
    constexpr void unlock() noexcept(...);

    // Возвращает ссылку на wrapper (блокировка не требуется).
    [[nodiscard]] constexpr Refer wrapper_value() const noexcept;

    // Возвращает внутреннее значение через исполнитель (блокировка должна быть установлена).
    [[nodiscard]] constexpr decltype(auto) value() const noexcept(...);
};
```

## Примеры

### Условная блокировка

```cpp
#include <scl/feature/wrapper_lock.h>
#include <scl/feature/wrapper.h>

scl::wrapper<int, mutex_executor> w{0};

scl::wrapper_lock<decltype(w) &> lk{w};  // блокировка ещё не установлена

if (needs_write) {
    lk.lock();             // блокировка устанавливается здесь
    lk.value() = 42;
    lk.unlock();           // блокировка снимается здесь
}
// деструктор снимет блокировку, если она ещё удерживается
```

### Роль в value_lock

`wrapper_lock` — строительный блок `value_lock`: каждому слою цепочки обёрток
соответствует свой `wrapper_lock`, а `value_lock::lock_for<Target>()` активирует
только те из них, которые нужны для достижения целевого типа.

## См. также

- [wrapper_guard](wrapper_guard.md)
- [value_lock](value_lock.md)
