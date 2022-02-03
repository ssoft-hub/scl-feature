#pragma once
#ifndef SCL_WRAPPER_TOOL_INPLACE_OPTIONAL_H
#define SCL_WRAPPER_TOOL_INPLACE_OPTIONAL_H

#include <utility>
#include <type_traits>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Inplace
{
    /*!
     * Инструмент для формирования значения "по месту", которое
     * по умолчанию не инициализировано (содержит мусор).
     */
    struct Optional
    {
        class BadOptionalAccess
            : public ::std::exception
        {
        public:
          BadOptionalAccess () {}
          virtual ~BadOptionalAccess () noexcept = default;
          virtual const char* what( ) const noexcept override { return "Bad optional access"; }
        };

        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;

            struct Empty {};
            union
            {
                Empty m_empty;
                Value m_value;
            };
            bool m_is_exists{};

            constexpr Holder ()
            {
            }

            template < typename ... _Arguments >
            Holder ( _Arguments && ... arguments )
            {
                construct( ::std::forward< _Arguments >( arguments ) ... );
            }

            constexpr Holder ( ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, Value && >() )
            {
                if ( other.m_is_exists )
                {
                    construct( ::std::move( other.m_value ) );
                    other.reset();
                }
            }

            constexpr Holder ( const ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, const Value && >() )
            {
                construct( ::std::forward< const Value >( other.m_value ) );
            }

            constexpr Holder ( volatile ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile Value && >() )
            {
                if ( other.m_is_exists )
                {
                    construct( ::std::move( other.m_value ) );
                    other.reset();
                }
            }

            constexpr Holder ( const volatile ThisType && other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile Value && >() )
            {
                construct( ::std::forward< const volatile Value >( other.m_value ) );
            }

            constexpr Holder ( ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, Value & >() )
            {
                construct( other.m_value );
            }

            constexpr Holder ( const ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, const Value & >() )
            {
                construct( other.m_value );
            }

            constexpr Holder ( volatile ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile Value & >() )
            {
                construct( other.m_value );
            }

            constexpr Holder ( const volatile ThisType & other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile Value & >() )
            {
                construct( other.m_value );
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, _OtherValue && >() )
            {
                if ( other.m_is_exists )
                {
                    construct( ::std::move( other.m_value ) );
                    other.reset();
                }
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, const _OtherValue  && >() )
            {
                construct( ::std::forward< typename Holder< const _OtherValue >::Value >( other.m_value ) );
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile _OtherValue && >() )
            {
                if ( other.m_is_exists )
                {
                    construct( ::std::move( other.m_value ) );
                    other.reset();
                }
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > && other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile _OtherValue && >() )
            {
                construct( ::std::forward< typename Holder< const volatile _OtherValue >::Value >( other.m_value ) );
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, _OtherValue & >() )
            {
                construct( other.m_value );
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, const _OtherValue & >() )
            {
                construct( other.m_value );
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, volatile _OtherValue & >() )
            {
                construct( other.m_value );
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > & other )
            noexcept( ::std::is_nothrow_constructible< Value, const volatile _OtherValue & >() )
            {
                construct( other.m_value );
            }

            ~Holder ()
            {
                reset();
            }

            template < typename ... _Arguments >
            void construct( _Arguments && ... _arguments )
            noexcept( ::std::is_nothrow_constructible< Value, _Arguments ... >() )
            {
                ::new ( (void *) ::std::addressof( m_value ) )Value( ::std::forward< _Arguments >( _arguments ) ... );
                m_is_exists = true;
            }

            void reset()
            noexcept
            {
                if ( m_is_exists )
                {
                    m_is_exists = false;
                    m_value.~Value();
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
                if ( !holder.m_is_exists )
                    throw BadOptionalAccess{};
                return ::std::forward< ValueRefer >( holder.m_value );
            }
        };
    };
}}}

#define SCL_OPTIONAL_VALUE_METHOD( refer ) \
    constexpr MixInValue refer value () refer \
    { \
        using WrapperRefer = MixInWrapper refer; \
        using ValueRefer = MixInValue refer; \
        if ( !static_cast< WrapperRefer >( *this ).m_holder.m_is_exists ) \
            throw ::ScL::Feature::Inplace::Optional::BadOptionalAccess{}; \
        return ::std::forward< ValueRefer >( static_cast< WrapperRefer >( *this ).m_holder.m_value ); \
    } \

