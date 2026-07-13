# Changelog

All notable changes to the ScL Feature module will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

- `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)`, `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR`,
  `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR` — member-only operator reflection; required for
  operators C++ mandates as non-static member functions (`=`, `->`, compound assigns)
- `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)`, `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR`,
  `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR` — hidden-friend (ADL-only) operator reflection
  with no member counterpart; the wrapper is the explicit first parameter
- `SCL_REFLECT_SUBSCRIPT_OPERATOR(op, name)` — reflects `operator[]`, including the built-in
  subscript of a pointer value
- `SCL_REFLECT_EQUALITY_OPERATOR(op, name)` — reflects `==` / `!=` returning `bool`, as required
  for a C++20 rewritten equality candidate
- Assignment for `scl::wrapper` — `wrapper = x` assigns the wrapped value for any
  right-hand operand: a plain value, another wrapper (read through its own executor,
  so value types may differ), or the same wrapper type. Assignment reflects through
  the executor with the same dispatch as every other reflected operator: an executor
  `operator_assign(self, rhs)` override is used when present (receiving the raw
  operand, the same convention as `operator_<name>` overrides), otherwise the value
  is assigned through `execute()`. Executors themselves carry no assignment operator
- `scl::wrapper_lock<Refer>` — lazy RAII lock for a single wrapper layer; `lock()`/`unlock()`
  call `guard()`/`unguard()` on the executor; non-copyable and non-movable
- `scl::value_lock<Refer>` — recursive lazy lock through the entire wrapper chain; captures
  references to every executor at construction (no guard acquired yet), activates guards
  layer-by-layer via `lock_for<Target>()`, reads the value via `value_as<Target>()`
- `scl::wrapper_cast(w)` — returns a `wrapper_caster<Refer>` proxy that implicitly converts
  to any type reachable in the wrapper chain via `.to<T>()` or implicit conversion operators;
  pass-through for non-wrapper types
- `is_executor_v<T>` — checks whether `T` satisfies the executor interface (`access()` and
  `execute()` callable for all three primary value categories, and no assignment operator of
  its own — an executor is neither copy- nor move-assignable); cv-ref qualifiers are stripped
- `concepts::executor<T>` — concept wrapping `is_executor_v`
- `is_convertible_from_v<Target, Refer>` — checks whether `Target` is reachable from `Refer`
  directly or by traversing any number of wrapper layers
- `concepts::convertible_from<Target, Refer>` — concept wrapping `is_convertible_from_v`
- `concepts::compatible_with<Expected, T>`, `concepts::compatible_with_part_of<Expected, T>`,
  `concepts::part_compatible_with<Expected, T>` — three concepts for wrapper-chain compatibility
- Executor method detection traits: `has_access_v`, `has_execute_v`, `has_guard_v`,
  `has_unguard_v`, `is_guard_noexcept_v`, `is_unguard_noexcept_v`
- `scl::feature::reflect<Wrapper, Executor, Type>` — CRTP mixin base for the reflection chain;
  `detail::wrapper` inherits from it automatically; user-provided partial specialisations for
  concrete value types inject proxy members into every wrapper holding that type
- `SCL_REFLECT_PROPERTY(prop)` — generates a `wrapper` data member that proxies a single field
  of the wrapped object through the executor for read and write, preserving cv-ref qualifiers;
  supports an executor override via a static `property_<name>(exec cv_ref)` member (per cv-ref
  qualification, analogous to `SCL_REFLECT_METHOD`). Copy/move-safe; not usable in constant
  expressions

### Changed

- Executor headers relocated under `executor/`: `inplace/plain.h` and `inplace/uninitialized.h`
  now live at `executor/inplace/` and are included as `<scl/feature/executor/inplace/...>`; the
  `scl::feature::inplace` namespace is unchanged
- Executor interface: the value accessor is renamed `value()` → `access()`, and the detection
  trait `has_value_v` → `has_access_v`. Frees the name `value` for value-typed concepts and
  removes the clash with `value_type` and the `value()` accessors on the lock handles. Breaking
  change for any custom executor or `executor_trait` specialisation
- Reflected operators now reflect the wrapped value's free and built-in operators, not only its
  member operators. `wrapper<int>` supports the arithmetic, comparison, bitwise, shift and
  logical operators, `-w`/`~w`/`!w`/`++w`, and compound assignment; `wrapper<int*>` supports
  `w[i]`. Every direction is reflected — `w op x`, `x op w`, and `w1 op w2` — and a member
  operator of the value takes precedence over a free one. `==`/`!=` return `bool`, so the C++20
  reversed comparison `x == w` is formed from the member. Applies to `SCL_REFLECT_BINARY_OPERATOR`,
  `SCL_REFLECT_MEMBER_BINARY_OPERATOR`, the unary macros, and the operators `reflect_operators`
  gives `scl::wrapper`
