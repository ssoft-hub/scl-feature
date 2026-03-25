# Changelog

All notable changes to the ScL Feature module will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [0.0.1] - 2026-03-25

### Added

- `scl::wrapper<Value, Executors...>` — composable proxy wrapper that delegates method calls
  through a chain of executor templates (default executor: `inplace::plain`)
- `inplace::plain` — in-place executor with zero overhead
- `inplace::uninitialized` — in-place executor with deferred (lazy) construction
- `scl::wrapper_guard` — RAII guard providing plain (non-executor) access to the held value
- `executor_access` — unified access point to `wrapper::m_executor`
- `wrapper_constructor_resolver` — 4-strategy constructor resolution for self/other wrapper types
- Self/other constructor macros: `SCL_CONSTRUCTOR_FOR_THIS_WRAPPER`, `SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER`
- Type traits: `is_wrapper_v`, `is_compatible_with`, `is_part_compatible_with`,
  `is_compatible_with_part_of`, `executor_trait`
- `SCL_HAS_QUALIFIED_METHOD` — compile-time predicate for method existence checks
- `concepts::wrapper` — concept for compile-time wrapper validation
- Reflection: `SCL_REFLECT_TYPE` macro for wrapper identity, `SCL_REFLECT_METHOD` macro
  for method introspection
