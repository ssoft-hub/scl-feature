# ScL Feature

Модуль C++20, header-only. Предоставляет компонуемую обёртку, делегирующую
вызовы методов к хранимому значению через цепочку шаблонов-исполнителей.

ScL Feature — модуль инструментария [ScL Toolkit](https://github.com/ssoft-hub/scl-kit).
Лицензия: [The Unlicense](../../LICENSE.md).

## Требования

- Компилятор с поддержкой C++20 (MSVC 19.30+, GCC 13+, Clang 16+)
- CMake 3.20+
- [scl::utility](https://gitlab.com/ssoft-scl/scl-utility) (подключается автоматически через CMake)

## Установка

Добавьте модуль как подкаталог в CMake и свяжите с интерфейсной целью:

```cmake
add_subdirectory(module/feature)
target_link_libraries(your_target PRIVATE scl::feature)
```

Затем подключите общий заголовок или заголовки отдельных компонентов:

```cpp
#include <scl/feature.h>              // всё
#include <scl/feature/wrapper.h>      // только wrapper
#include <scl/feature/reflection.h>   // только reflection
```

## Компоненты

### Wrapper — обёртчик

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [wrapper](wrapper/wrapper.md) | `<scl/feature/wrapper.h>` | Обёртка, делегирующая вызовы методов через цепочку исполнителей |

### Executors — исполнители

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [inplace::plain](executors/plain.md) | `<scl/feature/executor/inplace/plain.h>` | Хранение значения напрямую, без накладных расходов |
| [inplace::uninitialized](executors/uninitialized.md) | `<scl/feature/executor/inplace/uninitialized.h>` | Сырое выровненное хранилище с отложенной конструкцией |

### Locking utilities — утилиты блокировки

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [wrapper_guard](locking/wrapper_guard.md) | `<scl/feature/wrapper_guard.h>` | RAII-охранник: вызывает `guard()`/`unguard()` при создании/уничтожении |
| [wrapper_lock](locking/wrapper_lock.md) | `<scl/feature/wrapper_lock.h>` | Ленивая RAII-блокировка одного слоя обёртки |
| [value_lock](locking/value_lock.md) | `<scl/feature/value_lock.h>` | Рекурсивная ленивая блокировка всей цепочки обёрток |

### Casts — приведение типов

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [wrapper_cast](casts/wrapper_cast.md) | `<scl/feature/wrapper_cast.h>` | Разворачивает обёртку до любого достижимого типа в её цепочке |

### Type traits — свойства типов

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [Wrapper traits](type_traits/wrapper.md) | `<scl/feature/type_traits/wrapper.h>` | `is_wrapper_v`, `is_compatible_with_v`, `is_convertible_from_v` и др. |
| [Executor traits](type_traits/executor.md) | `<scl/feature/type_traits/executor.h>` | `is_executor_v`, `has_value_v`, `has_guard_v` и др. |

### Concepts — концепты

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [Концепты](concepts/concepts.md) | `<scl/feature/concepts.h>` | `executor`, `wrapper`, `convertible_from`, `compatible_with` и др. |

### Reflection — рефлексия

| Компонент | Заголовок | Описание |
|-----------|-----------|----------|
| [reflect](reflection/reflect.md) | `<scl/feature/reflection/reflect.h>` | CRTP-примесь — базовый класс цепочки рефлексии |
| [SCL_REFLECT_TYPE](reflection/type.md) | `<scl/feature/reflection/type.h>` | Объявляет псевдонимы wrapper и исполнителя для макросов рефлексии |
| [SCL_REFLECT_METHOD](reflection/method.md) | `<scl/feature/reflection/method.h>` | Генерирует прокси-перегрузки метода для всех cv-ref квалификаций |

## Быстрый старт

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/executor/inplace/plain.h>

struct Document {
    std::string title() const &;
    std::string title() &&;
    void set_title(std::string t);
};

// Добавляем прокси-члены во все wrapper<Document, ...>.
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Document>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(title)
    SCL_REFLECT_METHOD(set_title)
};

using DocWrapper = scl::wrapper<Document, scl::feature::inplace::plain>;

DocWrapper doc{"Hello"};
doc.set_title("World");
std::string t = std::move(doc).title();  // вызывает Document::title() &&
```

## См. также

- [English documentation](../en/Main.md)
- [CHANGELOG](../../CHANGELOG.md)
- [CONTRIBUTING](../../CONTRIBUTING.md)
