#pragma once
#ifndef SCL_FEATURE_REFLECTION_LIST_H
#define SCL_FEATURE_REFLECTION_LIST_H

#include <scl/feature/mixin.h>
#include <list>

namespace ScL::Feature
{
    template <typename Self_, typename SelfHolder_, typename Value_, typename Allocator_>
    class ValueReflectionMixIn<Self_, SelfHolder_, ::std::list<Value_, Allocator_>>
    {};
} // namespace ScL::Feature

#endif
