#pragma once
#ifndef SCL_FEATURE_REFLECTION_VECTOR_H
#define SCL_FEATURE_REFLECTION_VECTOR_H

#include <vector>
#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Value_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::vector<Value_, Allocator_>>
    {
    public:
        template <typename ... Args_>
        decltype(auto) reserve (Args_ && ... /*args*/) const
        {
        }

        template <typename ... Args_>
        decltype(auto) emplace_back (Args_ && ... /*args*/)
        {
        }
    };
}}

#endif
