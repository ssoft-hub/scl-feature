#pragma once
#ifndef SCL_FEATURE_REFLECTION_LIST_H
#define SCL_FEATURE_REFLECTION_LIST_H

#include <list>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Value_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::list<Value_, Allocator_>>
    {
    };
}}

#endif
