#pragma once
#ifndef SCL_ACCESS_HOLDER_INTERFACE_H
#define SCL_ACCESS_HOLDER_INTERFACE_H

#include <memory>
#include <ScL/Feature/Trait.h>
#include <ScL/Meta/Trait/DoesMethodExist.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _WrapperRefer >
    struct WrapperAccess
    {
        static_assert( ::std::is_reference< _WrapperRefer >::value,
            "The template parameter _WrapperRefer must to be a reference type." );

        using WrapperRefer = _WrapperRefer;
        using Wrapper = ::std::decay_t< WrapperRefer >;

        static_assert( ::ScL::Feature::isWrapper< Wrapper >(),
            "The template parameter _WrapperRefer must to be a Wrapper reference type." );

        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;

        static constexpr HolderRefer holderRefer ( WrapperRefer refer )
        {
            return ::std::forward< HolderRefer >( refer.m_holder );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _WrapperRefer,
        typename = ::std::enable_if_t< ::ScL::Feature::isWrapper< ::std::decay_t< _WrapperRefer > >() > >
    constexpr ::ScL::SimilarRefer< typename ::std::decay_t< _WrapperRefer >::Holder, _WrapperRefer && > wrapperHolder ( _WrapperRefer && refer )
    {
        using WrapperRefer = _WrapperRefer &&;
        return ::ScL::Feature::Detail::WrapperAccess< WrapperRefer >::holderRefer( ::std::forward< WrapperRefer >( refer ) );
    }
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _HolderRefer, bool has_method >
    struct GuardHelper;

    template < typename _HolderRefer >
    struct GuardHelper< _HolderRefer, false >
    {
        static_assert( ::std::is_reference< _HolderRefer >::value, "The template parameter _HolderRefer type must to be a reference type." );
        static constexpr void guard ( _HolderRefer ) {}
    };

    template < typename _HolderRefer >
    struct GuardHelper< _HolderRefer, true >
    {
        static_assert( ::std::is_reference< _HolderRefer >::value, "The template parameter _HolderRefer type must to be a reference type." );
        using Holder = ::std::decay_t< _HolderRefer >;
        static constexpr void guard ( _HolderRefer holder )
        {
            Holder::guard( ::std::forward< _HolderRefer >( holder ) );
        }
    };

    template < typename _HolderRefer, bool has_method >
    struct UnguardHelper;

    template < typename _HolderRefer >
    struct UnguardHelper< _HolderRefer, false >
    {
        static_assert( ::std::is_reference< _HolderRefer >::value, "The template parameter _HolderRefer type must to be a reference type." );
        static constexpr void unguard ( _HolderRefer ) {}
    };

    template < typename _HolderRefer >
    struct UnguardHelper< _HolderRefer, true >
    {
        static_assert( ::std::is_reference< _HolderRefer >::value, "The template parameter _HolderRefer type must to be a reference type." );
        static constexpr void unguard ( _HolderRefer holder )
        {
            using Holder = ::std::decay_t< _HolderRefer >;
            Holder::unguard( ::std::forward< _HolderRefer >( holder ) );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    struct HolderInterface
    {
        SCL_DOES_METHOD_EXIST_TRAIT( guard )
        SCL_DOES_METHOD_EXIST_TRAIT( unguard )
        SCL_DOES_METHOD_EXIST_TRAIT( value )

        template < typename _HolderRefer,
            typename = ::std::enable_if_t< does_value_method_exist< ::std::decay_t< _HolderRefer >, _HolderRefer > > >
        static constexpr void guard ( _HolderRefer holder )
        {
            using Holder = ::std::decay_t< _HolderRefer >;
            ::ScL::Feature::Detail::GuardHelper< _HolderRefer, does_guard_method_exist< Holder, void( _HolderRefer ) > >::guard( ::std::forward< _HolderRefer >( holder ) );
        }

        template < typename _HolderRefer,
            typename = ::std::enable_if_t< does_value_method_exist< ::std::decay_t< _HolderRefer >, _HolderRefer > > >
        static constexpr void unguard ( _HolderRefer holder )
        {
            using Holder = ::std::decay_t< _HolderRefer >;
            ::ScL::Feature::Detail::UnguardHelper< _HolderRefer, does_unguard_method_exist< Holder, void( _HolderRefer ) > >::unguard( ::std::forward< _HolderRefer >( holder ) );
        }

        template < typename _HolderRefer,
            typename = ::std::enable_if_t< does_value_method_exist< ::std::decay_t< _HolderRefer >, _HolderRefer > > >
        static constexpr decltype(auto) value ( _HolderRefer holder )
        {
            using Holder = ::std::decay_t< _HolderRefer >;
            return Holder::value( ::std::forward< _HolderRefer >( holder ) );
        }
    };
}}}

#endif
