#pragma once
#ifndef SCL_FEATURE_REFLECTION_UNORDERED_SET_H
#define SCL_FEATURE_REFLECTION_UNORDERED_SET_H

#include <ScL/Feature/MixIn.h>
#include <unordered_set>

namespace ScL::Feature
{
    template <typename Self_,
        typename SelfHolder_,
        typename Key_,
        typename Hash_,
        typename Pred_,
        typename Allocator_>
    class ValueReflectionMixIn<Self_,
        SelfHolder_,
        ::std::unordered_set<Key_, Hash_, Pred_, Allocator_>>
    {};
} // namespace ScL::Feature

#endif
