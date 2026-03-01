#pragma once
#ifndef SCL_FEATURED_MIXIN_H
#define SCL_FEATURED_MIXIN_H

#include <type_traits>

namespace scl::feature::detail
{
    template <typename Self_, typename SelfHolder_, typename Value_>
    class ReflectionMixIn
    {};

    template <typename Self_, typename Value_>
    class CastingMixIn
    {};
} // namespace scl::feature::detail

namespace scl::feature
{
    template <typename Self_, typename SelfHolder_, typename Value_>
    class ValueReflectionMixIn
    {};

    template <typename Self_, typename SelfHolder_, typename Value_>
    class ToolReflectionMixIn
    {};
} // namespace scl::feature

namespace scl::feature
{
    template <typename Self_>
    class ToolAdditionMixIn
    {};
} // namespace scl::feature

namespace scl::feature
{
    template <typename Self_, typename SelfHolder_>
    class MixIn
        : public ::scl::feature::ToolAdditionMixIn<Self_>
        , public ::scl::feature::detail::CastingMixIn<Self_, Self_>
        , public ::scl::feature::detail::ReflectionMixIn<Self_, SelfHolder_, Self_>
    {
        static_assert(!::std::is_reference_v<Self_>);
        static_assert(!::std::is_reference_v<SelfHolder_>);
    };
} // namespace scl::feature

#endif
