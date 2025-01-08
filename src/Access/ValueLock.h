#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_LOCK_H
#define SCL_FEATURE_ACCESS_VALUE_LOCK_H

#include <ScL/Feature/Access/WrapperLock.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL::Feature::Detail { template < typename _Value, typename _Tool > class Wrapper; }
namespace ScL::Feature::Detail { template < typename > struct ValueLockHelper; }

namespace ScL::Feature
{
    /*!
     * @brief This type activates all features for the multi wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template < typename _Refer >
    using ValueLock = typename ::ScL::Feature::Detail::ValueLockHelper< _Refer >::Type;
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperLock specialization for non wrapped value.
     * It does nothing around a value.
     */
    template < typename _Refer >
    class ValueLockForNonWrapped
    {
        using ThisType = ValueLockForNonWrapped< _Refer >;

    public:
        using Refer = _Refer;
        using WrapperLock = ::ScL::Feature::WrapperLock< _Refer >;

        using ValueAccess = typename WrapperLock::ValueAccess;
        using PointerAccess = typename WrapperLock::PointerAccess;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( !::ScL::Feature::isWrapper< ::std::decay_t< Refer > >(), "The template parameter _Refer must to be a not Wrapper type reference!" );

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
        constexpr void  lockFor ()
        {
            using ExpectedType = ::std::decay_t<Refer>;
            using TestType = ::std::decay_t<Type_>;
            static_assert(::std::is_same_v<ExpectedType, TestType> );
        }

        constexpr ValueAccess valueAccess () const
        {
            return m_wrapper_lock.valueAccess();
        }

        constexpr PointerAccess pointerAccess () const
        {
            return m_wrapper_lock.pointerAccess();
        }
    };
}

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the ValueLock specialization for multi wrapped value.
     * It activates all features in the constructor and deactivate them in the destructor.
     */
    template < typename _Refer >
    class ValueLockForWrapped
    {
        using ThisType = ValueLockForWrapped< _Refer >;

    public:
        using WrapperRefer = _Refer;
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

        static_assert( ::std::is_reference< WrapperRefer >::value, "The template parameter _Refer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Refer must to be a Wrapper type reference!" );
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
            using ExpectedType = ::std::decay_t<WrapperRefer>;
            using TestType = ::std::decay_t<Type_>;

            if constexpr ( ::std::is_same_v<ExpectedType, TestType>())
                return;

            m_wrapper_lock.lock();
            m_value_lock.template lockFor<Type_>();
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
    };
}

namespace ScL::Feature::Detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template < typename _Refer >
    using ValueLockSwitchCase = ::std::conditional_t< ::ScL::Feature::isWrapper< ::std::remove_reference_t< _Refer > >(),
                                                        WrappedCase,
                                                        NonWrappedCase >;

    template < typename, typename >
    struct ValueLockSwitch;

    template < typename _Refer >
    struct ValueLockSwitch< NonWrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::ValueLockForNonWrapped< _Refer >;
    };

    template < typename _Refer >
    struct ValueLockSwitch< WrappedCase, _Refer >
    {
        using Type = ::ScL::Feature::Detail::ValueLockForWrapped< _Refer >;
    };

    template < typename _Refer >
    struct ValueLockHelper
    {
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." );
        using Type = typename ValueLockSwitch< ValueLockSwitchCase< _Refer >, _Refer >::Type;
    };
}

#endif
