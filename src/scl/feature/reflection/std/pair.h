#pragma once
#ifndef SCL_FEATURE_REFLECTION_STRING_H
#define SCL_FEATURE_REFLECTION_STRING_H

#include <scl/feature/mixin.h>
#include <scl/feature/reflection.h>

#include <utility>

namespace scl::feature {
template<typename Self_, typename SelfHolder_, typename First_, typename Second_>
class ValueReflectionMixIn<Self_, SelfHolder_, ::std::pair<First_, Second_>>
{
    SCL_REFLECT_PROPERTY(first);
    SCL_REFLECT_PROPERTY(second);

    // using first_type = SCL_DECLTYPE_PROPERTY(first);
    // using second_type = SCL_DECLTYPE_PROPERTY(second);
};
} // namespace scl::feature

#endif
