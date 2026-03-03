# ScL.Feature

[![GitLab Pipeline](https://gitlab.com/ssoft-scl/scl-feature/badges/main/pipeline.svg)](https://gitlab.com/ssoft-scl/scl-feature/-/pipelines)
[![GitLab Pages](https://img.shields.io/badge/pages-gitlab-orange)](https://ssoft-scl.gitlab.io/scl-feature/)
[![GitHub Pages](https://img.shields.io/badge/pages-github-blue)](https://ssoft-hub.github.io/scl-feature/)

C++20 header-only library. Part of the [ScL Toolkit](../scl-kit).

Provides `scl::feature::wrapper<T, Tools...>` — a composable adapter that adds orthogonal
features to any user-defined type without modifying it. Multiple tools can be combined
in superposition in any order.

## Requirements

- C++20 compiler (MSVC 19.30+, GCC 13+, Clang 16+)
- CMake 3.20+

## Installation

```cmake
add_subdirectory(module/feature)
target_link_libraries(your_target PRIVATE scl::feature)
```

## Doxygen

- [GitLab Pages](https://ssoft-scl.gitlab.io/scl-feature/)
- [GitHub Pages](https://ssoft-hub.github.io/scl-feature/)

## See also

- [CHANGELOG](CHANGELOG.md)
- [CONTRIBUTING](CONTRIBUTING.md)
