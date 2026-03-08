#pragma once

#include <scl/feature/detail/wrapper.h>

#include <type_traits>

namespace scl::feature
{
    /**
     * @ingroup scl_feature_type_traits
     * @brief Checks whether @p Type is a @c wrapper specialization.
     *
     * cv-qualifiers on @p Type are stripped before the check.
     *
     * @tparam Type  Type to check.
     *
     * @code{.cpp}
     *    static_assert(!is_wrapper_v<int>);
     *    static_assert( is_wrapper_v<wrapper<int>>);
     *    static_assert( is_wrapper_v<wrapper<int> const>);  // cv stripped
     * @endcode
     */
    template <typename Type>
    inline constexpr bool is_wrapper_v =
        ::std::is_same_v<::std::remove_cv_t<Type>, Type> ? false : is_wrapper_v<::std::remove_cv_t<Type>>;

    template <typename Type, template <typename> class Executor>
    inline constexpr bool is_wrapper_v<::scl::feature::detail::wrapper<Type, Executor>> = true;

} // namespace scl::feature
