#pragma once

#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/concepts/reference.h>

namespace scl::feature::concepts
{
    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p T is a @c wrapper specialization.
     *
     * @tparam T  Type to check.
     *
     * @code{.cpp}
     *    static_assert( wrapper<wrapper<int>>);
     *    static_assert(!wrapper<int>);
     * @endcode
     */
    template <typename Type>
    concept wrapper = ::scl::feature::is_wrapper_v<Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p Type is a reference to a @c wrapper specialization.
     *
     * Combines @c scl::concepts::reference and @c wrapper: the type must be
     * both a reference (lvalue or rvalue) and a wrapper instantiation.
     *
     * @tparam Type  Type to check.
     *
     * @code{.cpp}
     *    static_assert( wrapper_reference<wrapper<int> &>);
     *    static_assert( wrapper_reference<wrapper<int> &&>);
     *    static_assert(!wrapper_reference<wrapper<int>>);
     *    static_assert(!wrapper_reference<int &>);
     * @endcode
     */
    template <typename Type>
    concept wrapper_reference = ::scl::concepts::reference<Type> && wrapper<Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p T is not a @c wrapper specialization.
     *
     * @tparam T  Type to check.
     *
     * @code{.cpp}
     *    static_assert(!non_wrapper<wrapper<int>>);
     *    static_assert( non_wrapper<int>);
     * @endcode
     */
    template <typename Type>
    concept non_wrapper = !::scl::feature::is_wrapper_v<Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p Type is compatible with @p Expected.
     *
     * Two wrappers are compatible when their value and executor types match,
     * meaning one can be constructed directly from the other's executor.
     *
     * @tparam Expected  The target wrapper type.
     * @tparam Type      The source type to check.
     *
     * @code{.cpp}
     *    using W = wrapper<int, plain>;
     *    static_assert( compatible_with<W, W>);
     *    static_assert(!compatible_with<W, wrapper<int, uninitialized>>);
     * @endcode
     */
    template <typename Expected, typename Type>
    concept compatible_with = ::scl::feature::is_compatible_with_v<Expected, Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p Type is compatible with a part of @p Expected.
     *
     * @p Type (a wrapper) is compatible with a nested layer of @p Expected,
     * meaning @p Expected can be constructed with @p Type as its value argument.
     *
     * @tparam Expected  The target wrapper type.
     * @tparam Type      The source type to check.
     *
     * @code{.cpp}
     *    using W  = wrapper<int, plain>;
     *    using WW = wrapper<W,   plain>;
     *    static_assert( compatible_with_part_of<WW, W>);
     *    static_assert(!compatible_with_part_of<W,  WW>);
     * @endcode
     */
    template <typename Expected, typename Type>
    concept compatible_with_part_of = ::scl::feature::is_compatible_with_part_of_v<Expected, Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when a part of @p Expected is compatible with @p Type.
     *
     * A nested layer of @p Expected (a wrapper) is compatible with @p Type,
     * meaning @p Type can be constructed from an inner layer of @p Expected.
     *
     * @tparam Expected  The source wrapper type.
     * @tparam Type      The target type to check.
     *
     * @code{.cpp}
     *    using W  = wrapper<int, plain>;
     *    using WW = wrapper<W,   plain>;
     *    static_assert( part_compatible_with<WW, W>);
     *    static_assert(!part_compatible_with<W,  WW>);
     * @endcode
     */
    template <typename Expected, typename Type>
    concept part_compatible_with = ::scl::feature::is_part_compatible_with_v<Expected, Type>;

    /**
     * @ingroup scl_feature_concepts
     * @brief Satisfied when @p Target is convertible from @p Refer directly or
     *        through any level of the wrapper chain.
     *
     * @tparam Target  The desired reference type (cv-ref qualified).
     * @tparam Refer   The starting reference type (outermost wrapper or plain value).
     *
     * @see scl::feature::is_convertible_from_v
     */
    template <typename Target, typename Refer>
    concept convertible_from = ::scl::feature::is_convertible_from_v<Target, Refer>;
} // namespace scl::feature::concepts
