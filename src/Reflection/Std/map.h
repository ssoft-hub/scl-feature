#pragma once
#ifndef SCL_FEATURE_REFLECTION_MAP_H
#define SCL_FEATURE_REFLECTION_MAP_H

#include <map>
#include <ScL/Feature/MixIn.h>
#include <ScL/Feature/Reflection.h>
#include <ScL/Feature/Reflection/Std/pair.h>

namespace ScL { namespace Feature
{
    template <typename Self_, typename Key_, typename Value_, typename Compare_, typename Allocator_>
    class ValueReflectionMixIn<Self_, ::std::map<Key_, Value_, Compare_, Allocator_>>
    {
    public:
        SCL_REFLECT_METHOD( get_allocator )
        SCL_REFLECT_METHOD( at )
        SCL_REFLECT_METHOD( begin )
        SCL_REFLECT_METHOD( cbegin )
        SCL_REFLECT_METHOD( rbegin )
        SCL_REFLECT_METHOD( crbegin )
        SCL_REFLECT_METHOD( rend )
        SCL_REFLECT_METHOD( crend )
        SCL_REFLECT_METHOD( empty )
        SCL_REFLECT_METHOD( size )
        SCL_REFLECT_METHOD( max_size )
        SCL_REFLECT_METHOD( clear )
        SCL_REFLECT_METHOD( insert )
        SCL_REFLECT_METHOD( insert_range )
        SCL_REFLECT_METHOD( insert_or_assign )
        SCL_REFLECT_METHOD( emplace )
        SCL_REFLECT_METHOD( emplace_hint )
        SCL_REFLECT_METHOD( try_emplace )
        SCL_REFLECT_METHOD( erase )
        SCL_REFLECT_METHOD( swap )
        SCL_REFLECT_METHOD( extract )
        SCL_REFLECT_METHOD( merge )
        SCL_REFLECT_METHOD( count )
        SCL_REFLECT_METHOD( find )
        SCL_REFLECT_METHOD( contains )
        SCL_REFLECT_METHOD( equal_range )
        SCL_REFLECT_METHOD( lower_bound )
        SCL_REFLECT_METHOD( upper_bound )
        SCL_REFLECT_METHOD( key_comp )
        SCL_REFLECT_METHOD( value_comp )
    };
}}

#endif
