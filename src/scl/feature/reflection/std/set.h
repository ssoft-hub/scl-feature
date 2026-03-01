#pragma once
#ifndef SCL_FEATURE_REFLECTION_SET_H
#define SCL_FEATURE_REFLECTION_SET_H

#include <scl/feature/mixin.h>

#include <set>

namespace scl::feature
{
    template <typename Self_,
        typename SelfHolder_,
        typename Key_,
        typename Compare_,
        typename Allocator_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::set<Key_, Compare_, Allocator_>>
    {};
} // namespace scl::feature

#endif
