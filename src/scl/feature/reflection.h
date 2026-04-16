#pragma once

/**
 * @file
 * @brief ScL Feature compile-time method reflection utilities.
 * @ingroup scl_feature_reflection
 * @details This header aggregates all ScL Feature reflection macros and types.
 */

/**
 * @defgroup scl_feature_reflection ScL Feature Reflection
 * @brief Compile-time reflection for wrapper types.
 *
 * @details
 * Provides the macros and types needed to generate proxy members that
 * transparently forward calls from a wrapper to the held object through
 * an executor, preserving all cv-ref qualifiers.
 *
 * | Symbol | Purpose |
 * |--------|---------|
 * | @c SCL_REFLECT_TYPE(Wrapper, Executor) | Declares the type aliases required by @c SCL_REFLECT_METHOD. Must appear first inside the wrapper class body. |
 * | @c SCL_REFLECT_METHOD(method) | Generates 24 proxy overloads (3 × 8 cv-ref) that dispatch through the executor. |
 * @{
 */

#include <scl/feature/reflection/method.h>
#include <scl/feature/reflection/type.h>

/** @} */ // end of group scl_feature_reflection
