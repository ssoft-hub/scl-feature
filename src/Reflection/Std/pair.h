#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <utility>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename First_, typename Second_>
    class ValueReflectionMixIn<Self_, ::std::pair<First_, Second_>>
    {
    public:
        // TODO: define types as Wrappers
        using First = First_;
        using Second = Second_;

    public:
        First first;
        Second second;
    };
}}

#endif
