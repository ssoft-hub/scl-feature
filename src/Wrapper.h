#pragma once
#ifndef SCL_FUTURE_WRAPPER_H
#define SCL_FUTURE_WRAPPER_H

#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif

namespace ScL { namespace Feature { namespace Inplace { struct Default; }}}
namespace ScL { namespace Feature { namespace Detail { template < typename, typename > class Wrapper; }}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Value, typename ... _Tools >
    struct MultyToolHelper;

    template < typename _Value >
    struct MultyToolHelper< _Value >
    {
        using Type = typename MultyToolHelper< _Value, ::ScL::Feature::Inplace::Default >::Type;
    };

    template < typename _Value, typename _Tool >
    struct MultyToolHelper< _Value, _Tool >
    {
        using Type = ::ScL::Feature::Detail::Wrapper< _Value, _Tool >;
    };

    template < typename _Value, typename _Tool >
    struct MultyToolHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > >
    {
        using Type = ::ScL::Feature::Detail::Wrapper< _Value, _Tool >;
    };

    template < typename _Value, typename _Tool, typename ... _Tools >
    struct MultyToolHelper< _Value, _Tool, _Tools ... >
    {
        using Type = typename MultyToolHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool >, _Tools ... >::Type;
    };

    template < typename _Value, typename _Tool, typename ... _Tools >
    struct MultyToolHelper< _Value, _Tool, _Tool, _Tools ... >
    {
        using Type = typename MultyToolHelper< _Value, _Tool, _Tools ... >::Type;
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Type > struct WrapperSimplifyHelper { using Type = _Type; };

    template < typename _Value, typename _Tool, typename _OtherTool >
    struct WrapperSimplifyHelper< ::ScL::Feature::Detail::Wrapper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool >, _OtherTool > >
    {
        using Type = ::ScL::Feature::Detail::Wrapper< typename WrapperSimplifyHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > >::Type, _OtherTool >;
    };

    template < typename _Value, typename _Tool >
    struct WrapperSimplifyHelper< ::ScL::Feature::Detail::Wrapper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool >, _Tool > >
    {
        using Type = typename WrapperSimplifyHelper< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > >::Type;
    };
}}}

namespace ScL { namespace Feature
{
    template < typename _Value, typename ... _Tools >
    using Wrapper = typename ::ScL::Feature::Detail::WrapperSimplifyHelper< typename ::ScL::Feature::Detail::MultyToolHelper< _Value, _Tools ... >::Type >::Type;
        //!< This is definition of Wrapper type. No tool duplicates are guarantee.
}}

// ::ScL::Feature::Wrapper must be defined before includes.

#include "Detail/Wrapper.h"

#endif
