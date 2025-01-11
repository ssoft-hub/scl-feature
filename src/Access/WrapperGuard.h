#pragma once
#ifndef SCL_FEATURE_WRAPPER_GUARD_H
#define SCL_FEATURE_WRAPPER_GUARD_H

#include <utility>
#include <ScL/Utility/SimilarRefer.h>
#include "Detail/HolderInterface.h"

namespace ScL::Feature::Detail { template < typename _Value, typename _Tool > class Wrapper; }
namespace ScL::Feature::Detail { template < typename _Refer > struct WrapperGuardHelper; }

namespace ScL::Feature
{
    /*!
     * @brief This type activates a feature for the wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template < typename _Refer >
    using WrapperGuard = typename ::ScL::Feature::Detail::WrapperGuardHelper< _Refer >::Type;
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperGuard specialization for non wrapped value.
     * It does nothing around a value.
     */
    template < typename _Refer >
    class WrapperGuardForNonWrapped
    {
        using ThisType = WrapperGuardForNonWrapped< _Refer >;

    public:
        using Refer = _Refer;
        using Pointer = ::std::add_pointer_t< ::std::remove_reference_t< Refer > >;

        using ValueAccess = Refer;
        using PointerAccess = Pointer;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

    private:
        Refer m_refer;

    private:
        WrapperGuardForNonWrapped ( ThisType && other ) = delete;
        WrapperGuardForNonWrapped ( const ThisType & other ) = delete;

    public:
        constexpr WrapperGuardForNonWrapped ( Refer refer )
            : m_refer(::std::forward<Refer>(refer))
        {
        }

        constexpr ValueAccess valueAccess () const
        {
            return ::std::forward< ValueAccess >( m_refer );
        }

        constexpr PointerAccess pointerAccess () const
        {
            return ::std::addressof(m_refer);
        }
    };
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperGuard specialization for wrapped value.
     * It activates a feature in the constructor and deactivate it in the destructor.
     */
    template < typename _Refer >
    class WrapperGuardForWrapped
    {
        using ThisType = WrapperGuardForWrapped< _Refer >;

    public:
        using WrapperRefer = _Refer;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;

        //using ReferPointer = ::ScL::Feature::Detail::ReferPointer< WrapperRefer >;

        using WrapperAccess = ValueRefer;
        using HolderAccess = HolderRefer;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        WrapperRefer m_refer;

    private:
        WrapperGuardForWrapped ( const ThisType & other ) = delete;
        WrapperGuardForWrapped ( ThisType && other ) = delete;

    public:
        constexpr WrapperGuardForWrapped ( WrapperRefer refer ) noexcept
            : m_refer( ::std::forward<WrapperRefer>( refer ) )
        {
            static_assert( ::ScL::Feature::Detail::HolderInterface::doesValueStaticMethodExist< Holder, HolderRefer >()
                , "There are no appropriate access methods for Holder." );
            ::ScL::Feature::Detail::HolderInterface::guard< HolderRefer >( holderAccess() );
        }

        ~WrapperGuardForWrapped ()
        {
            ::ScL::Feature::Detail::HolderInterface::unguard< HolderRefer >( holderAccess() );
        }

        constexpr WrapperAccess wrapperAccess () const noexcept
        {
            return ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( holderAccess() );
        }

        constexpr HolderAccess holderAccess () const noexcept
        {
            return ::ScL::Feature::Detail::wrapperHolder< WrapperRefer >( ::std::forward< WrapperRefer >( m_refer ) );
        }
    };
}

namespace ScL::Feature::Detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template < typename _Refer >
    using WrapperGuardSwitchCase = ::std::conditional_t< ::ScL::Feature::isWrapper< ::std::remove_reference_t< _Refer > >(),
                                                         WrappedCase,
                                                         NonWrappedCase >;

    template < typename, typename >
    struct WrapperGuardSwitch;

    template < typename _Refer >
    struct WrapperGuardSwitch< NonWrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::WrapperGuardForNonWrapped< _Refer >;
    };

    template < typename _Refer >
    struct WrapperGuardSwitch< WrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::WrapperGuardForWrapped< _Refer >;
    };

    template < typename _Refer >
    struct WrapperGuardHelper
    {
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." );
        using Type = typename WrapperGuardSwitch< WrapperGuardSwitchCase< _Refer >, _Refer >::Type;
    };
}

#endif
