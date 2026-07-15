# SCL_REFLECT_PROPERTY

Генерирует член-данные типа `wrapper`, который проксирует одно поле обёрнутого
объекта через исполнитель, обеспечивая доступ как на чтение, так и на запись.

- Заголовок: `#include <scl/feature/reflection/property.h>`

---

## Описание

```cpp
#define SCL_REFLECT_PROPERTY(prop)
```

Генерирует один член-данные внутри включающего класса: `scl::wrapper` над типом поля
свойства, привязанный к полю обёрнутого объекта и работающий через исполнитель,
установленный `SCL_REFLECT_TYPE`. Дополнительно создаётся внутренняя тег-структура
для обнаружения переопределения исполнителем. Точное раскрытие опирается на псевдонимы,
являющиеся деталями реализации, — не обращайтесь к ним напрямую.

Макрос может применяться в любом классе, удовлетворяющем приведённым выше требованиям,
— не только в классах, которые сами являются экземплярами `scl::wrapper`.

`SCL_REFLECT_TYPE` должен быть объявлен в том же теле класса перед этим макросом.

Отражаемое поле `prop` **обязано существовать как член-данные** обёрнутого типа —
даже если исполнитель предоставляет переопределение.

> **Член-свойство — это представление.** Его внутренний исполнитель восстанавливает
> родительскую обёртку через байтовое смещение, поэтому memberwise-копирование/перемещение
> **всей** обёртки безопасно, но сам член-свойство нельзя копировать, перемещать или
> возвращать отдельно — это перебазирует смещение и портит обратный указатель
> (неопределённое поведение). Используйте его по месту (`wrapper.prop`,
> `wrapper_cast(wrapper.prop)`).

## Доступ

| Выражение | Поведение |
|-----------|-----------|
| `int v = wrapper.prop` | неявное преобразование через `operator T() &` / `operator T const&() const&` |
| `wrapper.prop = value` | присваивание через `operator=(T)` |
| `wrapper.prop = other_wrapper.prop` | присваивание между обёртками через `wrapper_cast` |

## Переопределение в исполнителе

Если исполнитель предоставляет статический член:

```cpp
static R property_prop(Executor cv_ref exec);
```

первый параметр которого **точно** совпадает с заданным cv-ref-квалификатором
(обнаружение выполняется тем же приёмом с приведением к указателю на функцию,
что и в `has_execute_v`), то доступ для этого квалификатора вызывает
переопределение напрямую, минуя `execute()`.

| Вид перегрузки | Условие | Диспетчеризация |
|----------------|---------|-----------------|
| *executor-override* | `Executor::property_prop(exec)` существует | вызывает этот статический член напрямую |
| *execute-path* | переопределения для данного cv-ref нет | вызывает `Executor::execute(exec, lambda, value)` |

Для разных квалификаторов эти два пути не исключают друг друга: переопределение
для mutable-lvalue может сосуществовать с путём через `execute` для const-lvalue
у одного и того же свойства.

## Распространение `noexcept`

Когда переопределение в исполнителе активно, `noexcept` наследуется от него.
Путь через `execute` гарантий `noexcept` не даёт.

## Безопасность копирования и перемещения

Внутренний исполнитель хранит байтовое смещение от самого себя до внешнего
исполнителя. Поскольку это расстояние одинаково для всех экземпляров, почленное
копирование и перемещение работают корректно без пользовательских конструкторов.

## Ограничения

Смещение вычисляется через `reinterpret_cast` (идиома `container_of`), а это не
константное выражение. Поэтому отражённый property-член нельзя использовать в
константном выражении, а обёртку, содержащую члены `SCL_REFLECT_PROPERTY`,
нельзя сконструировать в `constexpr`-контексте.

Член-исполнитель должен быть объявлен **перед** любым `SCL_REFLECT_PROPERTY`,
чтобы он конструировался первым (инициализация членов идёт в порядке объявления).

## Примеры

### Без переопределения в исполнителе

```cpp
#include <scl/feature/reflection/property.h>

struct Point { int x = 0; int y = 0; };

struct PointWrapper;
template <>
struct scl::feature::executor_trait<PointWrapper> {
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_exec); }
};

struct PointWrapper {
    scl::feature::inplace::plain<Point> m_exec;
    SCL_REFLECT_TYPE(PointWrapper, scl::feature::inplace::plain<Point>)
    SCL_REFLECT_PROPERTY(x)
    SCL_REFLECT_PROPERTY(y)
};

PointWrapper p{Point{3, 7}};
int a = p.x;  // 3 — путь через execute
p.x = 42;     // запись через путь execute
```

### С переопределением в исполнителе

```cpp
struct SpecialExecutor {
    Point m_value;
    int   m_shadow_x = 0;

    // Доступ к mutable-lvalue перенаправляется на m_shadow_x:
    static int & property_x(SpecialExecutor & self) noexcept
    { return self.m_shadow_x; }

    template <typename Self, typename Func, typename... Args>
    static decltype(auto) execute(Self &&, Func && f, Args &&... args)
    { return f(::std::forward<Args>(args)...); }

    template <typename Self>
    static decltype(auto) access(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_value); }
};
```

Для этого исполнителя доступ к `wrapper.x` на mutable-lvalue вызывает
`property_x` напрямую (`noexcept`, читает `m_shadow_x`), а доступ на const
переходит к `execute()` (читает `m_value.x`).

## См. также

- [SCL_REFLECT_TYPE](type.md)
- [SCL_REFLECT_METHOD](method.md)
- [reflect](reflect.md)
- [executor_trait](../type_traits/executor.md)
