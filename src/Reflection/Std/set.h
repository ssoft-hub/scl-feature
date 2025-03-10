#pragma once
#ifndef SCL_FEATURE_REFLECTION_SET_H
#define SCL_FEATURE_REFLECTION_SET_H

#include <set>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename SelfHolder_, typename Key_, typename Compare_, typename Allocator_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::set<Key_, Compare_, Allocator_>>
    {
    };
}}

#endif
