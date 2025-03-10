#pragma once
#ifndef SCL_FEATURE_REFLECTION_UNORDERED_SET_H
#define SCL_FEATURE_REFLECTION_UNORDERED_SET_H

#include <unordered_set>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename SelfHolder_, typename Key_, typename Hash_, typename Pred_, typename Allocator_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::unordered_set<Key_, Hash_, Pred_, Allocator_>>
    {
    };
}}

#endif
