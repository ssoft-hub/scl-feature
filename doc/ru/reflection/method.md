# SCL_REFLECT_METHOD

Генерирует прокси-перегрузки метода для всех 8 комбинаций cv-ref-квалификаторов,
перенаправляя вызовы из wrapper к хранимому объекту через исполнитель.

- Заголовок: `#include <scl/feature/reflection/method.h>`

---

## Описание

```cpp
#define SCL_REFLECT_METHOD(method)
```

Генерирует **24 прокси-перегрузки** для `method` (3 на cv-ref × 8 квалификаций):

| Вид перегрузки | Условие | Диспетчеризация |
|----------------|---------|-----------------|
| *executor-override* | существует `Executor::method_##method(exec, args...)` | вызывает этот статический член напрямую |
| *execute-path* | нет override у исполнителя | вызывает `Executor::execute(exec, callable, args...)` |
| *явные шаблонные аргументы* | всегда генерируется | открывает `wrapper.method<T>(args...)` |

Перегрузки *executor-override* и *execute-path* взаимно исключаются —
для любого исполнителя активно не более 16 из них.

`SCL_REFLECT_TYPE` должен быть объявлен в том же теле класса до этого макроса.

## Передача аргументов

Перед вызовом каждый аргумент пропускается через `scl::wrapper_cast()`, что
позволяет передавать `wrapper<T>` туда, где ожидается `T`.

## Ограничение: совпадающие типы возвращаемых значений

`SCL_REFLECT_METHOD` различает перегрузки по типу возвращаемого значения.
Если две перегрузки возвращают одинаковый тип — макрос не может их разрешить:

```cpp
struct Good {
    short get() &;       // short ≠ int — различимы
    int   get() const &;
};

struct Bad {
    int get() &;         // int == int — макрос не может различить
    int get() const &;
};
```

## Переопределение метода в исполнителе

Если исполнитель предоставляет статический член `method_foo(exec, args...)`,
вызовы `wrapper.foo(args...)` идут напрямую к нему, минуя `execute()`.
Так исполнитель может подменить поведение для отдельных методов.

## Шаблонные методы

Если обёрнутый метод является шаблоном, явные шаблонные аргументы доступны
через прокси напрямую:

```cpp
struct Value {
    template <typename T>
    T convert() const &;
};

// После SCL_REFLECT_METHOD(convert):
scl::wrapper<Value, inplace::plain> w{...};
double d = w.convert<double>();
```

## Примеры

### Со специализацией reflect

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/executor/inplace/plain.h>

struct Point {
    short x() &;
    int   x() const &;
    float x() &&;
};

template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Point>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(x)
};

using PointWrapper = scl::wrapper<Point, scl::feature::inplace::plain>;

PointWrapper p;
short  a = p.x();                // Point::x() &
int    b = std::as_const(p).x(); // Point::x() const &
float  c = std::move(p).x();     // Point::x() &&
```

### В пользовательском классе wrapper

```cpp
struct MyWrapper;
template <>
struct scl::feature::executor_trait<MyWrapper> {
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_executor); }
};

struct MyWrapper {
    using executor_type = scl::feature::inplace::plain<Point>;
    executor_type m_executor;

    SCL_REFLECT_TYPE(MyWrapper, executor_type)
    SCL_REFLECT_METHOD(x)
};
```

## См. также

- [SCL_REFLECT_TYPE](type.md)
- [reflect](reflect.md)
- [wrapper_cast](../casts/wrapper_cast.md)
- [executor_trait](../type_traits/executor.md)
