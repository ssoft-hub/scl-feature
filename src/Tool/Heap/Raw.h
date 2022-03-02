#pragma once
#ifndef SCL_WRAPPER_TOOL_HEAP_RAW_H
#define SCL_WRAPPER_TOOL_HEAP_RAW_H

#include <cassert>
#include <memory>
#include <utility>

#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Heap
{
    /*!
     * Инструмент для формирования значения в "куче" на основе raw указателя.
     */
    struct Raw
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;
            using Pointer = _Value *;

            Pointer m_pointer;

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
                : m_pointer( new Value( ::std::forward< _Arguments >( arguments ) ... ) )
            {
            }

            Holder ( ThisType && other )
                : m_pointer( ::std::forward< Pointer >( other.m_pointer ) )
            {
                other.m_pointer = nullptr;
            }

            Holder ( const ThisType && other )
                : Holder( ::std::forward< const Value >( *other.m_pointer ) )
            {
            }

            Holder ( volatile ThisType && other )
                : m_pointer( ::std::forward< volatile Pointer >( other.m_pointer ) )
            {
                other.m_pointer = nullptr;
            }

            Holder ( const volatile ThisType && other )
                : Holder( ::std::forward< const volatile Value >( *other.m_pointer ) )
            {
            }

            Holder ( ThisType & other )
                : Holder( *other.m_pointer )
            {
            }

            Holder ( const ThisType & other )
                : Holder( *other.m_pointer )
            {
            }

            Holder ( volatile ThisType & other )
                : Holder( *other.m_pointer )
            {
            }

            Holder ( const volatile ThisType & other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< typename Holder< _OtherValue >::Pointer >( other.m_pointer ) )
            {
                other.m_pointer = nullptr;
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > && other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< volatile typename Holder< _OtherValue >::Pointer >( other.m_pointer ) )
            {
                other.m_pointer = nullptr;
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > && other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > & other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > & other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > & other )
                : Holder( *other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > & other )
                : Holder( *other.m_pointer )
            {
            }

            ~Holder ()
            {
                if ( m_pointer != nullptr )
                    delete m_pointer;
            }

            /*!
             * Assignment operation between compatible Holders. Specialization
             * of operation enabled if left is not constant any reference and
             * right is not constantrvalue reference.
             */
            template < typename _LeftWrapperRefer, typename _RightWrapperRefer,
            typename = ::std::enable_if_t< !::std::is_const< ::std::remove_reference_t< _LeftWrapperRefer > >::value && !::std::is_const< ::std::remove_reference_t< _RightWrapperRefer > >::value
                && ( ::std::is_volatile< ::std::remove_reference_t< _LeftWrapperRefer > >::value == ::std::is_volatile< ::std::remove_reference_t< _RightWrapperRefer > >::value )
                && ::std::is_rvalue_reference< _RightWrapperRefer && >::value > >
            static decltype(auto) operatorAssignment ( _LeftWrapperRefer && left, _RightWrapperRefer && right )
            {
                ::std::swap( ::ScL::Feature::Detail::wrapperHolder( left ).m_pointer
                    , ::ScL::Feature::Detail::wrapperHolder( right ).m_pointer ) ;
                return ::std::forward< _LeftWrapperRefer >( left );
            }

            //! Access to internal value of Holder for any king of referencies.
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< Value, HolderRefer >;
                return ::std::forward< ValueRefer >( *holder.m_pointer );
            }
        };
    };
}}}

#endif
