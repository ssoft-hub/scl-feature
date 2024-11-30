#pragma once
#ifndef SCL_GUARD_FEATURE_POINTER_H
#define SCL_GUARD_FEATURE_POINTER_H

#include <cassert>
#include <ScL/Utility/SimilarRefer.h>

#include "Detail/HolderInterface.h"

namespace ScL { namespace Feature { namespace Detail { template < typename _Value, typename _Tool > class Wrapper; }}}
namespace ScL { namespace Feature { namespace Detail { template < typename _Refer > struct WrapperGuardHelper; }}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Refer >
    using WrapperGuard = typename ::ScL::Feature::Detail::WrapperGuardHelper< _Refer >::Type;
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Указатель на экземпляр вложенного в Wrapper значения, к которому применена
     * особенность, реализуемая данным Wrapper. Данный указатель применяется, если
     * тип вложенного экземпляра значения сам не является Wrapper.
     */
    template < typename _Refer >
    class DefaultWrapperGuard
    {
        using ThisType = DefaultWrapperGuard< _Refer >;

    public:
        using Refer = _Refer;
        using Pointer = ::std::add_pointer_t< ::std::remove_reference_t< Refer > >;

        using ValueAccess = Refer;
        using PointerAccess = Pointer;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

    private:
        Pointer m_pointer;

    private:
        DefaultWrapperGuard ( ThisType && other ) = delete;
        DefaultWrapperGuard ( const ThisType & other ) = delete;

    public:
        constexpr DefaultWrapperGuard ( Refer refer )
            : m_pointer( ::std::addressof( refer ) )
        {
        }

        constexpr ValueAccess valueAccess () const
        {
            return ::std::forward< ValueAccess >( *m_pointer );
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_pointer;
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Гарант применения особенностей к экземпляру вложенного в Wrapper значения.
     * Данный вариант применяется, если тип вложенного экземпляра значения сам является Wrapper.
     */
    template < typename _Refer >
    class SpecialWrapperGuard
    {
        using ThisType = SpecialWrapperGuard< _Refer >;

    public:
        using WrapperRefer = _Refer;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;
        using WrapperPointer = ::std::add_pointer_t< ::std::remove_reference_t< WrapperRefer > >;

        //using ReferPointer = ::ScL::Feature::Detail::ReferPointer< WrapperRefer >;

        using WrapperAccess = ValueRefer;
        using HolderAccess = HolderRefer;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        WrapperPointer m_pointer;

    private:
        SpecialWrapperGuard ( ThisType && other ) = delete;
        SpecialWrapperGuard ( const ThisType & other ) = delete;

    public:
        constexpr SpecialWrapperGuard ( WrapperRefer refer ) noexcept
            : m_pointer( ::std::addressof( refer ) )
        {
            static_assert( ::ScL::Feature::Detail::HolderInterface::doesValueStaticMethodExist< Holder, HolderRefer >()
                , "There are no appropriate access methods for Holder." );
            ::ScL::Feature::Detail::HolderInterface::guard< HolderRefer >( holderAccess() );
        }

        ~SpecialWrapperGuard ()
        {
            ::ScL::Feature::Detail::HolderInterface::unguard< HolderRefer >( holderAccess() );
        }

        constexpr WrapperAccess wrapperAccess () const noexcept
        {
            return ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( holderAccess() );
        }

        constexpr HolderAccess holderAccess () const noexcept
        {
            return ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( ::std::forward< WrapperRefer >( *m_pointer ) );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    struct ValueCase;
    struct WrapperCase;

    template < typename _Refer >
    struct WrapperGuardCaseHelper
    {
        using Type = ::std::conditional_t< ::ScL::Feature::isWrapper< ::std::decay_t< _Refer > >(),
            WrapperCase,
            ValueCase >;
    };

    template < typename _Refer >
    using WrapperGuardSwitchCase = typename WrapperGuardCaseHelper< _Refer >::Type;

    template < typename, typename >
    struct WrapperSwith;

    template < typename _Refer >
    struct WrapperSwith< ValueCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::DefaultWrapperGuard< _Refer >;
    };

    template < typename _Refer >
    struct WrapperSwith< WrapperCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::SpecialWrapperGuard< _Refer >;
    };

    template < typename _Refer >
    struct WrapperGuardHelper
    {
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." );
        using Type = typename WrapperSwith< WrapperGuardSwitchCase< _Refer >, _Refer >::Type;
    };
}}}

#endif
