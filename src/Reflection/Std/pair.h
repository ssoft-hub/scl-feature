#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <ScL/Feature/MixIn.h>
#include <ScL/Feature/Reflection.h>

#include <utility>

namespace ScL::Feature {
template<typename Self_, typename SelfHolder_, typename First_, typename Second_>
class ValueReflectionMixIn<Self_, SelfHolder_, ::std::pair<First_, Second_>>
{
    SCL_REFLECT_PROPERTY(first);
    SCL_REFLECT_PROPERTY(second);

    // using first_type = SCL_DECLTYPE_PROPERTY(first);
    // using second_type = SCL_DECLTYPE_PROPERTY(second);
};
} // namespace ScL::Feature

#endif
