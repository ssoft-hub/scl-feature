#pragma once
#ifndef SCL_FEATURED_MIXIN_H
#define SCL_FEATURED_MIXIN_H

#include <type_traits>

namespace ScL::Feature::Detail
{
    template <typename Self_, typename Value_>
    class ReflectionMixIn
    {};

    template <typename Self_, typename Value_>
    class CastingMixIn
    {};
}

namespace ScL::Feature
{
    template <typename Self_, typename Value_>
    class ValueReflectionMixIn {};

    template <typename Self_, typename Value_>
    class ToolReflectionMixIn {};
}

namespace ScL::Feature
{
    template <typename Self_>
    class ToolAdditionMixIn {};
}

namespace ScL::Feature
{
    template <typename Self_>
    class MixIn
        : public ::ScL::Feature::ToolAdditionMixIn<Self_>
        , public ::ScL::Feature::Detail::CastingMixIn<Self_, ::std::remove_cv_t<::std::remove_reference_t<Self_>>>
        , public ::ScL::Feature::Detail::ReflectionMixIn<Self_, ::std::remove_cv_t<::std::remove_reference_t<Self_>>>
    {};
}

#endif
