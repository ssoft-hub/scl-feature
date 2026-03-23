#pragma once

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
 *
 * @{
 */

#include <scl/feature/detail/wrapper.h>
#include <scl/feature/inplace/plain.h>

#include <type_traits>

namespace scl::feature::detail
{
    template <typename State, template <typename> class... Executor>
    struct wrapper_fold
    {
        using type = State;
    };

    template <typename State, template <typename> class Executor, template <typename> class... Rest>
    struct wrapper_fold<State, Executor, Rest...>
    {
        using type = typename wrapper_fold<::scl::feature::detail::wrapper<State, Executor>, Rest...>::type;
    };

    template <typename Value, template <typename> class Executor, template <typename> class... Rest>
    struct wrapper_fold<::scl::feature::detail::wrapper<Value, Executor>, Executor, Rest...>
    {
        using type = typename wrapper_fold<::scl::feature::detail::wrapper<Value, Executor>, Rest...>::type;
    };
} // namespace scl::feature::detail

namespace scl
{
    /**
     * @ingroup scl_feature_wrapper
     * @brief Composable proxy wrapper that delegates to @p Value through executors.
     *
     * Each executor is a class template @c E such that @c E<Value> proxies
     * method calls and may add cross-cutting behaviour (e.g. COW, thread
     * safety, deferred invocation).  Executors are composed left-to-right;
     * adjacent duplicates in the list are collapsed.  Defaults to
     * @c feature::inplace::plain — a zero-overhead direct proxy — when no executor
     * is specified.
     *
     * @tparam Value      The wrapped value type.
     * @tparam Executors  Zero or more executor class templates.
     *
     * @code{.cpp}
     *    wrapper<int> a{42};                                   // feature::inplace::plain (default)
     *    wrapper<int, feature::inplace::plain> b{42};          // explicit plain
     *    wrapper<int, feature::inplace::uninitialized> c{};    // deferred construction
     * @endcode
     */
    template <typename Value, template <typename> class... Executors>
    using wrapper = ::std::conditional_t<sizeof...(Executors) == 0,
        typename ::scl::feature::detail::wrapper_fold<Value, ::scl::feature::inplace::plain>::type,
        typename ::scl::feature::detail::wrapper_fold<Value, Executors...>::type>;
} // namespace scl

/** @} */ // end of group scl_feature_wrapper
