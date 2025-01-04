#pragma once
#ifndef SCL_FEATURE_DETAIL_METHOD_REFLECTION_H
#define SCL_FEATURE_DETAIL_METHOD_REFLECTION_H

#include "ResultSwitch.h"

#define SCL_REFLECT_METHOD_PROTOTYPE( method, this_refer ) \
    template < typename ... Arguments_ > \
    constexpr decltype(auto) method ( Arguments_ && ... arguments ) this_refer \
    { \
        using WrapperRefer = Self_ this_refer; \
        using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >; \
        auto invokable = [](ValueRefer && value, Arguments_ && ... arguments) \
            -> decltype(::std::declval<ValueRefer>().method( ::std::declval<Arguments_ &&>() ... )) \
        { \
            return ::std::forward<ValueRefer>(value).method( ::std::forward<Arguments_>(arguments) ... ); \
        }; \
        using Invokable = decltype(invokable); \
        using Returned = ::std::invoke_result_t< Invokable, ValueRefer, Arguments_ && ... >; \
         \
        return ::ScL::Feature::Detail::Operator::ResultSwitch< \
            ::ScL::Feature::Detail::Operator::LeftWrapperCase, \
            ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, ValueRefer > > \
                 ::invoke( ::std::forward< Invokable >( invokable ), static_cast< WrapperRefer >( *this ), ::std::forward< Arguments_ >( arguments ) ... ); \
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
