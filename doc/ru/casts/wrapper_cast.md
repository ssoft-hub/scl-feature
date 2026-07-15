# wrapper_cast

Разворачивает обёртку до любого достижимого типа в её цепочке; для не-wrapper
типов — прозрачный проброс.

- Заголовок: `#include <scl/feature/wrapper_cast.h>`
- Объявление: `template <typename Type> constexpr decltype(auto) wrapper_cast(Type && w) noexcept;`
- Пространство имён: `scl`

---

## Описание

`scl::wrapper_cast(x)` возвращает прокси `wrapper_caster<Refer>` с неявными
операторами преобразования ко всем типам, достижимым при обходе цепочки обёрток.
Для аргументов, не являющихся обёртками, функция прозрачно пробрасывает
пересылающую ссылку без каких-либо накладных расходов.

Операторы преобразования имеют квалификатор `&&` — прокси должен быть rvalue
в момент преобразования (непосредственный временный объект из `wrapper_cast()`
или явный `std::move(proxy)`). Кроме того, доступно именованное преобразование
через `.to<T>()`.

> **Время жизни.** Прокси хранит ссылку на аргумент и не продлевает его время
> жизни. Используйте результат в пределах того же полного выражения; не сохраняйте
> `wrapper_caster` в переменную, особенно из временного объекта —
> `auto p = wrapper_cast(make_wrapper());` оставит `p` ссылающимся на разрушенную
> обёртку по завершении инициализирующего выражения.

Основное применение — шаблонный код, единообразно принимающий как обычные
значения, так и обёртки. Именно это делает `SCL_REFLECT_METHOD`: перед вызовом
метода он пропускает каждый аргумент через `wrapper_cast`, чтобы `wrapper<T>`
принимался там, где ожидается `T`.

## wrapper_caster

`wrapper_caster<Refer>` — `[[nodiscard]]`-прокси, некопируемый и неперемещаемый.

```cpp
template <typename Refer>
class [[nodiscard]] wrapper_caster
{
public:
    wrapper_caster(wrapper_caster &&) = delete;
    wrapper_caster(wrapper_caster const &) = delete;

    // Неявное преобразование к любому достижимому из Refer типу.
    // Операторы квалифицированы как &&.
    [[nodiscard]] operator TargetType() &&;

    // Явное именованное преобразование.
    template <typename T>
    [[nodiscard]] T to() &&
        requires concepts::convertible_from<T, Refer>;
};
```

## Примеры

### Передача обёртки в функцию, ожидающую тип значения

```cpp
#include <scl/feature/wrapper_cast.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/executor/inplace/plain.h>

void increment(int & v) { ++v; }

scl::wrapper<int, scl::feature::inplace::plain> w{41};

// Неявное преобразование: wrapper<int> → int&
increment(scl::wrapper_cast(w));  // w содержит 42
```

### Явное преобразование через .to<T>()

```cpp
int & ref = scl::wrapper_cast(w).to<int &>();
```

### Сквозная передача для не-wrapper типов

```cpp
int x = 7;
int & r = scl::wrapper_cast(x);  // r == x; нет аллокаций, нет накладных расходов
```

### В шаблонном коде

```cpp
template <typename Arg>
void process(Arg && arg)
{
    // Работает единообразно: wrapper → value, не-wrapper → as-is.
    use(scl::wrapper_cast(std::forward<Arg>(arg)));
}
```

## См. также

- [value_lock](../locking/value_lock.md)
- [concepts::convertible_from](../concepts/concepts.md)
- [SCL_REFLECT_METHOD](../reflection/method.md)
