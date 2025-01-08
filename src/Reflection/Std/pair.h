#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <utility>
#include <ScL/Feature/MixIn.h>
#include <ScL/Feature/Reflection.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename First_, typename Second_>
    class ValueReflectionMixIn<Self_, ::std::pair<First_, Second_>>
    {
        SCL_REFLECT_PROPERTY(First_, first);
        SCL_REFLECT_PROPERTY(Second_, second);

        using first_type = SCL_PROPERTY_DECLTYPE(first);
        using second_type = SCL_PROPERTY_DECLTYPE(second);
    };
}}

#endif
