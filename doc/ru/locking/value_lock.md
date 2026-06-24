# value_lock

Рекурсивная ленивая блокировка всей цепочки обёрток.

- Заголовок: `#include <scl/feature/value_lock.h>`
- Объявление: `template <typename Refer> class value_lock;`
- Пространство имён: `scl`

---

## Описание

При конструировании `scl::value_lock<Refer>` запоминает исполнители каждого слоя
цепочки обёрток — без каких-либо блокировок. Блокировки устанавливаются явно
через `lock_for<Target>()`: метод затрагивает ровно те слои, которые необходимы
для доступа к `Target`. Значение читается через `value_as<Target>()`.

Тем самым один объект `value_lock` покрывает разные уровни доступа: для обращения
к внешней обёртке блокировать ничего не нужно, тогда как чтение вложенного
значения последовательно блокирует все промежуточные слои.

`Refer` должен быть ссылочным типом. `Target` должен удовлетворять концепту
`concepts::convertible_from<Target, Refer>`.
Объект некопируемый и неперемещаемый.

## Интерфейс

```cpp
template <typename Refer>
class value_lock
{
public:
    value_lock(value_lock &&) = delete;
    value_lock(value_lock const &) = delete;

    // Запоминает исполнители всех слоёв. Блокировки не устанавливаются.
    constexpr explicit value_lock(Refer ref) noexcept(...);

    constexpr ~value_lock() = default;

    // Блокирует слои, необходимые для доступа к Target.
    // Если Target напрямую доступен из Refer — блокировок нет.
    template <typename Target>
    constexpr void lock_for() noexcept(...)
        requires concepts::convertible_from<Target, Refer>;

    // Возвращает значение уровня Target.
    // Требует предварительного вызова lock_for<Target>().
    template <typename Target>
    [[nodiscard]]
    constexpr Target value_as() noexcept(...)
        requires concepts::convertible_from<Target, Refer>;
};
```

## Примеры

### Доступ к вложенному значению

```cpp
#include <scl/feature/value_lock.h>
#include <scl/feature/wrapper.h>

using Inner = scl::wrapper<int, mutex_executor>;
using Outer = scl::wrapper<Inner, scl::feature::inplace::plain>;

Outer w{42};

scl::value_lock<Outer &> vl{w};    // блокировка не установлена

// Доступ к int — блокирует слой mutex_executor:
vl.lock_for<int &>();
int & v = vl.value_as<int &>();    // v == 42

// Доступ к внешней обёртке — блокировка не нужна:
vl.lock_for<Outer &>();
Outer & ref = vl.value_as<Outer &>();
```

### Доступ к промежуточным слоям

```cpp
// Доступ к Inner напрямую (plain-слой, без мьютекса):
vl.lock_for<Inner &>();
Inner & inner = vl.value_as<Inner &>();
```

## См. также

- [wrapper_lock](wrapper_lock.md)
- [wrapper_guard](wrapper_guard.md)
- [wrapper_cast](../casts/wrapper_cast.md)
- [concepts::convertible_from](../concepts/concepts.md)
