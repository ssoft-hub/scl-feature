#pragma once
#ifndef SCL_FEATURE_REFLECTION_UNORDERED_MAP_H
#define SCL_FEATURE_REFLECTION_UNORDERED_MAP_H

#include <scl/feature/mixin.h>
#include <unordered_map>

namespace scl::feature
{
    template <typename Self_,
        typename SelfHolder_,
        typename Key_,
        typename Value_,
        typename Hash_,
        typename Pred_,
        typename Allocator_>
    class ValueReflectionMixIn<Self_,
        SelfHolder_,
        ::std::unordered_map<Key_, Value_, Hash_, Pred_, Allocator_>>
    {};
} // namespace scl::feature

#endif
