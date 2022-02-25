#pragma once
#ifndef SCL_GUARD_FEATURE_POINTER_H
#define SCL_GUARD_FEATURE_POINTER_H

#include <ScL/Utility/SimilarRefer.h>

#include "Detail/HolderInterface.h"
#include "Detail/ReferPointer.h"

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
        using ReferPointer = ::ScL::Feature::Detail::ReferPointer< Refer >;

        using ValueAccess = Refer;
        using PointerAccess = ReferPointer const &;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

    private:
        ReferPointer m_refer_pointer;

    public:
        DefaultWrapperGuard ( Refer refer )
            : m_refer_pointer( ::std::forward< Refer >( refer ) )
        {
        }

        DefaultWrapperGuard ( ThisType && other )
            : m_refer_pointer( ::std::forward< ReferPointer >( other.m_refer_pointer ) )
        {
        }

        ValueAccess valueAccess () const
        {
            return ::std::forward< ValueAccess >( *m_refer_pointer );
        }

        PointerAccess pointerAccess () const
        {
            return m_refer_pointer;
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
        using ReferPointer = ::ScL::Feature::Detail::ReferPointer< WrapperRefer >;

        using WrapperAccess = ValueRefer;
        using HolderAccess = HolderRefer;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        ReferPointer m_refer_pointer;

    private:
        SpecialWrapperGuard ( const ThisType & other ) = delete;

    public:
        SpecialWrapperGuard ( WrapperRefer refer )
            : m_refer_pointer( ::std::forward< WrapperRefer >( refer ) )
        {
            static_assert( ::ScL::Feature::Detail::HolderInterface::doesValueStaticMethodExist< Holder, HolderRefer >()
                , "There are no appropriate access methods for Holder." );
            ::ScL::Feature::Detail::HolderInterface::guard< HolderRefer >( ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( *m_refer_pointer ) );
        }

        SpecialWrapperGuard ( ThisType && other )
            : m_refer_pointer( ::std::forward< ReferPointer >( other.m_refer_pointer ) )
        {
        }

        ~SpecialWrapperGuard ()
        {
            if ( !!m_refer_pointer )
                ::ScL::Feature::Detail::HolderInterface::unguard< HolderRefer >( ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( *m_refer_pointer ) );
        }

        WrapperAccess wrapperAccess () const
        {
            assert( m_refer_pointer );
            return ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( *m_refer_pointer ) );
        }

        HolderAccess holderAccess () const
        {
            assert( m_refer_pointer );
            return ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( *m_refer_pointer );
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
