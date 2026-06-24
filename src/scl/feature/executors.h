#pragma once

/**
 * @file
 * @brief Aggregates all ScL Feature inplace executor headers.
 * @details Include this header to get all built-in executor types in one shot.
 */

/**
 * @defgroup scl_feature_inplace ScL Feature Inplace Executors
 * @brief Executor types that hold the wrapped value in-place.
 * @{
 */

#include <scl/feature/inplace/plain.h>
#include <scl/feature/inplace/strong_typedef.h>
#include <scl/feature/inplace/uninitialized.h>

/** @} */ // end of group scl_feature_inplace
