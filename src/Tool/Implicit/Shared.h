#pragma once
#ifndef SCL_WRAPPER_TOOL_IMPLICIT_SHARED_H
#define SCL_WRAPPER_TOOL_IMPLICIT_SHARED_H

#include <cassert>
#include <memory>
#include <utility>
#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Implicit
{
    /*!
     * Инструмент для формирования неявно обобщенного значения на основе
     * умного указателя ::std::shared_ptr.
     * Реализует технику ленивых вычислений, когда копирование
     * экземпляра значения происходит только в момент доступа к неконстантному
     * экземпляру.
     */
    struct Shared
    {
        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;

            using Pointer = ::std::shared_ptr< Value >;
            using WritableGuard = ::ScL::Feature::HolderGuard< ThisType & >;

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
                : m_pointer( other.m_pointer )
            {
            }

            Holder ( ThisType & other )
                : m_pointer( other.m_pointer )
            {
            }

            Holder ( const ThisType & other )
                : m_pointer( other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< typename Holder< _OtherValue >::Pointer >( other.m_pointer ) )
            {
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > && other )
                : m_pointer( other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > & other )
                : m_pointer( other.m_pointer )
            {
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > const  & other )
                : m_pointer( other.m_pointer )
            {
            }

            ~Holder ()
            {
                m_pointer.reset();
            }

            /*!
             * Assignment operation between compatible Holders. Specialization
             * of operation enabled if left is not constant reference and any
             * kind of right.
             */
            template < typename _LeftWrapperRefer, typename _RightWrapperRefer,
                typename = ::std::enable_if_t< !::std::is_const< ::std::remove_reference_t< _LeftWrapperRefer > >{}
                    && ( ::std::is_volatile< ::std::remove_reference_t< _LeftWrapperRefer > >{} == ::std::is_volatile< ::std::remove_reference_t< _RightWrapperRefer > >{} ) > >
            static decltype(auto) operatorAssignment ( _LeftWrapperRefer && left, _RightWrapperRefer && right )
            {
                using RightWrapperRefer = _RightWrapperRefer &&;
                using RightHolder = typename ::std::decay_t< RightWrapperRefer >::Holder;
                using RightPointerRefer = ::ScL::SimilarRefer< typename RightHolder::Pointer, RightWrapperRefer >;
                ::ScL::Feature::Detail::wrapperHolder( left ).m_pointer
                    = ::std::forward< RightPointerRefer >( ::ScL::Feature::Detail::wrapperHolder( right ).m_pointer );
            }

            /*!
             * Guard internal value of Holder for any not constant referencies.
             */
            template < typename _HolderRefer,
                typename = ::std::enable_if_t<
                    !::std::is_const< ::std::remove_reference_t< _HolderRefer > >{} > >
            static constexpr void guard ( _HolderRefer && holder )
            {
                if ( !!holder.m_pointer && !holder.m_pointer.unique() )
                    holder.m_pointer = ::std::make_shared< Value >( *holder.m_pointer.get() );
            }

            /*!
             * Access to internal value of Holder for any king of referencies
             * (except volatile).
             */
            template < typename _HolderRefer,
                typename = ::std::enable_if_t< !::std::is_volatile< ::std::remove_reference_t< _HolderRefer > >{} > >
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
