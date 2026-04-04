# ScL.Feature

[![GitLab Pipeline](https://gitlab.com/ssoft-scl/scl-feature/badges/main/pipeline.svg)](https://gitlab.com/ssoft-scl/scl-feature/-/pipelines)
[![GitLab Pages](https://img.shields.io/badge/pages-gitlab-orange)](https://ssoft-scl.gitlab.io/scl-feature/)
[![GitHub Pages](https://img.shields.io/badge/pages-github-blue)](https://ssoft-hub.github.io/scl-feature/)

C++20 header-only library. Part of the [ScL Toolkit](../scl-kit).

ScL.Feature provides `scl::wrapper<Value, Executors...>` — a composable proxy wrapper that
delegates method calls to the held value through a chain of executor templates. Each executor
can transparently add cross-cutting behaviour such as copy-on-write semantics, thread safety,
or deferred invocation without modifying the wrapped type. Licensed under [The Unlicense](LICENSE.md).

## Features

- **Wrapper** — `scl::wrapper<Value, Executors...>`, a composable proxy:
  - Delegates all method calls to the held `Value` through a left-folded executor chain
  - Adjacent duplicate executors in the list are collapsed automatically
  - Defaults to `feature::inplace::plain` when no executor is specified
- **Executors** (`scl::feature::inplace`):
  - `inplace::plain` — stores `Value` in-place with zero overhead; serves as the default executor
  - `inplace::uninitialized` — holds `Value` in correctly sized and aligned raw storage,
    enabling deferred (lazy) construction
- **Wrapper guard** — `scl::feature::wrapper_guard<Refer>`:
  - RAII guard that calls `guard()` / `unguard()` on the executor at construction / destruction
  - Works uniformly for wrapper references and plain value references
  - Exposes `value()` preserving the cv- and ref-qualifiers of the incoming reference
- **Type traits** (`scl::feature`):
  - `is_wrapper_v<T>` — checks whether `T` is a `wrapper` specialization (strips cv-ref qualifiers)
  - `is_executor_v<T>` — checks whether `T` satisfies the executor interface (strips cv-ref qualifiers)
  - `is_convertible_from_v<Target, Refer>` — checks whether a reference `Refer` can be converted
    to `Target` by unwrapping through the wrapper chain
  - `has_value_v<E, Self>`, `has_execute_v<E, Self>` — detects `value()` / `execute()` on executor `E`
  - `has_guard_v<E, Self>`, `has_unguard_v<E, Self>` — detects `guard()` / `unguard()` on executor `E`
  - `is_guard_noexcept_v<E, Self>`, `is_unguard_noexcept_v<E, Self>` — checks `noexcept` on
    `guard()` / `unguard()` (true when the method is absent or marked `noexcept`)
  - `is_compatible_with_v<Expected, Test>` — `Test` is the same as or derived from `Expected`;
    for wrapper specialisations with the same executor the check is applied recursively to value types
  - `is_compatible_with_part_of_v<Expected, Test>` — `Expected` (a wrapper) recursively contains
    a value compatible with `Test`
  - `is_part_compatible_with_v<Expected, Test>` — `Test` (a wrapper) recursively contains
    a value compatible with `Expected`
- **Concepts** (`scl::feature::concepts`):
  - `concepts::executor<T>` — satisfied when `T` satisfies the executor interface, strips cv-ref qualifiers (`is_executor_v`)
  - `concepts::wrapper<T>` — satisfied when `T` is a `wrapper` specialization
  - `concepts::convertible_from<Target, Refer>` — satisfied when `Refer` can be unwrapped to `Target`
  - `concepts::compatible_with<Expected, T>`
  - `concepts::compatible_with_part_of<Expected, T>`
  - `concepts::part_compatible_with<Expected, T>`
- **Reflection macros** (`scl/feature/reflection/`):
  - `SCL_REFLECT_TYPE(Type, Member)` — declares the wrapper identity alias required by method reflection
  - `SCL_REFLECT_METHOD(method)` — generates 16 proxy overloads (8 cv-ref qualifications × 2:
    deduced and explicit template arguments); each overload is constrained so only overloads
    that actually exist on the wrapped type are exposed

## Requirements

- C++20 compiler (MSVC 19.30+, GCC 13+, Clang 16+)
- CMake 3.20+
- [scl::utility](../utility) (fetched automatically via CMake)

## Installation

Add the module as a subdirectory and link against the interface target:

```cmake
add_subdirectory(module/feature)
target_link_libraries(your_target PRIVATE scl::feature)
```

## Quick example

### Reflection

```cpp
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Target {
    short get() &;
    int   get() const &;
    float get() &&;
};

struct MyWrapper {
    Executor<Target> m_executor; // any executor
    SCL_REFLECT_TYPE(MyWrapper, m_executor);

    SCL_REFLECT_METHOD(get)  // generates 16 overloads, 3 survive constraints
};

MyWrapper w{42};
w.get();               // Target::get() &       → short
std::as_const(w).get() // Target::get() const & → int
std::move(w).get();    // Target::get() &&      → float
```

## Doxygen

- [GitLab Pages](https://ssoft-scl.gitlab.io/scl-feature/)
- [GitHub Pages](https://ssoft-hub.github.io/scl-feature/)

## See also

- [CHANGELOG](CHANGELOG.md)
- [CONTRIBUTING](CONTRIBUTING.md)
