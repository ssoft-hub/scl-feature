#pragma once
#ifndef SCL_FEATURE_OPERATOR_H
#define SCL_FEATURE_OPERATOR_H

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Feature/Access/ValuePointer.h>
#include <ScL/Utility/SingleArgument.h>
#include "BinaryOperator.h"
#include "UnaryOperator.h"

SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( & , AddressOf )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( * , Indirection )
SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION( ->*, MemberIndirection )
SCL_BINARY_OPERATOR_IMPLEMENTAION( SCL_SINGLE_ARG( , ), Comma )

SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION( [], SquareBrackets )
SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION( (), RoundBrackets )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( +, PrefixPlus )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( -, PrefixMinus )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( ++, PrefixPlusPlus )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( --, PrefixMinusMinus )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( ~, PrefixBitwiseNot )
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( !, PrefixLogicalNot )

SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION( ++, PostfixPlusPlus )
SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION( --, PostfixMinusMinus )

SCL_BINARY_OPERATOR_IMPLEMENTAION( ==, Equal )
SCL_BINARY_OPERATOR_IMPLEMENTAION( !=, NotEqual )
SCL_BINARY_OPERATOR_IMPLEMENTAION( <, Less )
SCL_BINARY_OPERATOR_IMPLEMENTAION( <=, LessOrEqual )
SCL_BINARY_OPERATOR_IMPLEMENTAION( >, Greater )
SCL_BINARY_OPERATOR_IMPLEMENTAION( >=, GreaterOrEqual )

SCL_BINARY_OPERATOR_IMPLEMENTAION( *, Multiply )
SCL_BINARY_OPERATOR_IMPLEMENTAION( /, Divide )
SCL_BINARY_OPERATOR_IMPLEMENTAION( %, Modulo )
SCL_BINARY_OPERATOR_IMPLEMENTAION( +, Addition )
SCL_BINARY_OPERATOR_IMPLEMENTAION( -, Subtraction )

SCL_BINARY_OPERATOR_IMPLEMENTAION( <<, LeftShift )
SCL_BINARY_OPERATOR_IMPLEMENTAION( >>, RightShift )

SCL_BINARY_OPERATOR_IMPLEMENTAION( &, BitwiseAnd )
SCL_BINARY_OPERATOR_IMPLEMENTAION( |, BitwiseOr )
SCL_BINARY_OPERATOR_IMPLEMENTAION( ^, BitwiseXor )

SCL_BINARY_OPERATOR_IMPLEMENTAION( &&, LogicalAnd )
SCL_BINARY_OPERATOR_IMPLEMENTAION( ||, LogicalOr )

