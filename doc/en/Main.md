# ScL Feature

C++20 header-only library providing a composable proxy wrapper that delegates
method calls to the held value through a chain of executor templates.

ScL Feature is a module of the [ScL Toolkit](https://github.com/ssoft-hub/scl-kit).
Licensed under [The Unlicense](../../LICENSE.md).

## Requirements

- C++20 compatible compiler (MSVC 19.30+, GCC 13+, Clang 16+)
- CMake 3.20+
- [scl::utility](https://gitlab.com/ssoft-scl/scl-utility) (fetched automatically via CMake)

## Installation

Add as a subdirectory in CMake and link against the interface target:

```cmake
add_subdirectory(module/feature)
target_link_libraries(your_target PRIVATE scl::feature)
```

Then include the umbrella header or individual component headers:

```cpp
#include <scl/feature.h>              // everything
#include <scl/feature/wrapper.h>      // wrapper only
#include <scl/feature/reflection.h>   // reflection only
```

## Components

### Wrapper

| Component | Header | Description |
|-----------|--------|-------------|
| [wrapper](wrapper/wrapper.md) | `<scl/feature/wrapper.h>` | Composable proxy that delegates calls through an executor chain |

### Executors

| Component | Header | Description |
|-----------|--------|-------------|
| [inplace::plain](executors/plain.md) | `<scl/feature/inplace/plain.h>` | In-place storage with zero overhead |
| [inplace::uninitialized](executors/uninitialized.md) | `<scl/feature/inplace/uninitialized.h>` | In-place raw storage with deferred construction |

### Locking utilities

| Component | Header | Description |
|-----------|--------|-------------|
| [wrapper_guard](locking/wrapper_guard.md) | `<scl/feature/wrapper_guard.h>` | RAII guard: calls `guard()`/`unguard()` at construction/destruction |
| [wrapper_lock](locking/wrapper_lock.md) | `<scl/feature/wrapper_lock.h>` | Lazy RAII lock for a single wrapper layer |
| [value_lock](locking/value_lock.md) | `<scl/feature/value_lock.h>` | Recursive lazy lock through the entire wrapper chain |

### Casts

| Component | Header | Description |
|-----------|--------|-------------|
| [wrapper_cast](casts/wrapper_cast.md) | `<scl/feature/wrapper_cast.h>` | Conversion proxy — unwraps a wrapper to any reachable type |

### Type traits

| Component | Header | Description |
|-----------|--------|-------------|
| [Wrapper traits](type_traits/wrapper.md) | `<scl/feature/type_traits/wrapper.h>` | `is_wrapper_v`, `is_compatible_with_v`, `is_convertible_from_v`, … |
| [Executor traits](type_traits/executor.md) | `<scl/feature/type_traits/executor.h>` | `is_executor_v`, `has_value_v`, `has_guard_v`, … |

### Concepts

| Component | Header | Description |
|-----------|--------|-------------|
| [Concepts](concepts/concepts.md) | `<scl/feature/concepts.h>` | `executor`, `wrapper`, `convertible_from`, `compatible_with`, … |

### Reflection

| Component | Header | Description |
|-----------|--------|-------------|
| [reflect](reflection/reflect.md) | `<scl/feature/reflection/reflect.h>` | CRTP mixin base for the reflection chain |
| [SCL_REFLECT_TYPE](reflection/type.md) | `<scl/feature/reflection/type.h>` | Declares wrapper and executor type aliases for reflection macros |
| [SCL_REFLECT_METHOD](reflection/method.md) | `<scl/feature/reflection/method.h>` | Generates proxy method overloads for all cv-ref qualifications |

## Quick start

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Document {
    std::string title() const &;
    std::string title() &&;
    void set_title(std::string t);
};

// Inject proxy members into every wrapper<Document, ...>.
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
std::string t = std::move(doc).title();  // calls Document::title() &&
```

## See also

- [Russian documentation](../ru/Main.md)
- [CHANGELOG](../../CHANGELOG.md)
- [CONTRIBUTING](../../CONTRIBUTING.md)
