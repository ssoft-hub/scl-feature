#pragma once
#ifndef SCL_FEATURE_REFLECTION_UNORDERED_MAP_H
#define SCL_FEATURE_REFLECTION_UNORDERED_MAP_H

#include <unordered_map>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Key_, typename Value_, typename Hash_, typename Pred_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::unordered_map<Key_, Value_, Hash_, Pred_, Allocator_>>
    {
    };
}}

#endif