SCL_BINARY_OPERATOR_IMPLEMENTAION( =, Assignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( *=, MultiplyAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( /=, DivideAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( %=, ModuloAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( +=, AdditionAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( -=, SubtractionAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( <<=, LeftShiftAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( >>=, RightShiftAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( &=, BitwiseAndAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( |=, BitwiseOrAssignment )
SCL_BINARY_OPERATOR_IMPLEMENTAION( ^=, BitwiseXorAssignment )

#define SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( symbol, right_refer, Invokable ) \
    template < typename _Left, typename _RightValue, typename _RightTool, \
        typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >() \
            && ::ScL::Feature::Detail::Operator::Binary::does ## Invokable ## OperatorExist< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > right_refer >() > > \
    constexpr decltype(auto) operator symbol ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > right_refer right ) \
    { \
        using LeftRefer = _Left &&; \
        using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > right_refer; \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Helper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
    } \

#define SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION( Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            template <> \
            struct Invokable ## Operator< Global::Invokable ## Case > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftRefer = _Left &&; \
                    using RightRefer = _Right &&; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Helper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
             \
        }} \
    }}}

#define SCL_GLOBAL_BINARY_OPERATOR( symbol, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION( Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

#define SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( symbol, this_refer, other_refer, Invokable ) \
    /*template < typename ... _Arguments, \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0 \
            && ::ScL::Feature::Detail::Operator::Binary::does ## Invokable ## OperatorExist< ThisType this_refer, ThisType other_refer >() > >*/ \
    constexpr decltype(auto) operator symbol ( ThisType other_refer right ) this_refer \
    { \
        using LeftRefer = ThisType this_refer; \
        using RightRefer = ThisType other_refer; \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Helper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( *this ), ::std::forward< RightRefer >( right ) ); \
    } \

#define SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( symbol, this_refer, Invokable ) \
    template < typename _Right, \
        typename = ::std::enable_if_t< ::ScL::Feature::Detail::Operator::Binary::does ## Invokable ## OperatorExist< ThisType this_refer, _Right && >() > > \
    constexpr decltype(auto) operator symbol ( _Right && right ) this_refer \
    { \
        using LeftRefer = ThisType this_refer; \
        using RightRefer = _Right &&; \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Helper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( *this ), ::std::forward< RightRefer >( right ) ); \
    } \

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( symbol, this_refer, Invokable ) \
    template < typename _Argument, \
        typename = ::std::enable_if_t< ::ScL::Feature::Detail::Operator::Unary::does ## Invokable ## OperatorExist< Value this_refer, _Argument && >() > > \
    constexpr decltype(auto) operator symbol ( _Argument && argument ) this_refer \
    { \
        using ThisRefer = ThisType this_refer; \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## Helper< ThisRefer, _Argument && >::invoke( ::std::forward< ThisRefer >( *this ), ::std::forward< _Argument && >( argument ) ); \
    } \

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( symbol, this_refer, Invokable ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< ::ScL::Feature::Detail::Operator::Unary::does ## Invokable ## OperatorExist< Value this_refer, _Arguments && ... >() > > \
    constexpr decltype(auto) operator symbol ( _Arguments && ... arguments ) this_refer \
    { \
        using ThisRefer = ThisType this_refer; \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## Helper< ThisRefer, _Arguments && ... >::invoke( ::std::forward< ThisRefer >( *this ), ::std::forward< _Arguments && >( arguments ) ... ); \
    } \

#define SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( symbol, this_refer, Invokable ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0 \
            && ::ScL::Feature::Detail::Operator::Unary::does ## Invokable ## OperatorExist< Value this_refer, _Arguments && ... >() > > \
    constexpr decltype(auto) operator symbol () this_refer \
    { \
        using ThisRefer = ThisType this_refer; \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## Helper< ThisRefer >::invoke( ::std::forward< ThisRefer >( *this ) ); \
    } \

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( symbol, this_refer, Invokable ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0 \
            && ::ScL::Feature::Detail::Operator::Unary::does ## Invokable ## OperatorExist< Value this_refer, _Arguments && ... >() > > \
    constexpr decltype(auto) operator symbol ( int ) this_refer \
    { \
        using ThisRefer = ThisType this_refer; \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## Helper< ThisRefer >::invoke( ::std::forward< ThisRefer >( *this ) ); \
    } \

#define SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( symbol, this_refer ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0 > > \
    constexpr ::ScL::Feature::ValuePointer< ThisType this_refer > operator symbol () this_refer \
    { \
        return ::ScL::Feature::ValuePointer< ThisType this_refer >( ::std::forward< ThisType this_refer >( *this ) ); \
    } \

#define SCL_DEREFERENCE_OPERATOR_PROTOTYPE( symbol, this_refer ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0  > > \
    constexpr ::ScL::Feature::ValuePointer< ThisType this_refer > operator symbol () this_refer \
    { \
        return ::ScL::Feature::ValuePointer< ThisType this_refer >( ::std::forward< ThisType this_refer >( *this ) ); \
    } \

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( other_refer ) \
    constexpr Wrapper ( ThisType other_refer other ) \
        : m_holder( ::ScL::Feature::Detail::WrapperResolver< ThisType, ThisType other_refer >( \
            ::std::forward< ThisType other_refer >( other ) ).resolve() ) \
    { \
    } \

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE_V2( other_refer ) \
    template < typename ... _Arguments, \
        typename = ::std::enable_if_t< ::std::is_constructible< ThisType, ThisType other_refer >{} && \
            sizeof...( _Arguments ) == 0 > > \
    constexpr Wrapper ( ThisType other_refer other ) \
        : m_holder( ::ScL::Feature::Detail::WrapperResolver< ThisType, ThisType other_refer >( \
            ::std::forward< ThisType other_refer >( other ) ).resolve() ) \
    { \
    } \

#define SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( other_refer ) \
    template < typename _OtherValue, typename _OtherTool > \
    constexpr Wrapper ( Wrapper< _OtherValue, _OtherTool > other_refer other ) \
        : m_holder( ::ScL::Feature::Detail::WrapperResolver< ThisType, Wrapper< _OtherValue, _OtherTool > other_refer >( \
            ::std::forward< Wrapper< _OtherValue, _OtherTool > other_refer >( other ) ).resolve() ) \
    { \
    } \

#define SCL_CAST_OPERATOR_PROTOTYPE( refer ) \
    /*TODO:*/

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( && ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( const && ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( volatile && ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( const volatile && ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( & ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( const & ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( volatile & ) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE( const volatile & ) \

#define SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( && ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( const && ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( volatile && ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( const volatile && ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( & ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( const & ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( volatile & ) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE( const volatile & ) \

#define SCL_CAST_OPERATOR \
    SCL_CAST_OPERATOR_PROTOTYPE( && ) \
    SCL_CAST_OPERATOR_PROTOTYPE( const && ) \
    SCL_CAST_OPERATOR_PROTOTYPE( volatile && ) \
    SCL_CAST_OPERATOR_PROTOTYPE( const volatile && ) \
    SCL_CAST_OPERATOR_PROTOTYPE( & ) \
    SCL_CAST_OPERATOR_PROTOTYPE( const & ) \
    SCL_CAST_OPERATOR_PROTOTYPE( volatile & ) \
    SCL_CAST_OPERATOR_PROTOTYPE( const volatile & ) \

#define SCL_ADDRESS_OF_OPERATOR( symbol ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

#define SCL_DEREFERENCE_OPERATOR( symbol ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), && ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const && ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile && ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile && ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), & ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const & ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile & ) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile & ) \

#define SCL_PREFIX_UNARY_OPERATOR( symbol, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( symbol, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS( symbol, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_INT( symbol, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

#define SCL_BINARY_OPERATOR_FOR_ANY( symbol, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY( SCL_SINGLE_ARG( symbol ), const volatile &, Invokable ) \

/*
 * NOTE: Закомментирована реализация из-за проблем со сборкой ::std::map< Wrapper, Value >
 * Для оператора присвоения '=' константный экземпляр слева является экзотикой,
 * поэтому такое решение может быть хоть как-то оправдано.
 */
#define SCL_ASSIGNMENT_OPERATOR_FOR_THIS( symbol, Invokable ) \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &&, const volatile &, Invokable )*/ \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &&, const volatile &, Invokable )*/ \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &&, const volatile &, Invokable )*/ \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &&, const volatile &, Invokable )*/ \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), &, const volatile &, Invokable ) \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const &, const volatile &, Invokable )*/ \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), volatile &, const volatile &, Invokable )*/ \
    /*BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const volatile &&, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, volatile &, Invokable ) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_SINGLE_ARG( symbol ), const volatile &, const volatile &, Invokable )*/ \

#undef SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION
#undef SCL_BINARY_OPERATOR_IMPLEMENTAION

#endif
