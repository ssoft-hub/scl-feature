#pragma once
#ifndef SCL_WRAPPER_TOOL_REFERENCE_RAW_H
#define SCL_WRAPPER_TOOL_REFERENCE_RAW_H

#include <utility>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Reference
{
    /*!
     * Инструмент для формирования не владеющей ссылки на экземпляр значения.
     */
    struct Raw
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;
            using ValuePtr = Value *;

            ValuePtr m_value_ptr;

            Holder ( Value && other ) = delete;

            constexpr Holder ( Value & other ) noexcept
                : m_value_ptr( ::std::addressof( other ) )
            {
            }

            constexpr Holder ( ThisType && other ) noexcept
                : m_value_ptr( ::std::forward< ValuePtr >( other.m_value_ptr ) )
            {
            }

            constexpr Holder ( const ThisType && other ) noexcept
                : m_value_ptr( ::std::forward< const ValuePtr >( other.m_value_ptr ) )
            {
            }

            constexpr Holder ( volatile ThisType && other ) noexcept
                : m_value_ptr( ::std::forward< volatile ValuePtr >( other.m_value_ptr ) )
            {
            }

            constexpr Holder ( const volatile ThisType && other ) noexcept
                : m_value_ptr( ::std::forward< const volatile ValuePtr >( other.m_value_ptr ) )
            {
            }

            constexpr Holder ( ThisType & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            constexpr Holder ( const ThisType & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            constexpr Holder ( volatile ThisType & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            constexpr Holder ( const volatile ThisType & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > && other ) noexcept
                : m_value_ptr( ::std::forward< typename Holder< _OtherValue >::ValuePtr >( other.m_value_ptr ) )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > && other ) noexcept
                : m_value_ptr( ::std::forward< typename Holder< const _OtherValue >::ValuePtr >( other.m_value_ptr ) )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > && other ) noexcept
                : m_value_ptr( ::std::forward< typename Holder< volatile _OtherValue >::ValuePtr >( other.m_value_ptr ) )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > && other ) noexcept
                : m_value_ptr( ::std::forward< typename Holder< const volatile _OtherValue >::ValuePtr >( other.m_value_ptr ) )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > & other ) noexcept
                : m_value_ptr( other.m_value_ptr )
            {
            }

            /*!
             * Access to internal value of Holder for any king of referencies.
             */
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder ) noexcept
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< ValuePtr, HolderRefer >;
                return ::std::forward< ValueRefer >( *holder.m_value_ptr );
            }
        };
    };
}}}

#endif
