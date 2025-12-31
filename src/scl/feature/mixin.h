#pragma once
#ifndef SCL_FEATURED_MIXIN_H
#define SCL_FEATURED_MIXIN_H

#include <type_traits>

namespace ScL::Feature::Detail
{
    template <typename Self_, typename SelfHolder_, typename Value_>
    class ReflectionMixIn
    {};

    template <typename Self_, typename Value_>
    class CastingMixIn
    {};
} // namespace ScL::Feature::Detail

namespace ScL::Feature
{
    template <typename Self_, typename SelfHolder_, typename Value_>
    class ValueReflectionMixIn
    {};

    template <typename Self_, typename SelfHolder_, typename Value_>
    class ToolReflectionMixIn
    {};
} // namespace ScL::Feature

namespace ScL::Feature
{
    template <typename Self_>
    class ToolAdditionMixIn
    {};
} // namespace ScL::Feature

namespace ScL::Feature
{
    template <typename Self_, typename SelfHolder_>
    class MixIn
        : public ::ScL::Feature::ToolAdditionMixIn<Self_>
        , public ::ScL::Feature::Detail::CastingMixIn<Self_, Self_>
        , public ::ScL::Feature::Detail::ReflectionMixIn<Self_, SelfHolder_, Self_>
    {
        static_assert(!::std::is_reference_v<Self_>);
        static_assert(!::std::is_reference_v<SelfHolder_>);
    };
} // namespace ScL::Feature

#endif