- `reflect_operators` — `operator->` and address-of/indirection stay member-only (a free
  fallback for `&` would hijack the pointer-to-wrapper syntax)
- `scl::feature::wrapper_guard` alias moved to `scl::wrapper_guard`
- `scl::wrapper`: Executor template parameter is now constrained with `concepts::executor`
- `scl::wrapper`: executor member marked `[[no_unique_address]]`
- `SCL_REFLECT_TYPE(Type, ExecutorType)` — second parameter changed from a member expression
  to an executor type name (e.g. `executor_type`)
- `SCL_REFLECT_METHOD(method)` extended: now generates 24 overloads (3 per cv-ref ×
  8 qualifications) — added executor-override dispatch and explicit-template-args overload;
  all calls route through `Executor::execute`; wrapper arguments are unwrapped via
  `scl::wrapper_cast`

### CI

- `script/ci/deploy_versioned_docs.sh` — builds versioned Doxygen HTML for
  all version tags (immutable) and `dev` (latest); injects version-selector
  nav bar; manages `versions.json` and root `index.html` on the pages branch.
- `.gitlab-ci.yml` pages job: switched to `archlinux:latest` for up-to-date
  Doxygen; triggers on version tags; deploys via `deploy_versioned_docs.sh`.
- `.github/workflows/docs.yml`: runs in `archlinux:latest` container; installs
  git before checkout; deploys to `gh-pages` via `deploy_versioned_docs.sh`.
- `script/ci/mirror_github.sh`: exclude `pages` branch from GitHub mirror;
  preserve `gh-pages` from `--prune`; add `--prune-tags`.
- `AGENTS.md` — development guidelines for contributors and agents:
  code conventions, branching rules, required pre-commit checks
  (clang-format, clang-tidy, cppcheck), commit message format,
  Doxygen documentation requirements, and compatibility constraints.
- `.cppcheck` — suppress `internalError`: cppcheck 2.13 on the GitHub Actions
  runners fails to analyse the macro-heavy reflection headers (`internalError`
  on `reflection/reflect.h`), unblocking the mirror's Lint workflow; cppcheck
  2.19 on the GitLab runners is unaffected.

### Fixed

- Reflection test suite no longer aborts compilation under MSVC: the `can_call_set_v`
  call-site checks tripped a fatal internal compiler error (C1001) on the non-Clang MSVC
  frontend. The checks now route through `is_invocable` on a SFINAE-friendly lambda,
  restoring the full test build on MSVC alongside GCC and Clang
- Reflected method, operator, and property dispatch resolved the wrapped value too early —
  as an argument to `Executor::execute()`, evaluated before `execute()` ran. The executor is
  now passed to the dispatch callable, which resolves the value from inside `execute()`, so
  any work `execute()` performs happens before the value is read
- `is_compatible_with_v`, `is_compatible_with_part_of_v`, `is_part_compatible_with_v`:
  cv-ref qualifiers were not stripped consistently from both sides before comparison
- `inplace::uninitialized::access()` `const` overload: incorrect `const`-qualified
  `reinterpret_cast` prevented compilation in const contexts
- `<scl/feature/type_traits.h>` now re-exports `SCL_HAS_QUALIFIED_METHOD` — the umbrella
  header omitted the `type_traits/has_qualified_method.h` include, so the macro was reachable
  only transitively through a reflection header

## [0.0.1] - 2026-03-25

### Added

- `scl::wrapper<Value, Executors...>` — composable proxy wrapper that delegates method calls
  through a chain of executor templates (default executor: `inplace::plain`)
- `inplace::plain` — in-place executor with zero overhead
- `inplace::uninitialized` — in-place executor with deferred (lazy) construction
- `scl::wrapper_guard` — RAII guard providing plain (non-executor) access to the held value
- `executor_access` — unified access point to `wrapper::m_executor`
- `wrapper_constructor_resolver` — 4-strategy constructor resolution for self/other wrapper types
- Self/other constructor macros: `SCL_WRAPPER_CONSTRUCTOR_FOR_SELF`, `SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER`
- Type traits: `is_wrapper_v`, `is_compatible_with`, `is_part_compatible_with`,
  `is_compatible_with_part_of`, `executor_trait`
- `SCL_HAS_QUALIFIED_METHOD` — compile-time predicate for method existence checks
- `concepts::wrapper` — concept for compile-time wrapper validation
- Reflection: `SCL_REFLECT_TYPE` macro for wrapper identity, `SCL_REFLECT_METHOD` macro
  for method introspection
