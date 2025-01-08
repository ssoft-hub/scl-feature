#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_GUARD_H
#define SCL_FEATURE_ACCESS_VALUE_GUARD_H

#include <ScL/Feature/Access/WrapperGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL::Feature::Detail { template < typename _Value, typename _Tool > class Wrapper; }
namespace ScL::Feature::Detail { template < typename > struct ValueGuardHelper; }

namespace ScL::Feature
{
    /*!
     * @brief This type activates all features for the multi wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template < typename _Refer >
    using ValueGuard = typename ::ScL::Feature::Detail::ValueGuardHelper< _Refer >::Type;
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperGuard specialization for non wrapped value.
     * It does nothing around a value.
     */
    template < typename _Refer >
    class ValueGuardForNonWrapped
    {
        using ThisType = ValueGuardForNonWrapped< _Refer >;

    public:
        using Refer = _Refer;
        using WrapperGuard = ::ScL::Feature::WrapperGuard< _Refer >;

        using ValueAccess = typename WrapperGuard::ValueAccess;
        using PointerAccess = typename WrapperGuard::PointerAccess;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

    private:
        WrapperGuard m_wrapper_guard;

    private:
        ValueGuardForNonWrapped ( ThisType && other ) = delete;
        ValueGuardForNonWrapped ( ThisType const & other ) = delete;

    public:
        constexpr ValueGuardForNonWrapped ( Refer refer )
            : m_wrapper_guard( ::std::forward< Refer >( refer ) )
        {
        }

        constexpr ValueGuardForNonWrapped ( WrapperGuard && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other ) )
        {
        }

        constexpr ValueAccess valueAccess () const
        {
            return m_wrapper_guard.valueAccess();
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_wrapper_guard.pointerAccess();
        }
    };
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the ValueGuard specialization for multi wrapped value.
     * It activates all features in the constructor and deactivate them in the destructor.
     */
    template < typename _Refer >
    class ValueGuardForWrapped
    {
        using ThisType = ValueGuardForWrapped< _Refer >;

    public:
        using WrapperRefer = _Refer;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;

        using WrapperGuard = ::ScL::Feature::WrapperGuard< WrapperRefer >;
        using ValueGuard = ::ScL::Feature::ValueGuard< ValueRefer >;

        using WrapperAccess = typename WrapperGuard::WrapperAccess;
        using ValueAccess =  typename ValueGuard::ValueAccess;
        using PointerAccess = typename ValueGuard::PointerAccess;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        WrapperGuard m_wrapper_guard;
        ValueGuard m_value_guard;

    private:
        ValueGuardForWrapped ( ThisType && other ) = delete;
        ValueGuardForWrapped ( ThisType const & other ) = delete;

    public:
        constexpr ValueGuardForWrapped ( WrapperRefer refer )
            : m_wrapper_guard( ::std::forward< WrapperRefer >( refer ) )
            , m_value_guard( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_guard.holderAccess() ) )
        {
        }

        constexpr ValueGuardForWrapped ( WrapperGuard && other )
            : m_wrapper_guard( ::std::forward< WrapperGuard >( other ) )
            , m_value_guard( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_guard.holderAccess() ) )
        {
        }

        constexpr WrapperAccess wrapperAccess () const
        {
            return m_wrapper_guard.wrapperAccess();
        }

        constexpr ValueAccess valueAccess () const
        {
            return m_value_guard.valueAccess();
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_value_guard.pointerAccess();
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
    using ValueGuardSwitchCase = ::std::conditional_t< ::ScL::Feature::isWrapper< ::std::remove_reference_t< _Refer > >(),
                                                        WrappedCase,
                                                        NonWrappedCase >;

    template < typename, typename >
    struct ValueGuardSwitch;

    template < typename _Refer >
    struct ValueGuardSwitch< NonWrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::ValueGuardForNonWrapped< _Refer >;
    };

    template < typename _Refer >
    struct ValueGuardSwitch< WrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::ValueGuardForWrapped< _Refer >;
    };

    template < typename _Refer >
    struct ValueGuardHelper
    {
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." );
        using Type = typename ValueGuardSwitch< ValueGuardSwitchCase< _Refer >, _Refer >::Type;
    };
}

#endif
