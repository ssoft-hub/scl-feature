#pragma once
#ifndef SCL_FEATURE_REFLECTION_SET_H
#define SCL_FEATURE_REFLECTION_SET_H

#include <ScL/Feature/MixIn.h>

#include <set>

namespace ScL::Feature
{
    template <typename Self_,
        typename SelfHolder_,
        typename Key_,
        typename Compare_,
        typename Allocator_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::set<Key_, Compare_, Allocator_>>
    {};
} // namespace ScL::Feature

#endif
