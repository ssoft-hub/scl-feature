# ScL Feature

[![GitLab Pipeline](https://gitlab.com/ssoft-scl/scl-feature/badges/dev/pipeline.svg)](https://gitlab.com/ssoft-scl/scl-feature/-/pipelines)
[![GitLab Pages](https://img.shields.io/badge/pages-gitlab-orange)](https://ssoft-scl.gitlab.io/scl-feature/)
[![GitHub Pages](https://img.shields.io/badge/pages-github-blue)](https://ssoft-hub.github.io/scl-feature/)

C++20 header-only library. Part of the ScL Toolkit.

ScL Feature provides `scl::wrapper<Value, Executors...>` — a composable proxy wrapper that
delegates method calls to the held value through a chain of executor templates. Each executor
can transparently add cross-cutting behaviour such as copy-on-write semantics, thread safety,
or deferred invocation without modifying the wrapped type. Licensed under [The Unlicense](LICENSE.md).

## Features

- **Wrapper** — `scl::wrapper<Value, Executors...>`, a composable proxy:
  - Delegates all method calls to the held `Value` through a left-folded executor chain
  - Adjacent duplicate executors in the list are collapsed automatically
  - Defaults to `feature::inplace::plain` when no executor is specified
  - Each executor must satisfy `concepts::executor`
- **Executors** (`scl::feature::inplace`):
  - `inplace::plain` — stores `Value` in-place with zero overhead; serves as the default executor
  - `inplace::uninitialized` — holds `Value` in correctly sized and aligned raw storage,
    enabling deferred (lazy) construction
- **Locking utilities**:
  - `scl::wrapper_guard<Refer>` — RAII guard that calls `guard()` / `unguard()` on the executor
    at construction / destruction; works uniformly for wrapper and plain value references
  - `scl::wrapper_lock<Refer>` — lazy RAII lock for a single wrapper layer; guard is activated
    only when `lock()` is called explicitly and released by `unlock()` or destruction
  - `scl::value_lock<Refer>` — recursive lazy lock through the entire wrapper chain; captures
    references to every executor at construction (no guard acquired), activates guards for the
    layers needed to reach a target type via `lock_for<Target>()`
- **Casts**:
  - `scl::wrapper_cast(w)` — returns a `wrapper_caster<Refer>` proxy that implicitly converts
    to any type reachable in the wrapper chain; pass-through for non-wrapper types; used in
    generic code to forward `wrapper<T>` arguments to functions expecting `T`; conversion
    operators are `&&`-qualified so the proxy must be used as an rvalue
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
- **Reflection** (`scl/feature/reflection/`):
  - `scl::feature::reflect<Wrapper, Executor, Type>` — CRTP mixin base for the reflection
    inheritance chain; `detail::wrapper` inherits from it automatically; provide a partial
    specialisation for a concrete `Type` to inject proxy members into every wrapper holding it
  - `SCL_REFLECT_TYPE(Type, ExecutorType)` — establishes the internal type context required by
    `SCL_REFLECT_METHOD` and the other reflection macros (the generated aliases are
    implementation details — do not reference them directly); the second argument is the
    **executor type name** (e.g. `executor_type`), not a member expression
  - `SCL_REFLECT_METHOD(method)` — generates 24 proxy overloads (3 per cv-ref × 8
    qualifications): an *executor-override* overload (active when the executor provides a
    `static method_##method(...)` member), an *execute-path* overload (dispatches through
    `Executor::execute`), and an *explicit-template-args* overload; the first two are mutually
    exclusive — at most 16 are active per executor; each overload is constrained so only
    overloads that actually exist on the wrapped type are exposed
  - `SCL_REFLECT_BINARY_OPERATOR(op, name)` — generates 24 member overloads (wrapper-left,
    `w op x`) plus 8 reverse-operand hidden-friend overloads (wrapper-right, `x op w`), making
    the reflected operator symmetric; the reverse friend is constrained out when the left
    operand is itself a wrapper
  - `SCL_REFLECT_PREFIX_UNARY_OPERATOR` / `SCL_REFLECT_POSTFIX_UNARY_OPERATOR` — member-only
    (a unary operator has no reverse-operand case)
  - `SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name)` / `SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR`
    / `SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR` — member-only; required for operators C++
    mandates as non-static member functions (`=`, `->`, compound assigns)
  - `SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name)` / `SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR`
    / `SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR` — hidden-friend (ADL-only) only, no member
    counterpart; the wrapper is the explicit first parameter
- **Constructor and assignment macros** (`scl/feature/detail/wrapper_constructors.h`):
  - `SCL_WRAPPER_CONSTRUCTOR_FOR_SELF` — expands to eight constructors (all cv-ref
    qualifications of `self_type`) that forward through the executor
  - `SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER` — single forwarding-reference constructor for any
    other wrapper type; uses `wrapper_constructor_resolver` for strategy-based construction
  - `SCL_WRAPPER_ASSIGNMENT_FOR_SELF` — eight assignment operators for all cv-ref
    qualifications of `self_type`; delegates to the executor's `operator=`
  - `SCL_WRAPPER_ASSIGNMENT_FOR_OTHER` — forwarding-reference assignment for any other
    compatible wrapper type; uses `wrapper_constructor_resolver` to select the source value

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

## Quick examples

### Reflection via `scl::wrapper`

The simplest approach — specialise `scl::feature::reflect` for the value type.
`scl::wrapper` wires the chain automatically.

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Target {
    short get() &;
    int   get() const &;
    float get() &&;
};

// Inject proxy members into every wrapper<Target, ...>.
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Target>
{
    SCL_REFLECT_TYPE(Wrapper, Executor)
    SCL_REFLECT_METHOD(get)  // generates 24 overloads (3×8), 3 survive constraints
};

using MyWrapper = scl::wrapper<Target, scl::feature::inplace::plain>;

MyWrapper w{42};
w.get();                // Target::get() &       → short
std::as_const(w).get(); // Target::get() const & → int
std::move(w).get();     // Target::get() &&      → float
```

### Reflection in a custom wrapper class

For hand-written wrappers, specialise `executor_trait` and use both macros directly:

```cpp
#include <scl/feature/reflection/method.h>
#include <scl/feature/inplace/plain.h>

struct Target {
    short get() &;
    int   get() const &;
    float get() &&;
};

struct MyWrapper;
template <>
struct scl::feature::executor_trait<MyWrapper> {
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    { return ::scl::forward_like<Self>(self.m_executor); }
};

struct MyWrapper {
    using executor_type = scl::feature::inplace::plain<Target>;
    executor_type m_executor;

    SCL_REFLECT_TYPE(MyWrapper, executor_type)
    SCL_REFLECT_METHOD(get)  // generates 24 overloads (3×8), 3 survive constraints
};

MyWrapper w{42};
w.get();               // Target::get() &       → short
std::as_const(w).get() // Target::get() const & → int
std::move(w).get();    // Target::get() &&      → float
```

### Unwrapping a wrapper — `wrapper_cast`

`scl::wrapper_cast(x)` returns a proxy that implicitly converts to any type
reachable in the wrapper chain.  For non-wrapper types it is a pass-through with
no overhead.  The primary use case: pass a `wrapper<T>` to a function that expects `T`.

```cpp
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/feature/inplace/plain.h>

void increment(int & v) { ++v; }

scl::wrapper<int, scl::feature::inplace::plain> w{41};

// Pass wrapper<int> to a function expecting int&.
increment(scl::wrapper_cast(w));  // w now holds 42

// Explicit conversion via .to<T>():
int & ref = scl::wrapper_cast(w).to<int &>();

// Pass-through for non-wrapper types — no allocation, no overhead:
int x = 7;
int & r = scl::wrapper_cast(x);  // r == x

// In generic code wrapper_cast works uniformly on both:
template <typename Arg>
void process(Arg && arg)
{
    use(scl::wrapper_cast(std::forward<Arg>(arg)));  // wrapper → value; non-wrapper → as-is
}
```

## Doxygen

- [GitLab Pages](https://ssoft-scl.gitlab.io/scl-feature/dev/)
- [GitHub Pages](https://ssoft-hub.github.io/scl-feature/dev/)

## See also

- [CHANGELOG](CHANGELOG.md)
- [CONTRIBUTING](CONTRIBUTING.md)
