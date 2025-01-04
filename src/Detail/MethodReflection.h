#pragma once
#ifndef SCL_FEATURE_DETAIL_METHOD_REFLECTION_H
#define SCL_FEATURE_DETAIL_METHOD_REFLECTION_H

#include <type_traits>

#define SCL_REFLECT_METHOD_PROTOTYPE( method, this_refer ) \
    template < typename ... Arguments_ \
        /*requires*/ \
        /*, typename = ::std::void_t< decltype( ::std::declval<Self_ this_refer>()->method( ::std::declval<Arguments_ && >() ... ) ) >*/ > \
    constexpr decltype(auto) method ( Arguments_ && ... arguments ) this_refer \
    { \
        return static_cast< Self_ this_refer >( *this )->method( ::std::forward<Arguments_&&>(arguments) ... ); \
    } \

#define SCL_REFLECT_METHOD( method ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, && ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, const && ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, volatile && ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, const volatile && ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, & ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, const & ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, volatile & ) \
    SCL_REFLECT_METHOD_PROTOTYPE( method, const volatile & ) \

#endif
