#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_LOCK_H
#define SCL_FEATURE_ACCESS_VALUE_LOCK_H

#include <ScL/Feature/Access/WrapperLock.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL::Feature::Detail { template < typename, typename > class Wrapper; }
namespace ScL::Feature::Detail { template < typename > struct ValueLockHelper; }

namespace ScL::Feature
{
    /*!
     * @brief This type activates all features for the multi wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template < typename Refer_ >
    using ValueLock = typename ::ScL::Feature::Detail::ValueLockHelper< Refer_ >::Type;
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperLock specialization for non wrapped value.
     * It does nothing around a value.
     */
    template < typename Refer_ >
    class ValueLockForNonWrapped
    {
        using ThisType = ValueLockForNonWrapped< Refer_ >;

    public:
        using Refer = Refer_;
        using WrapperLock = ::ScL::Feature::WrapperLock< Refer_ >;

        using ValueAccess = typename WrapperLock::ValueAccess;
        using PointerAccess = typename WrapperLock::PointerAccess;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter Refer_ must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter Refer_ must to be a not Wrapper type reference!" );

    private:
        WrapperLock m_wrapper_lock;

    private:
        ValueLockForNonWrapped ( ThisType const & other ) = delete;
        ValueLockForNonWrapped ( ThisType && other ) = delete;

    public:
        constexpr ValueLockForNonWrapped ( Refer refer )
            : m_wrapper_lock( ::std::forward< Refer >( refer ) )
        {
        }

        constexpr ValueLockForNonWrapped ( WrapperLock && other )
            : m_wrapper_lock( ::std::forward< WrapperLock >( other ) )
        {
        }

        ~ValueLockForNonWrapped ()
        {
            unlock();
        }

        constexpr void  lock () noexcept {}
        constexpr void  unlock () noexcept {}

        template <typename Type_>
        constexpr auto lockFor ()
            -> ::std::enable_if_t<::std::is_same_v<Refer_, Type_>, void>
        {
        }

        constexpr ValueAccess valueAccess () const
        {
            return m_wrapper_lock.valueAccess();
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_wrapper_lock.pointerAccess();
        }

        template <typename Type_>
        constexpr auto valueAccessFor () const noexcept
            -> ::std::enable_if_t< ::std::is_same_v<Refer_, Type_>, Type_ >
        {
            return valueAccess();
        }
    };
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the ValueLock specialization for multi wrapped value.
     * It activates all features in the constructor and deactivate them in the destructor.
     */
    template < typename Refer_ >
    class ValueLockForWrapped
    {
        using ThisType = ValueLockForWrapped< Refer_ >;

    public:
        using WrapperRefer = Refer_;
        using Wrapper = ::std::decay_t< WrapperRefer >;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >;

        using WrapperLock = ::ScL::Feature::WrapperLock< WrapperRefer >;
        using ValueLock = ::ScL::Feature::ValueLock< ValueRefer >;

        using WrapperAccess = typename WrapperLock::WrapperAccess;
        using ValueAccess =  typename ValueLock::ValueAccess;
        using PointerAccess = typename ValueLock::PointerAccess;

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter Refer_ must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter Refer_ must to be a Wrapper type reference!" );
        //static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and ValueRefer must to be similar types!" );
        static_assert( ::ScL::Feature::isSimilar< HolderRefer, WrapperRefer >(), "The Refer and HolderRefer must to be similar types!" );

    private:
        WrapperLock m_wrapper_lock;
        ValueLock m_value_lock;

    private:
        ValueLockForWrapped ( ThisType const & other ) = delete;
        ValueLockForWrapped ( ThisType && other ) = delete;

    public:
        constexpr ValueLockForWrapped ( WrapperRefer refer )
            : m_wrapper_lock( ::std::forward< WrapperRefer >( refer ) )
            , m_value_lock( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_lock.holderAccess() ) )
        {
        }

        constexpr ValueLockForWrapped ( WrapperLock && other )
            : m_wrapper_lock( ::std::forward< WrapperLock >( other ) )
            , m_value_lock( ::ScL::Feature::Detail::HolderInterface::value< HolderRefer >( m_wrapper_lock.holderAccess() ) )
        {
        }

        ~ValueLockForWrapped ()
        {
            unlock();
        }

        constexpr void  lock ()
        {
            m_wrapper_lock.lock();
            m_value_lock.lock();
        }

        constexpr void  unlock ()
        {
            m_wrapper_lock.unlock();
            m_value_lock.unlock();
        }

        template <typename Type_>
        constexpr void  lockFor ()
        {
            if constexpr ( !::std::is_same_v<Refer_, Type_> )
            {
                m_wrapper_lock.lock();
                m_value_lock.template lockFor<Type_>();
            }
        }

        constexpr WrapperAccess wrapperAccess () const
        {
            return m_wrapper_lock.wrapperAccess();
        }

        constexpr ValueAccess valueAccess () const
        {
            return m_value_lock.valueAccess();
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_value_lock.pointerAccess();
        }

        template <typename Type_>
        constexpr Type_ valueAccessFor () const
        {
            if constexpr ( ::std::is_same_v<Refer_, Type_> )
                return m_wrapper_lock.valueAccess();
            else
                return m_value_lock.template valueAccessFor<Type_>();
        }
    };
}

namespace ScL::Feature::Detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template < typename Refer_ >
    using ValueLockSwitchCase = ::std::conditional_t< ::ScL::Feature::isWrapper< ::std::remove_reference_t< Refer_ > >(),
                                                        WrappedCase,
                                                        NonWrappedCase >;

    template < typename, typename >
    struct ValueLockSwitch;

    template < typename Refer_ >
    struct ValueLockSwitch< NonWrappedCase, Refer_ >
    {
        using Type = ::ScL::Feature::Detail::ValueLockForNonWrapped< Refer_ >;
    };

    template < typename Refer_ >
    struct ValueLockSwitch< WrappedCase, Refer_ >
    {
        using Type = ::ScL::Feature::Detail::ValueLockForWrapped< Refer_ >;
    };

    template < typename Refer_ >
    struct ValueLockHelper
    {
        static_assert( ::std::is_reference< Refer_ >::value, "The template parameter Refer_ must to be a reference type." );
        using Type = typename ValueLockSwitch< ValueLockSwitchCase< Refer_ >, Refer_ >::Type;
    };
}

#endif
