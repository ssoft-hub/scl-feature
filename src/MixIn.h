#pragma once
#ifndef SCL_FEATURED_MIXIN_H
#define SCL_FEATURED_MIXIN_H

namespace ScL { namespace Feature { namespace Detail
{
    template <typename Self_, typename Value_>
    class ReflectionMixIn
    {};
}}}

namespace ScL { namespace Feature
{
    template <typename Self_, typename Value_>
    class ValueReflectionMixIn {};

    template <typename Self_, typename Value_>
    class ToolReflectionMixIn {};
}}

namespace ScL { namespace Feature
{
    template <typename Self_>
    class ToolMixIn {};
}}

namespace ScL { namespace Feature
{
    template <typename Self_>
    class MixIn
        : public ::ScL::Feature::ToolMixIn<Self_>
        , public ::ScL::Feature::Detail::ReflectionMixIn<Self_, Self_>
    {};
}}

#endif
