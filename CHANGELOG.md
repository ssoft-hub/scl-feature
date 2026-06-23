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
- Assignment operators for `inplace::plain` — eight cv-ref overloads via
  `SCL_EXECUTOR_ASSIGNMENT_FOR_SELF`; constrained on `assignable_from<value_type>`, `noexcept`-propagating,
  value forwarded with `scl::forward_like`
- Assignment operators for `inplace::uninitialized` — eight cv-ref overloads via
  `SCL_EXECUTOR_ASSIGNMENT_FOR_SELF`; copies raw storage bytes via `std::ranges::copy`,
  constrained (together with the copy/move constructor) to `std::is_trivially_copyable_v`
  value types — byte-copying a non-trivially-copyable object is undefined behaviour
- Assignment operators for `scl::wrapper` — eight self-type cv-ref overloads
  (`SCL_WRAPPER_ASSIGNMENT_FOR_SELF`) plus a forwarding-reference overload for compatible
  wrapper types (`SCL_WRAPPER_ASSIGNMENT_FOR_OTHER`)
- `scl::wrapper_lock<Refer>` — lazy RAII lock for a single wrapper layer; `lock()`/`unlock()`
  call `guard()`/`unguard()` on the executor; non-copyable and non-movable
- `scl::value_lock<Refer>` — recursive lazy lock through the entire wrapper chain; captures
  references to every executor at construction (no guard acquired yet), activates guards
  layer-by-layer via `lock_for<Target>()`, reads the value via `value_as<Target>()`
- `scl::wrapper_cast(w)` — returns a `wrapper_caster<Refer>` proxy that implicitly converts
  to any type reachable in the wrapper chain via `.to<T>()` or implicit conversion operators;
  pass-through for non-wrapper types
- `is_executor_v<T>` — checks whether `T` satisfies the executor interface (`value()` and
  `execute()` callable for all three primary value categories); cv-ref qualifiers are stripped
- `concepts::executor<T>` — concept wrapping `is_executor_v`
- `is_convertible_from_v<Target, Refer>` — checks whether `Target` is reachable from `Refer`
  directly or by traversing any number of wrapper layers
- `concepts::convertible_from<Target, Refer>` — concept wrapping `is_convertible_from_v`
- `concepts::compatible_with<Expected, T>`, `concepts::compatible_with_part_of<Expected, T>`,
  `concepts::part_compatible_with<Expected, T>` — three concepts for wrapper-chain compatibility
- Executor method detection traits: `has_value_v`, `has_execute_v`, `has_guard_v`,
  `has_unguard_v`, `is_guard_noexcept_v`, `is_unguard_noexcept_v`
- `scl::feature::reflect<Wrapper, Executor, Type>` — CRTP mixin base for the reflection chain;
  `detail::wrapper` inherits from it automatically; user-provided partial specialisations for
  concrete value types inject proxy members into every wrapper holding that type

### Changed

- `SCL_REFLECT_BINARY_OPERATOR` — now emits the 24 member overloads (wrapper-left) plus 8
  reverse-operand hidden-friend overloads (wrapper-right, `x op w`), making the reflected
  operator symmetric; the reverse friend is constrained out when the left operand is itself a
  wrapper (`is_wrapper_v`), so `w1 op w2` stays unambiguous. The reverse direction has no
  executor-override path but still routes through `Executor::execute`
- `SCL_REFLECT_PREFIX_UNARY_OPERATOR`, `SCL_REFLECT_POSTFIX_UNARY_OPERATOR` — emit member
  overloads only (a unary operator has no reverse-operand case); use `SCL_REFLECT_FRIEND_*`
  for an ADL-only unary operator
- `reflect_operators` — `operator=`, `operator->`, and all compound-assignment operators
  switched to `SCL_REFLECT_MEMBER_BINARY_OPERATOR` / `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR`
  to comply with the C++ requirement that these operators be non-static member functions
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

- `is_compatible_with_v`, `is_compatible_with_part_of_v`, `is_part_compatible_with_v`:
  cv-ref qualifiers were not stripped consistently from both sides before comparison
- `inplace::uninitialized::value()` `const` overload: incorrect `const`-qualified
  `reinterpret_cast` prevented compilation in const contexts

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
