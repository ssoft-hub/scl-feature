#pragma once

/**
 * @file
 * @brief ScL Feature built-in executor implementations.
 * @ingroup scl_feature_executors
 * @details This header aggregates all ScL Feature built-in executor types.
 */

/**
 * @defgroup scl_feature_executors ScL Feature Executors
 * @brief Built-in executor strategies for @c scl::wrapper.
 *
 * @details
 * An executor is a class template that controls how the wrapper stores the
 * held value and dispatches method calls to it.  Each executor must provide
 * two static member function templates:
 *
 * - @c execute(self, func, args...) — runs @p func with @p args and returns
 *   its result; may add cross-cutting behaviour around the call.
 * - @c access(self) — returns a reference to the held value, propagating
 *   the cv-ref qualifiers of @p self.
 *
 * | Executor | Storage | Overhead |
 * |----------|---------|----------|
 * | @c scl::feature::inplace::plain | in-place, direct | none |
 * | @c scl::feature::inplace::uninitialized | in-place, raw aligned storage | none |
 * | @c scl::feature::implicit::indirect | heap, copy-on-write | intrusive counter (1 pointer) |
 * @{
 */

#include <scl/feature/executor/implicit/indirect.h>
#include <scl/feature/executor/inplace/plain.h>
#include <scl/feature/executor/inplace/uninitialized.h>

/** @} */ // end of group scl_feature_executors
