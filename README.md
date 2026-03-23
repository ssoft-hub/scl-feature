# ScL Feature

[![GitLab Pipeline](https://gitlab.com/ssoft-scl/scl-feature/badges/main/pipeline.svg)](https://gitlab.com/ssoft-scl/scl-feature/-/pipelines)
[![GitLab Pages](https://img.shields.io/badge/pages-gitlab-orange)](https://ssoft-scl.gitlab.io/scl-feature/)
[![GitHub Pages](https://img.shields.io/badge/pages-github-blue)](https://ssoft-hub.github.io/scl-feature/)

C++20 header-only library. Part of the [ScL Toolkit](../scl-kit).

Provides `scl::wrapper<Value, Executors...>` — a composable proxy wrapper that delegates
method calls to the held value through a chain of executor templates. Each executor can
transparently add cross-cutting behaviour such as copy-on-write semantics, thread safety,
or deferred invocation without modifying the wrapped type.

```cpp
scl::wrapper<int>                              w1{42};  // inplace::plain (default)
scl::wrapper<int, feature::inplace::plain>     w2{42};  // explicit plain
scl::wrapper<int, feature::inplace::uninitialized> w3{};  // deferred construction
```

## Requirements

- C++20 compiler (MSVC 19.30+, GCC 13+, Clang 16+)
- CMake 3.20+

## Installation

```cmake
add_subdirectory(module/feature)
target_link_libraries(your_target PRIVATE scl::feature)
```

## Documentation

- [GitLab Pages](https://ssoft-scl.gitlab.io/scl-feature/)
- [GitHub Pages](https://ssoft-hub.github.io/scl-feature/)

## See also

- [CHANGELOG](CHANGELOG.md)
- [CONTRIBUTING](CONTRIBUTING.md)
