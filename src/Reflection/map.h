#pragma once
#ifndef SCL_FEATURE_REFLECTION_MAP_H
#define SCL_FEATURE_REFLECTION_MAP_H

#include <map>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Key_, typename Value_, typename Compare_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::map<Key_, Value_, Compare_, Allocator_>>
    {
    };
}}

#endif
