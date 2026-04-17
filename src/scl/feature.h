#pragma once

/**
 * @file
 * @brief Includes all ScL Feature headers.
 */

#include <scl/feature/concepts.h>
#include <scl/feature/executors.h>
#include <scl/feature/reflection.h>
#include <scl/feature/type_traits.h>
#include <scl/feature/value_lock.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/feature/wrapper_guard.h>
#include <scl/feature/wrapper_lock.h>

/**
 * @namespace scl
 * @brief Root namespace of the ScL project.
 */

/**
 * @namespace scl::feature
 * @brief Namespace of the ScL Feature module.
 */

/**
 * @namespace scl::feature::concepts
 * @brief C++20 concepts for ScL Feature types.
 */

/**
 * @defgroup scl_feature_wrapper ScL Wrapper
 * @brief Composable value wrapper with pluggable executor strategy.
 *
 * @details
 * A @c wrapper<Value, Executors...> proxies method calls to the held
 * @c Value through a chain of @e executors.  Each executor is a class
 * template that can transparently add cross-cutting properties such as
 * copy-on-write semantics, thread safety, or deferred invocation without
 * modifying the wrapped type.  Multiple executors are composed via
 * left-fold; adjacent duplicates are collapsed automatically.
 */

/**
 * @defgroup scl_feature_locking ScL Locking Utilities
 * @brief RAII guards and lazy locks for wrapper and plain value types.
 *
 * @details
 * Three complementary primitives cover the full range of locking needs for
 * @c scl::wrapper chains:
 *
 * | Type | Acquisition | Depth |
 * |------|-------------|-------|
 * | @c scl::wrapper_guard | eager (constructor) | single layer |
 * | @c scl::wrapper_lock  | lazy (@c lock())    | single layer |
 * | @c scl::value_lock    | lazy (@c lock_for()) | entire chain |
 *
 * All three are transparent for non-wrapper types: construction, locking,
 * and value access become no-ops or direct reference passes respectively.
 */

/**
 * @defgroup scl_feature_wrapper_cast ScL Wrapper Cast
 * @brief Lazy-locking cast utility for wrapper and plain value references.
 *
 * @details
 * @c scl::wrapper_cast() returns a @c wrapper_caster<Refer> proxy that
 * captures references to every executor in the wrapper chain without
 * acquiring any guard.  Guards are activated **lazily**, only at the moment
 * the proxy is converted to the desired target type.
 *
 * This allows generic code to cast uniformly through wrapper chains:
 * non-wrapper types are passed through as-is via a no-op overload.
 */

/**
 * @mainpage ScL Feature
 *
 * @section intro Overview
 *
 * ScL Feature is a header-only C++20 module providing a composable wrapper
 * type that adds orthogonal features to any user-defined type without
 * modifying it. It is part of the ScL Toolkit.
 *
 * @section start Quick start
 *
 * Include the umbrella header and link against the CMake target:
 * @code{.cmake}
 * target_link_libraries(your_target PRIVATE scl::feature)
 * @endcode
 * @code{.cpp}
 * #include <scl/feature.h>
 *
 * scl::wrapper<int> w{42};
 * @endcode
 *
 * @section features Features
 *
 * - **Wrapper** — @c scl::wrapper<Value, Tools...>, a composable
 *   adapter with orthogonal tool-based features.
 * - **Executors** (@ref scl_feature_executors) — built-in executor strategies:
 *   @c inplace::plain (direct) and @c inplace::uninitialized (raw storage).
 * - **Type traits** (@ref scl_feature_type_traits) — @c is_wrapper,
 *   @c is_executor, and related metaprogramming utilities.
 * - **Concepts** (@ref scl_feature_concepts) — C++20 concepts such as
 *   @c concepts::wrapper and @c concepts::executor.
 * - **Locking utilities** (@ref scl_feature_locking) — @c wrapper_guard,
 *   @c wrapper_lock, and @c value_lock for RAII access to wrapper chains.
 * - **Cast** (@ref scl_feature_wrapper_cast) — @c wrapper_cast() lazy-locking
 *   cast proxy for uniform access through wrapper and plain value references.
 * - **Reflection** (@ref scl_feature_reflection) — @c SCL_REFLECT_TYPE / @c SCL_REFLECT_METHOD
 *   macros and @c reflect mixin for compile-time proxy-member generation.
 */
