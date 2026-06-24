# ScL Feature — Agent Instructions

## Overview
C++20 header-only library, part of the [ScL Toolkit](https://gitlab.com/ssoft-scl/scl-kit).
Depends on `scl::utility`. License: Unlicense.
Provides `scl::wrapper<Value, Executors...>` — a composable proxy wrapper with executor-based cross-cutting behaviour.

## Source Layout
```
src/scl/feature/     — public headers
  wrapper.h          — core wrapper template
  wrapper_cast.h     — implicit unwrapping proxy
  wrapper_guard.h    — RAII guard
  wrapper_lock.h     — lazy RAII lock
  value_lock.h       — recursive lazy lock
  inplace/           — executors (plain, uninitialized)
  reflection/        — SCL_REFLECT_TYPE / SCL_REFLECT_METHOD macros
  concepts/          — executor/wrapper concepts
  type_traits/       — is_wrapper_v, is_executor_v, etc.
  detail/            — internal: wrapper_constructor_resolver, constructor macros
test/                — unit tests
project/cmake/       — CMakeLists.txt
project/doxygen/     — Doxyfile
```

## Key Files
- `src/scl/feature/wrapper.h` — start here for the core abstraction
- `src/scl/feature/detail/wrapper_constructor_resolver.h` — 4 construction strategies
- `src/scl/feature/reflection/method.h` — SCL_REFLECT_METHOD (generates 24 overloads)
- `src/scl/feature/inplace/plain.h` — simplest executor, zero overhead

## Code Conventions
- C++20 standard, header-only (`src/scl/feature/`)
- Only dependency: `scl::utility`
- Follow existing naming and style — do not invent new patterns
- All public symbols live in namespace `scl::` or `scl::feature::`
- Every file starts with `#pragma once`
- When adding a new header, include it in the corresponding top-level header (e.g. `src/scl/feature/wrapper.h` or `reflection.h`)
- Test files named `*_gtest.cpp`, `*_doctest.cpp`, or `*_catch2.cpp`; every new public API must have a GoogleTest (`*_gtest.cpp`) test
- No comments unless the WHY is non-obvious
- All source comments and identifiers in **English**

## Required Checks Before Every Commit
Run the lint scripts in `script/lint/`. They are the **single source of truth** — CI
(`.gitlab-ci.yml`, `.github/workflows/lint.yml`) invokes these exact scripts, so a green
local run matches CI.

```sh
# Run the whole lint stage (clang-format, clang-tidy, cppcheck)
bash script/lint/lint.sh

# ...or a single tool
bash script/lint/clang_format.sh
bash script/lint/clang_tidy.sh
bash script/lint/cppcheck.sh
```

The clang-tidy and cppcheck scripts need the `scl-utility` headers. They are auto-detected
from `../utility/src` (monorepo checkout) or `/tmp/scl-utility/src` (CI clone); override with
`SCL_UTILITY_SRC=/path/to/scl-utility/src`. Tool executables can be overridden via
`CLANG_FORMAT` / `CLANG_TIDY` / `CPPCHECK` (e.g. on Windows where they are not on `PATH`).

To auto-apply formatting: `clang-format -i <files>`.

## Branching
- Branch name format: `{user}/feat/{subject}`, `{user}/fix/{subject}`, `{user}/refactor/{subject}`
- **Never commit directly to `dev` or `main`**
- Every commit must be in a buildable state

## Compatibility
- Supported compilers: MSVC 19.30+, GCC 13+, Clang 16+
- Breaking change = removing or renaming any public API symbol; avoid unless necessary
- Use compatibility helpers from `scl::utility` (attribute macros, `forward_like`, `member_like`, etc.) — do not reimplement
- Use C++ feature test macros (`__cpp_*`, `__has_cpp_attribute`) to guard functionality dependent on std version
- Do not use compiler-specific extensions directly — use `scl::utility` abstractions

## Before PR/MR
1. Update `CHANGELOG.md` with a description of the change
2. Ensure all commits are in a buildable state
3. For releases: grep for the old version string and update **all** occurrences — `CHANGELOG.md`, `project/doxygen/Doxyfile` (`PROJECT_NUMBER`), `CMakeLists.txt`, `README.md`

## Commit Message Format
Use Conventional Commits. Language: **English only**.

```
type(scope): short description (max 72 chars)

Body describing WHY the change was made, WHAT problem it solves,
and HOW it was approached. Wrap lines at 72 characters.
```

**Required types:** `feat`, `fix`, `refactor`, `docs`, `test`, `chore`, `ci`, `perf`, `style`

- No `Co-Authored-By` or any co-authorship trailers

Example:
```
feat(reflection): add execute-path overload to SCL_REFLECT_METHOD

The macro previously only generated executor-override overloads,
making it impossible to dispatch through Executor::execute for
methods not statically known to the executor. Added a second
overload constrained on has_execute_v to cover this case.
```

## Doxygen Documentation
Every public header, class, struct, function, and type alias must have a Doxygen comment. Language: **English only**.

- Use `@brief` for one-line description
- Use `@tparam`, `@param`, `@return` where applicable
- Every entity must declare `@ingroup <group>` matching its thematic group
- Groups are defined with `@defgroup` in the top-level module header or dedicated group header

Example:
```cpp
/// @defgroup ScL_Feature_Wrapper Wrapper
/// @ingroup ScL_Feature
/// @{

/// @brief Composable proxy wrapper delegating calls through an executor chain.
/// @ingroup ScL_Feature_Wrapper
/// @tparam Value     The held value type.
/// @tparam Executors Zero or more executor templates applied left-to-right.
template <typename Value, template <typename> class... Executors>
class wrapper;

/// @}
```

## Do Not
- Add runtime dependencies beyond `scl::utility`
- Break C++20 compatibility (MSVC 19.30+, GCC 13+, Clang 16+)
- Commit without running `script/lint/lint.sh` (clang-format, clang-tidy, cppcheck)
- Add implementation (.cpp) files — library is header-only
