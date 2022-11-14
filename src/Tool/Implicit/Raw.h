#pragma once
#ifndef SCL_WRAPPER_TOOL_IMPLICIT_RAW_H
#define SCL_WRAPPER_TOOL_IMPLICIT_RAW_H

#include <atomic>
#include <cassert>
#include <memory>
#include <utility>

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Implicit
{
    /*!
     * Инструмент для формирования значения в "куче" на основе raw указателя.
     * Реализует технику ленивых вычислений, когда копирование
     * экземпляра значения происходит только в момент доступа к неконстантному
     * экземпляру.
     */
    template < typename _Counter >
    struct CountedRaw
    {
        struct BaseCounted
        {
            using Counter = _Counter;
            Counter m_counter;
            BaseCounted () : m_counter() {}
            virtual ~BaseCounted () {}
        };

        template < typename _Value >
        struct Counted
            : public BaseCounted
        {
            using ThisType = Counted< _Value >;
            using Value = _Value;

            Value m_value;

            template < typename ... _Arguments >
            Counted ( _Arguments && ... arguments )
                : BaseCounted()
                , m_value( ::std::forward< _Arguments >( arguments ) ... )
            {
            }
        };

        template < typename _Holder, bool abstract_case >
        struct ConstructHelper;

        template < typename _Holder >
        struct ConstructHelper< _Holder, true >
        {
            using Access = typename _Holder::Access;
            using CountedPointer = typename _Holder::CountedPointer;

            template < typename ... _Arguments >
            static CountedPointer makePointer ( _Arguments && ... ) { return nullptr; }
            static Access access ( CountedPointer ) { return nullptr; }
        };

        template < typename _Holder >
        struct ConstructHelper< _Holder, false >
        {
            using Access = typename _Holder::Access;
            using CountedPointer = typename _Holder::CountedPointer;
            using CountedValue = typename _Holder::CountedValue;

            template < typename ... _Arguments >
            static CountedPointer makePointer ( _Arguments && ... arguments ) { return new CountedValue( ::std::forward< _Arguments >( arguments ) ... ); }
            static Access access ( CountedPointer pointer ) { return ::std::addressof( static_cast< CountedValue * >( pointer )->m_value ); }
        };

        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;

            using CountedValue = Counted< Value >;
            using CountedPointer = BaseCounted *;
            using Access = Value *;
            using WritableGuard = ::ScL::Feature::HolderGuard< ThisType & >;

            CountedPointer m_pointer;
            Access m_access; // to resolve multiple inheritance

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
                : m_pointer( ConstructHelper< ThisType, ::std::is_abstract< Value >{} >::makePointer( ::std::forward< _Arguments >( arguments ) ... ) )
                , m_access( ConstructHelper< ThisType, ::std::is_abstract< Value >{} >::access( m_pointer ) )
            {
                increment();
            }

            Holder ( ThisType && other )
                : m_pointer( ::std::forward< CountedPointer >( other.m_pointer ) )
                , m_access( ::std::forward< Access >( other.m_access ) )
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            Holder ( const ThisType && other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            Holder ( volatile ThisType && other )
                : m_pointer( ::std::forward< volatile CountedPointer >( other.m_pointer ) )
                , m_access( ::std::forward< volatile Access >( other.m_access ) )
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            Holder ( const volatile ThisType && other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            Holder ( ThisType & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            Holder ( const ThisType & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            Holder ( volatile ThisType & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            Holder ( const volatile ThisType & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< typename Holder< _OtherValue >::CountedPointer >( other.m_pointer ) )
                , m_access( ::std::forward< typename Holder< _OtherValue >::Access >( other.m_access ) )
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > && other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > && other )
                : m_pointer( ::std::forward< volatile typename Holder< _OtherValue >::CountedPointer >( other.m_pointer ) )
                , m_access( ::std::forward< volatile typename Holder< _OtherValue >::Access >( other.m_access ) )
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > && other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( Holder< _OtherValue > & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( const Holder< _OtherValue > & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( volatile Holder< _OtherValue > & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            template < typename _OtherValue >
            Holder ( const volatile Holder< _OtherValue > & other )
                : m_pointer( other.m_pointer )
                , m_access( other.m_access )
            {
                increment();
            }

            ~Holder ()
            {
                decrement();
            }

            void increment ()
            {
                if ( m_pointer )
                    ++m_pointer->m_counter;
            }

            void decrement ()
            {
                if ( m_pointer && !(--m_pointer->m_counter) )
                {
                    delete m_pointer;
                    m_pointer = nullptr;
                    m_access = nullptr;
                }
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
                auto & left_holder = ::ScL::Feature::Detail::wrapperHolder( left );
                auto & right_holder = ::ScL::Feature::Detail::wrapperHolder( right );

                if ( left_holder.m_pointer != right_holder.m_pointer )
                {
                    left_holder.decrement();
                    left_holder.m_pointer = right_holder.m_pointer;
                    left_holder.m_access = right_holder.m_access;
                    left_holder.increment();
                }

                return ::std::forward< _LeftWrapperRefer >( left );
            }

            /*!
             * Guard internal value of Holder for any not constant referencies.
             */
            template < typename _HolderRefer,
                typename = ::std::enable_if_t<
                        !::std::is_const< ::std::remove_reference_t< _HolderRefer > >{} > >
            static constexpr void guard ( _HolderRefer && holder )
            {
                if ( !!holder.m_pointer && holder.m_pointer->m_counter > 1 )
                {
                    CountedPointer pointer = new CountedValue( *holder.m_access );
                    holder.decrement();
                    holder.m_pointer = pointer;
                    holder.m_access = ::std::addressof( static_cast< CountedValue * >( holder.m_pointer )->m_value );
                    holder.increment();
                }
            }

            /*!
             * Access to internal value of Holder for any king of referencies.
             */
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< Value, HolderRefer >;
                return ::std::forward< ValueRefer >( *holder.m_access );
            }
        };

        template < typename _WrapperRefer >
        static bool isEmpty ( _WrapperRefer && wrapper ) noexcept
        {
            return !::ScL::Feature::Detail::wrapperHolder( wrapper ).m_pointer;
        }

        template < typename _TestWrapper, typename _WrapperRefer >
        static bool isKindOf( _WrapperRefer && wrapper ) noexcept
        {
            using TestAccess = typename _TestWrapper::Holder::Access;
            return dynamic_cast< TestAccess >( ::ScL::Feature::Detail::wrapperHolder( wrapper ).m_access );
        }


        template < typename _LeftWrapperRefer, typename _RightWrapperRefer,
            typename = ::std::enable_if_t< !::std::is_const< ::std::remove_reference_t< _LeftWrapperRefer > >{}
                && ( ::std::is_volatile< ::std::remove_reference_t< _LeftWrapperRefer > >{} == ::std::is_volatile< ::std::remove_reference_t< _RightWrapperRefer > >{} ) > >
        static void staticCast ( _LeftWrapperRefer && left, _RightWrapperRefer && right ) noexcept
        {
            auto & left_holder = ::ScL::Feature::Detail::wrapperHolder( left );
            auto & right_holder = ::ScL::Feature::Detail::wrapperHolder( right );

            if ( left_holder.m_pointer != right_holder.m_pointer )
            {
                left_holder.decrement();
                left_holder.m_pointer = right_holder.m_pointer;
                left_holder.m_access = static_cast< typeof( left_holder.m_access ) >( right_holder.m_access );
                left_holder.increment();
            }
        }
    };
}}}

namespace ScL { namespace Feature { namespace Implicit
{
    using Raw = ::ScL::Feature::Implicit::CountedRaw< ::std::atomic< int32_t > >;
}}}

#include <ScL/Feature/MixIn.h>

namespace ScL { namespace Feature
{
    template < typename _Type >
    class MixIn< ::ScL::Feature::Detail::Wrapper< _Type, ::ScL::Feature::Implicit::Raw > >
    {
        using Extended = ::ScL::Feature::Detail::Wrapper< _Type, ::ScL::Feature::Implicit::Raw >;
        auto & self () const { return *static_cast< Extended const * >( this ); }

    public:
        bool isEmpty() const noexcept
        {
            return Extended::Tool::isEmpty( self() );
        }

        template < typename _Other >
        bool isKindOf () const noexcept
        {
            return Extended::Tool::template isKindOf< _Other >( self() );
        }

        template < typename _Other >
        _Other staticCast () const noexcept
        {
            _Other result;
            Extended::Tool::staticCast( result, self() );
            return result;
        }

        template < typename _Other >
        _Other dynamicCast () const noexcept
        {
            if ( isKindOf< _Other >() )
                return staticCast< _Other >();
            return {};
        }
    };
}}
#endif
