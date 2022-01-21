#pragma once
#ifndef SCL_WRAPPER_TOOL_HEAP_SHARED_H
#define SCL_WRAPPER_TOOL_HEAP_SHARED_H

#include <cassert>
#include <memory>
#include <utility>

#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Heap
{
    /*!
     * Инструмент для формирования значения в "куче" на основе умного указателя
     * ::std::shared_ptr. Не поддерживает использование volatile Wrapper из-за
     * ограничений ::std::shared_ptr.
     */
    struct Shared
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;
            using Pointer = ::std::shared_ptr< _Value >;

            Pointer m_pointer;

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
                : m_pointer( ::std::make_shared< Value >( ::std::forward< _Arguments >( arguments ) ... ) )
            {
            }

            Holder ( ThisType && other )
                : m_pointer( ::std::forward< Pointer >( other.m_pointer ) )
            {
            }

            Holder ( const ThisType && other )
                : Holder( *other.m_pointer.get() )
            {
            }

            Holder ( ThisType & other )
                : Holder( *other.m_pointer.get() )
            {
            }

            Holder ( const ThisType & other )
                : Holder( *other.m_pointer.get() )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< typename Holder< _OtherValue >::Pointer >( other.m_pointer ) )
            {
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > && other )
                : Holder( *other.m_pointer.get() )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > & other )
                : Holder( *other.m_pointer.get() )
            {
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > & other )
                : Holder( *other.m_pointer.get() )
            {
            }

            /*!
             * Assignment operation between compatible Holders. Specialization
             * of operation enabled if left is not constant any reference and
             * right is not constant rvalue reference.
             */
            template < typename _LeftHolderRefer, typename _RightHolderRefer,
                typename = ::std::enable_if_t< !::std::is_const< ::std::remove_reference_t< _LeftHolderRefer > >::value
                    && !::std::is_const< ::std::remove_reference_t< _RightHolderRefer > >::value
                    && ::std::is_rvalue_reference< _RightHolderRefer && >::value > >
            static void operatorAssignment ( _LeftHolderRefer && left, _RightHolderRefer && right )
            {
                left.m_pointer.swap( right.m_pointer );
            }

            /*!
             * Access to internal value of Holder for any king of referencies
             * (except volatile).
             */
            template < typename _HolderRefer,
                typename = ::std::enable_if_t< !::std::is_volatile< ::std::remove_reference_t< _HolderRefer > >::value > >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< _Value, HolderRefer >;
                // NOTE: Functionality ::std::shared_ptr has a limitation for volatile case.
                return ::std::forward< ValueRefer >( *holder.m_pointer.get() );
            }
        };
    };
}}}

#endif
