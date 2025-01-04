#pragma once
#ifndef SCL_FEATURE_REFLECTION_VECTOR_H
#define SCL_FEATURE_REFLECTION_VECTOR_H

#include <ScL/Feature/MixIn.h>
#include <ScL/Feature/Reflection.h>
#include <vector>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Value_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::vector<Value_, Allocator_>>
    {
    public:
        SCL_REFLECT_METHOD( assign )
        SCL_REFLECT_METHOD( assign_range )
        SCL_REFLECT_METHOD( get_allocator )
        SCL_REFLECT_METHOD( at)
        SCL_REFLECT_METHOD( front )
        SCL_REFLECT_METHOD( back )
        SCL_REFLECT_METHOD( data )
        SCL_REFLECT_METHOD( begin )
        SCL_REFLECT_METHOD( cbegin )
        SCL_REFLECT_METHOD( end )
        SCL_REFLECT_METHOD( cend )
        SCL_REFLECT_METHOD( rbegin )
        SCL_REFLECT_METHOD( crbegin )
        SCL_REFLECT_METHOD( rend )
        SCL_REFLECT_METHOD( crend )
        SCL_REFLECT_METHOD( empty )
        SCL_REFLECT_METHOD( size )
        SCL_REFLECT_METHOD( max_size )
        SCL_REFLECT_METHOD( reserve )
        SCL_REFLECT_METHOD( capacity )
        SCL_REFLECT_METHOD( shrink_to_fit )
        SCL_REFLECT_METHOD( clear )
        SCL_REFLECT_METHOD( insert )
        SCL_REFLECT_METHOD( insert_range )
        SCL_REFLECT_METHOD( emplace )
        SCL_REFLECT_METHOD( erase )
        SCL_REFLECT_METHOD( push_back )
        SCL_REFLECT_METHOD( emplace_back )
        SCL_REFLECT_METHOD( append_range )
        SCL_REFLECT_METHOD( pop_back )
        SCL_REFLECT_METHOD( resize )
        SCL_REFLECT_METHOD( swap )
    };
}}

#endif
