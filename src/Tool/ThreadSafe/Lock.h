#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_LOCK_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_LOCK_H

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace ThreadSafe
{
    template < typename _Lock, typename _HolderRefer >
    struct Locking;
}}}

namespace ScL { namespace Feature { namespace ThreadSafe
{
    /*!
     * Инструмент для формирования значения "по месту" с добавлением
     * объекта блокировки _Lock типа.
     */
    template < typename _Lock >
    struct Lock
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;
            using LockType = _Lock;

            mutable LockType m_lock;
            Value m_value;

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
                : m_lock{}
                , m_value{ ::std::forward< _Arguments >( arguments ) ... }
            {
            }

            Holder ( ThisType && other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< ThisType && >;
                OtherMovableGuard guard( other );
                m_value = ::std::forward< Value >( other.m_value );
            }

            Holder ( const ThisType && other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const ThisType && >;
                OtherReadableGuard guard( other );
                m_value = ::std::forward< const Value >( other.m_value );
            }

            Holder ( volatile ThisType && other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< volatile ThisType && >;
                OtherMovableGuard guard( other );
                m_value = ::std::forward< volatile Value >( other.m_value );
            }

            Holder ( const volatile ThisType && other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const volatile ThisType && >;
                OtherReadableGuard guard( other );
                m_value = ::std::forward< const volatile Value >( other.m_value );
            }

            Holder ( ThisType & other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< ThisType & >;
                OtherMovableGuard guard( other );
                m_value = other.m_value;
            }

            Holder ( const ThisType & other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const ThisType & >;
                OtherReadableGuard guard( other );
                m_value = other.m_value;
            }

            Holder ( volatile ThisType & other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< volatile ThisType & >;
                OtherMovableGuard guard( other );
                m_value = other.m_value;
            }

            Holder ( const volatile ThisType & other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const volatile ThisType & >;
                OtherReadableGuard guard( other );
                m_value = other.m_value;
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > && other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< Holder< _OtherValue > && >;
                OtherMovableGuard guard( other );
                m_value = ::std::forward< Value >( other.m_value );
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > && other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const Holder< _OtherValue > && >;
                OtherReadableGuard guard( other );
                m_value = ::std::forward< const Value >( other.m_value );
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > && other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< volatile Holder< _OtherValue > && >;
                OtherMovableGuard guard( other );
                m_value = ::std::forward< Value >( other.m_value );
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > && other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const volatile Holder< _OtherValue > && >;
                OtherReadableGuard guard( other );
                m_value = ::std::forward< const volatile Value >( other.m_value );
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > & other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< Holder< _OtherValue > & >;
                OtherMovableGuard guard( other );
                m_value = other.m_value;
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > & other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const Holder< _OtherValue > & >;
                OtherReadableGuard guard( other );
                m_value = other.m_value;
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > & other )
                : m_lock{}
            {
                using OtherMovableGuard = ::ScL::Feature::HolderGuard< volatile Holder< _OtherValue > & >;
                OtherMovableGuard guard( other );
                m_value = other.m_value;
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > & other )
                : m_lock{}
            {
                using OtherReadableGuard = ::ScL::Feature::HolderGuard< const volatile Holder< _OtherValue > & >;
                OtherReadableGuard guard( other );
                m_value = other.m_value;
            }

            ~Holder ()
            {
                using WritableGuard = ::ScL::Feature::HolderGuard< ThisType & >;
                WritableGuard guard( *this );
            }

            //! Guard internal value of Holder for any king of referencies.
            template < typename _HolderRefer >
            static constexpr void guard ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                ::ScL::Feature::ThreadSafe::Locking< LockType, ::std::remove_reference_t< HolderRefer > >::lock( ::std::forward< HolderRefer >( holder ).m_lock );
            }

            //! Unguard internal value of Holder for any king of referencies.
            template < typename _HolderRefer >
            static constexpr void unguard ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                ::ScL::Feature::ThreadSafe::Locking< LockType, ::std::remove_reference_t< HolderRefer > >::unlock( ::std::forward< HolderRefer >( holder ).m_lock );
            }

            //! Access to internal value of Holder for any king of referencies.
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< _Value, HolderRefer >;
                return ::std::forward< ValueRefer >( holder.m_value );
            }
        };
    };
}}}

#endif
