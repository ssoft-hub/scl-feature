#pragma once

/**
 * @file
 * @brief Includes all ScL Feature headers.
 */

#include <scl/feature/concepts.h>
#include <scl/feature/type_traits.h>
#include <scl/feature/wrapper.h>
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
 *
 * All three are transparent for non-wrapper types: construction, locking,
 * and value access become no-ops or direct reference passes respectively.
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
 * - **Type traits** (@ref scl_feature_type_traits) — @c is_wrapper and
 *   related metaprogramming utilities.
 * - **Concepts** (@ref scl_feature_concepts) — C++20 concepts such as
 *   @c concepts::wrapper.
 * - **Locking utilities** (@ref scl_feature_locking) — @c wrapper_guard,
 *   and @c wrapper_lock for RAII access to wrapper chains.
 */
