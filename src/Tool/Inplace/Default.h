#pragma once
#ifndef SCL_WRAPPER_TOOL_INPLACE_DEFAULT_H
#define SCL_WRAPPER_TOOL_INPLACE_DEFAULT_H

#include <utility>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Inplace
{
    /*!
     * Инструмент для формирования экземпляра значения "по месту".  Определение "по месту" означает,
     * что для значения не используется динамическое размещение в "куче и оно является неотъемлемой
     * частью области видимости, в котором этот экземпляр значения определен.
     */
    struct Default
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;

            Value m_value;

            template < typename ... _Arguments >
            constexpr Holder ( _Arguments && ... arguments )
            noexcept( ::std::is_nothrow_constructible< Value, _Arguments && ... >() )
                : m_value{ ::std::forward< _Arguments >( arguments ) ... }
            {
            }

            constexpr Holder ( ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, Value && >() )
                : m_value{ ::std::forward< Value >( other.m_value ) }
            {
            }

            constexpr Holder ( const ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, const Value && >() )
                : m_value{ ::std::forward< const Value >( other.m_value ) }
            {
            }

            constexpr Holder ( volatile ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile Value && >() )
                : m_value{ ::std::forward< volatile Value >( other.m_value ) }
            {
            }

            constexpr Holder ( const volatile ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile Value && >() )
                : m_value{ ::std::forward< const volatile Value >( other.m_value ) }
            {
            }

            constexpr Holder ( ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, Value & >() )
                : m_value{ other.m_value }
            {
            }

            constexpr Holder ( const ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, const Value & >() )
                : m_value{ other.m_value }
            {
            }

            constexpr Holder ( volatile ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile Value & >() )
                : m_value{ other.m_value }
            {
            }

            constexpr Holder ( const volatile ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile Value & >() )
                : m_value{ other.m_value }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, _OtherValue && >() )
                : m_value{ ::std::forward< typename Holder< _OtherValue >::Value >( other.m_value ) }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, const _OtherValue && >() )
                : m_value{ ::std::forward< typename Holder< const _OtherValue >::Value >( other.m_value ) }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile _OtherValue && >() )
                : m_value{ ::std::forward< typename Holder< volatile _OtherValue >::Value >( other.m_value ) }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile _OtherValue && >() )
                : m_value{ ::std::forward< typename Holder< const volatile _OtherValue >::Value >( other.m_value ) }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, _OtherValue & >() )
                : m_value{ other.m_value }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, const _OtherValue & >() )
                : m_value{ other.m_value }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile _OtherValue & >() )
                : m_value{ other.m_value }
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile _OtherValue & >() )
                : m_value{ other.m_value }
            {
            }

            /*!
             * Access to internal value of Holder for any king of referencies.
             */
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder ) noexcept
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< Value, HolderRefer >;
                return ::std::forward< ValueRefer >( holder.m_value );
            }
        };
    };
}}}

#endif
