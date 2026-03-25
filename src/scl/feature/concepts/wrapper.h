#pragma once

#include <scl/feature/type_traits/wrapper.h>

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

    template <typename Expected, typename Type>
    concept compatible_with = ::scl::feature::is_compatible_with_v<Expected, Type>;

    template <typename Expected, typename Type>
    concept compatible_with_part_of = ::scl::feature::is_compatible_with_part_of_v<Expected, Type>;

    template <typename Expected, typename Type>
    concept part_compatible_with = ::scl::feature::is_part_compatible_with_v<Expected, Type>;
} // namespace scl::feature::concepts
