#pragma once
#ifndef SCL_WRAPPER_TOOL_HEAP_UNIQUE_H
#define SCL_WRAPPER_TOOL_HEAP_UNIQUE_H

#include <cassert>
#include <memory>
#include <utility>

#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Heap
{
    /*!
     * Инструмент для формирования значения в "куче" на основе умного указателя
     * ::std::unique_ptr. Не поддерживает использование volatile Wrapper из-за
     * ограничений ::std::unique_ptr.
     */
    struct Unique
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;
            using Pointer = ::std::unique_ptr< _Value >;

            Pointer m_pointer;

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
                : m_pointer( ::std::make_unique< Value >( ::std::forward< _Arguments >( arguments ) ... ) )
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

            ~Holder ()
            {
                m_pointer.reset();
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
                // NOTE: Functionality ::std::unique_ptr has a limitation for volatile case.
                return ::std::forward< ValueRefer >( *holder.m_pointer.get() );
            }
        };
    };
}}}

#endif