#define SCL_OPTIONAL_VALUE \
    SCL_OPTIONAL_VALUE_METHOD( & ) \
    SCL_OPTIONAL_VALUE_METHOD( const & ) \
    SCL_OPTIONAL_VALUE_METHOD( volatile & ) \
    SCL_OPTIONAL_VALUE_METHOD( const volatile & ) \
    SCL_OPTIONAL_VALUE_METHOD( && ) \
    SCL_OPTIONAL_VALUE_METHOD( const && ) \
    SCL_OPTIONAL_VALUE_METHOD( volatile && ) \
    SCL_OPTIONAL_VALUE_METHOD( const volatile && ) \

namespace ScL { namespace Feature
{
    namespace Detail { template < typename, typename > class Wrapper; }

    template < typename _Value, typename _Tool >
    class MixIn< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > >
    {
        using MixInValue = _Value;
        using MixInWrapper = ::ScL::Feature::Detail::Wrapper< _Value, _Tool >;
        using MixInHolder = ::ScL::Feature::Inplace::Optional::Holder< _Value >;

    public:
        SCL_OPTIONAL_VALUE

        constexpr MixInValue const & valueOr ( MixInValue const & value ) const &
        noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            if ( !static_cast< WrapperRefer >( *this ).m_holder.m_is_exists )
                return value;
            return static_cast< WrapperRefer >( *this ).m_holder.m_value;
        }

        template< typename _Type,
            typename = ::std::enable_if_t< ::std::is_constructible< MixInValue, _Type && >::value > >
        constexpr MixInValue valueOr ( _Type && value ) const &
        noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            if ( !static_cast< WrapperRefer >( *this ).m_holder.m_is_exists )
                return MixInValue{ ::std::forward< _Type >( value ) };
            return static_cast< WrapperRefer >( *this ).m_holder.m_value;
        }

        template< typename _Type,
            typename = ::std::enable_if_t< ::std::is_constructible< MixInValue, _Type && >::value > >
        MixInValue valueOr ( _Type && value ) &&
        noexcept
        {
            using WrapperRefer = MixInWrapper &&;
            if ( !static_cast< WrapperRefer >( *this ).m_holder.m_is_exists )
                return MixInValue{ ::std::forward< _Type >( value ) };
            return ::std::move( static_cast< WrapperRefer >( *this ).m_holder.m_value );
        }

        constexpr void swap ( MixInWrapper & other ) noexcept
        {
            auto _this = static_cast< MixInWrapper * >( this );

            if ( _this->m_holder.m_is_exists && other.m_holder.m_is_exists )
                ::std::swap( _this->m_holder.m_value, other.m_holder.m_value );
            else if ( _this->m_holder.m_is_exists )
            {
                other.m_holder.construct( ::std::move( _this->m_holder.m_value ) );
                _this->m_holder.reset();
            }
            else if ( other.m_holder.m_is_exists )
            {
                _this->m_holder.construct( ::std::move( other.m_holder.m_value ) );
                other.m_holder.reset();
            }
        }

        constexpr void reset () noexcept
        {
            static_cast< MixInWrapper * >( this )->m_holder.reset();
        }

        template < typename ... _Arguments >
        constexpr ::std::enable_if_t< ::std::is_constructible< _Value, _Arguments && ... >::value, MixInValue & >
        emplace ( _Arguments && ... arguments )
        {
            auto _this = static_cast< MixInWrapper * >( this );
            _this->m_holder.reset();
            _this->m_holder.construct( ::std::forward< _Arguments >( arguments ) ... );
            return _this->m_holder.m_value;
        }

        template< typename _Type, typename ... _Arguments >
        constexpr ::std::enable_if_t< ::std::is_constructible< _Value, _Arguments && ... >::value, MixInValue & >
        emplace ( std::initializer_list< _Type > list, _Arguments && ... arguments )
        {
            auto _this = static_cast< MixInWrapper * >( this );
            _this->m_holder.reset();
            _this->m_holder.construct( list, ::std::forward< _Arguments >( arguments ) ... );
            return _this->m_holder.m_value;
        }

        constexpr explicit operator bool() const noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            return static_cast< WrapperRefer >( *this ).m_holder.m_is_exists;
        }


        constexpr bool hasValue() const noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            return static_cast< WrapperRefer >( *this ).m_holder.m_is_exists;
        }
    };
}}

#undef SCL_OPTIONAL_VALUE
#undef SCL_OPTIONAL_VALUE_METHOD

#endif
