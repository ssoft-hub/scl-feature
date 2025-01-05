#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <utility>
#include <ScL/Feature/MixIn.h>
#include <ScL/Feature/Wrapper.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename First_, typename Second_>
    class ValueReflectionMixIn<Self_, ::std::pair<First_, Second_>>
    {
    public:
        // TODO: define types as Wrappers
        using First = Wrapper<First_>;
        using Second = Wrapper<Second_>;

    private:
        First reflected_first{};
        Second reflected_second{};

    public:
        // First & first{reflected_first};
        // Second & second{reflected_second};
    };
}}

#endif
