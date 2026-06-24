#pragma once

#include <concepts>
#include <functional>

/**
 * @defgroup scl_feature_concepts_executor Executor Concept
 * @ingroup scl_feature_concepts
 * @brief C++20 concept for executor types used with @c scl::wrapper.
 * @{
 */

namespace scl::feature::concepts
{
    /**
     * @brief Satisfied when @p T models the executor protocol.
     *
     * An executor type must expose:
     * - a @c value_type member alias,
     * - a static @c value(Self&&) member that returns a reference to the held value,
     * - a static @c execute(Self&&, Func&&, Args&&...) member that invokes @p Func.
     *
     * @tparam T  Type to check.
     *
     * @ingroup scl_feature_concepts_executor
     *
     * @code{.cpp}
     *    static_assert(scl::feature::concepts::executor<
     *        feature::inplace::plain<int>>);
     * @endcode
     */
    template <typename T>
    concept executor = requires { typename T::value_type; } &&
        requires(T & t, void (*fn)()) {
            {
                T::value(t)
            } -> std::convertible_to<typename T::value_type &>;
            T::execute(t, fn);
        };
} // namespace scl::feature::concepts

/** @} */ // end of group scl_feature_concepts_executor
