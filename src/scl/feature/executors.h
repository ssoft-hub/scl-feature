#pragma once

/**
 * @defgroup scl_feature_executors ScL Executors
 * @brief Convenience header that includes all built-in executor strategies.
 *
 * Include this header to get access to all executor types without
 * including each one individually.
 *
 * @code{.cpp}
 * #include <scl/feature/executors.h>
 *
 * scl::wrapper<int, scl::feature::inplace::plain>        w_plain{42};
 * scl::wrapper<int, scl::feature::inplace::debug>        w_debug{42};
 * scl::wrapper<int, scl::feature::inplace::uninitialized> w_uninitialized{};
 * @endcode
 */

#include <scl/feature/inplace/debug.h>
#include <scl/feature/inplace/plain.h>
#include <scl/feature/inplace/uninitialized.h>
