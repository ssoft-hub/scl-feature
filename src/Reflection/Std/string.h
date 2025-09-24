#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <ScL/Feature/MixIn.h>

#include <string>

namespace ScL::Feature
{
    template <typename Self_, typename SelfHolder_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::string>
    {
    public:
        template <typename... Args_>
        decltype(auto) length(Args_ &&... /*args*/) const
        {
            return 0;
        }
    };
} // namespace ScL::Feature

#endif
