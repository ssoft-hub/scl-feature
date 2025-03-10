#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <string>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::string>
    {
    public:
        template <typename ... Args_>
        decltype(auto) length (Args_ && ... /*args*/) const
        {
            return 0;
        }
    };
}}

#endif
