#pragma once
#ifndef SCL_WRAPPER_TOOL_INPLACE_DEBUG_H
#define SCL_WRAPPER_TOOL_INPLACE_DEBUG_H

#include <utility>
#include <iostream>
#include <ScL/Utility/SimilarRefer.h>
#include <ScL/Utility/SingleArgument.h>

#if defined( __GNUC__ )
#   define SCL_DEBUG_FUNC_INFO __PRETTY_FUNCTION__
#elif defined( _MSC_VER )
#   define SCL_DEBUG_FUNC_INFO __FUNCSIG__
#else
#   define SCL_DEBUG_FUNC_INFO  ""
#endif

#define SCL_DEBUG_PREFIX_UNARY_OPERATOR( symbol, Invokable ) \
/*
    template < typename _WrapperRefer > \
    static decltype(auto) operator ## Invokable ( _WrapperRefer && wrapper ) \
    { \
        using WrapperRefer = _WrapperRefer &&; \
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >; \
        return symbol ::std::forward< ValueRefer && >( ::ScL::Feature::Detail::wrapperHolder( ::std::forward< WrapperRefer >( wrapper ) ).m_value ); \
    } \
*/

#define SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( symbol, Invokable ) \
/*
    template < typename _WrapperRefer, typename _Argument > \
    static decltype(auto) operator ## Invokable ( _WrapperRefer && Wrapper, _Argument && argument ) \
    { \
        using WrapperRefer = _WrapperRefer &&; \
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >; \
        TODO: ::std::forward< ValueRefer && >( Wrapper.m_value ).operator symbol ( ::std::forward< _Argument && >( argument ) ); \
    } \
*/

#define SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS( symbol, Invokable ) \
/*
    template < typename _WrapperRefer, typename ... _Arguments > \
    static decltype(auto) operator ## Invokable ( _WrapperRefer && Wrapper, _Arguments && ... arguments ) \
    { \
        using WrapperRefer = _WrapperRefer &&; \
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >; \
        TODO: ::std::forward< ValueRefer && >( Wrapper.m_value ).operator symbol ( ::std::forward< _Arguments && >( arguments ) ... ); \
    } \
*/

#define SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_INT( symbol, Invokable ) \
/*
    template < typename _WrapperRefer > \
    static decltype(auto) operator ## Invokable ( _WrapperRefer && Wrapper ) \
    { \
        using WrapperRefer = _WrapperRefer &&; \
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >; \
        TODO: ::std::forward< ValueRefer && >( Wrapper.m_value ) symbol; \
    } \
*/

#define SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( symbol, Invokable ) \
/*
    template < typename _LeftWrapperRefer, typename _RightWrapperRefer > \
    static decltype(auto) operator ## Invokable ( _LeftWrapperRefer && left, _RightWrapperRefer && right ) \
    { \
        using LeftWrapper = ::std::decay_t< _LeftWrapperRefer >;\
        using RightWrapper = ::std::decay_t< _RightWrapperRefer >;\
        TODO: LeftWrapper::value( ::std::forward< _LeftWrapperRefer && >( left ) ) symbol RightWrapper::Value( ::std::forward< _RightWrapperRefer && >( right ) ); \
    } \
*/

namespace ScL { namespace Feature { namespace Inplace
{
    /*!
     * Инструмент для формирования значения "по месту". Определение "по месту" означает,
     * что для значения не используется специальное размещение в куче и оно является
     * неотъемлемой частью пространства имен, в котором это значение определено.
     * Отличается от ::ScL::Feature::Inplace::Default наличием отладочного вывода в ::std::cout.
     */
    struct Debug
    {
        static void outText ( const char * text ) { ::std::cout << text << ::std::endl; }

        template < typename _Value >
        struct Holder
        {
            using ThisType = Holder< _Value >;
            using Value = _Value;

            Value m_value;

            template < typename ... _Arguments >
            constexpr Holder ( _Arguments && ... arguments )
                : m_value( ::std::forward< _Arguments >( arguments ) ... )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( ThisType && other )
                : m_value( ::std::forward< Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( const ThisType && other )
                : m_value( ::std::forward< const Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( volatile ThisType && other )
                : m_value( ::std::forward< volatile Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( const volatile ThisType && other )
                : m_value( ::std::forward< const volatile Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( ThisType & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( const ThisType & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( volatile ThisType & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            constexpr Holder ( const volatile ThisType & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > && other )
                : m_value( ::std::forward< typename Holder< _OtherValue >::Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > && other )
                : m_value( ::std::forward< typename Holder< const _OtherValue >::Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > && other )
                : m_value( ::std::forward< typename Holder< volatile _OtherValue >::Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > && other )
                : m_value( ::std::forward< typename Holder< const volatile _OtherValue >::Value >( other.m_value ) )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( Holder< _OtherValue > & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( const Holder< _OtherValue > & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( volatile Holder< _OtherValue > & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _OtherValue >
            constexpr Holder ( const volatile Holder< _OtherValue > & other )
                : m_value( other.m_value )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _HolderRefer >
            static constexpr void guard ( _HolderRefer && )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            template < typename _HolderRefer >
            static constexpr void unguard ( _HolderRefer && )
            {
                outText( SCL_DEBUG_FUNC_INFO );
            }

            /*!
             * Access to internal value of Holder for any king of referencies.
             */
            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                outText( SCL_DEBUG_FUNC_INFO );

                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer< Value, HolderRefer >;
                return ::std::forward< ValueRefer >( holder.m_value );
            }

            /* All kind of assignment operators */
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( =, Assignment )

            SCL_DEBUG_PREFIX_UNARY_OPERATOR( &, AddressOf )
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( *, Indirection )
            SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( ->*, MemberIndirection )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( SCL_SINGLE_ARG( , ), Comma )
            /* Subscript */
            SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( [], SquareBrackets )

            /* Functional forms */
            SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS( (), RoundBrackets )
            /* Arithmetic operators */
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( +, PrefixPlus )
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( -, PrefixMinus )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( *, Multiply )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( /, Divide )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( %, Modulo )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( +, Addition )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( -, Subtraction )
            /* Compound assignment */
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( *=, MultiplyAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( /=, DivideAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( %=, ModuloAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( +=, AdditionAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( -=, SubtractionAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( <<=, ShiftLeftAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( >>=, ShiftRightAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( &=, BitwiseAndAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( ^=, BitwiseXorAssignment )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( |=, BitwiseOrAssignment )
            /* Increment and decrement */
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( ++, PrefixPlusPlus )
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( --, PrefixMinusMinus )
            SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_INT( ++, PostfixPlusPlus )
            SCL_DEBUG_POSTFIX_UNARY_OPERATOR_WITH_INT( --, PostfixMinusMinus )
            /* Relational and comparison operators */
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( ==, IsEqual )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( !=, NotEqual )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( <, Less )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( <=, LessOrEqual )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( >, Greater )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( >=, GreaterOrEqual )
            /* Logical operators */
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( !, PrefixLogicalNot )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( &&, LogicalAnd )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( ||, LogicalOr )
            /* Bitwise operators */
            SCL_DEBUG_PREFIX_UNARY_OPERATOR( ~, PrefixBitwiseNot )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( &, BitwiseAnd )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( ^, BitwiseXor )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( |, BitwiseOr )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( <<, ShiftLeft )
            SCL_DEBUG_BINARY_OPERATOR_FOR_ANY( >>, ShiftRight )
        };
    };
}}}

#endif
