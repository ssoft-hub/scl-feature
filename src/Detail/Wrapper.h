#pragma once
#ifndef SCL_FEATURE_DETAIL_WRAPPER_H
#define SCL_FEATURE_DETAIL_WRAPPER_H

#ifdef _MSC_VER
#pragma warning( disable : 4521 4522 )
#endif

#include <ScL/Feature/MixIn.h>
#include "Operator.h"
#include "Range.h"
#include "Resolver.h"

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Класс для формирования экземпляра значения _Value, наделенными дополнительными
     * свойствами. Дополнительные свойства реализуются посредством функциональности _Tool.
     */
    template < typename _Value, typename _Tool >
    class Wrapper final
        : public ::ScL::Feature::MixIn< Wrapper< _Value, _Tool > >
    {
        static_assert( !::std::is_reference< _Tool >::value,
            "The template parameter _Tool must to be not a reference type." );

        template < typename >
        friend class ::ScL::Feature::MixIn;

        template < typename >
        friend struct ::ScL::Feature::Detail::WrapperAccess;

        using ThisType = ::ScL::Feature::Detail::Wrapper< _Value, _Tool >;

    public:
        using Value = _Value;
        using Data = Value;     // convenient alias
        using Tool = _Tool;
        using Holder = typename Tool:: template Holder< _Value >;

    private:
        Holder m_holder;

    public:
        /// Конструктор инициализации значения по заданным параметрам
        template < typename ... _Arguments,
            typename = ::std::enable_if_t< ::std::is_constructible< Holder, _Arguments && ... >::value > >
        constexpr Wrapper ( _Arguments && ... arguments )
        noexcept( ::std::is_nothrow_constructible< Holder, _Arguments && ... >() )
            : m_holder( ::std::forward< _Arguments >( arguments ) ... )
        {}

        template < typename _Type, typename ... _Arguments,
            typename = ::std::enable_if_t< ::std::is_constructible< Holder, ::std::initializer_list< _Type >, _Arguments && ... >::value > >
        constexpr Wrapper ( ::std::initializer_list< _Type > list, _Arguments && ... arguments )
        noexcept( ::std::is_nothrow_constructible< Holder, ::std::initializer_list< _Type >, _Arguments && ... >() )
            : m_holder( list, ::std::forward< _Arguments >( arguments ) ... )
        {}

        /* All kind of constructors for ThisType */
        SCL_CONSTRUCTOR_FOR_THIS_WRAPPER
        /* All kind of constructors for Wrapper< _OtherValue, _OtherTool > */
        SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER

        /* All kind of assignment operators for any type (including Wrapper< _OtherValue, _Other >) */
        SCL_BINARY_OPERATOR_FOR_ANY( =, Assignment )

        /* All kind of assignment operators for ThisType */
        SCL_ASSIGNMENT_OPERATOR_FOR_THIS( =, Assignment )

        /* Member access */
        // NOTE: Используется доступ через оператор "->", но семантически необходим ".".
        // TODO: Заменить на оператор "." с внедрением P0416R1(N4477) или P0352 в стандарт C++,
        // а для оператора "->" использовать реализацию подобно другим.
        SCL_DEREFERENCE_OPERATOR( -> )
        SCL_ADDRESS_OF_OPERATOR( & )
        // NOTE: SCL_PREFIX_UNARY_OPERATOR( &, AddressOf ) restricted
        SCL_PREFIX_UNARY_OPERATOR( *, Indirection )
        SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( ->*, MemberIndirection )
        SCL_BINARY_OPERATOR_FOR_ANY( SCL_SINGLE_ARG( , ), Comma )
        /* Subscript */
        SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT( [], SquareBrackets )

        /* Functional forms */
        SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS( (), RoundBrackets )
        /* Arithmetic operators */
        SCL_PREFIX_UNARY_OPERATOR( +, PrefixPlus )
        SCL_PREFIX_UNARY_OPERATOR( -, PrefixMinus )
        SCL_BINARY_OPERATOR_FOR_ANY( *, Multiply )
        SCL_BINARY_OPERATOR_FOR_ANY( /, Divide )
        SCL_BINARY_OPERATOR_FOR_ANY( %, Modulo )
        SCL_BINARY_OPERATOR_FOR_ANY( +, Addition )
        SCL_BINARY_OPERATOR_FOR_ANY( -, Subtraction )
        /* Compound assignment */
        SCL_BINARY_OPERATOR_FOR_ANY( *=, MultiplyAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( /=, DivideAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( %=, ModuloAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( +=, AdditionAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( -=, SubtractionAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( <<=, LeftShiftAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( >>=, RightShiftAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( &=, BitwiseAndAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( ^=, BitwiseXorAssignment )
        SCL_BINARY_OPERATOR_FOR_ANY( |=, BitwiseOrAssignment )
        /* Increment and decrement */
        SCL_PREFIX_UNARY_OPERATOR( ++, PrefixPlusPlus )
        SCL_PREFIX_UNARY_OPERATOR( --, PrefixMinusMinus )
        SCL_POSTFIX_UNARY_OPERATOR_WITH_INT( ++, PostfixPlusPlus )
        SCL_POSTFIX_UNARY_OPERATOR_WITH_INT( --, PostfixMinusMinus )
        /* Relational and comparison operators */
        SCL_BINARY_OPERATOR_FOR_ANY( ==, Equal )
        SCL_BINARY_OPERATOR_FOR_ANY( !=, NotEqual )
        SCL_BINARY_OPERATOR_FOR_ANY( <, Less )
        SCL_BINARY_OPERATOR_FOR_ANY( <=, LessOrEqual )
        SCL_BINARY_OPERATOR_FOR_ANY( >, Greater )
        SCL_BINARY_OPERATOR_FOR_ANY( >=, GreaterOrEqual )
        /* Logical operators */
        SCL_PREFIX_UNARY_OPERATOR( !, PrefixLogicalNot )
        SCL_BINARY_OPERATOR_FOR_ANY( &&, LogicalAnd )
        SCL_BINARY_OPERATOR_FOR_ANY( ||, LogicalOr )
        /* Bitwise operators */
        SCL_PREFIX_UNARY_OPERATOR( ~, PrefixBitwiseNot )
        SCL_BINARY_OPERATOR_FOR_ANY( &, BitwiseAnd )
        SCL_BINARY_OPERATOR_FOR_ANY( ^, BitwiseXor )
        SCL_BINARY_OPERATOR_FOR_ANY( |, BitwiseOr )
        SCL_BINARY_OPERATOR_FOR_ANY( <<, LeftShift )
        SCL_BINARY_OPERATOR_FOR_ANY( >>, RightShift )

        // NOTE: These overloads are used to implement output I/O manipulators such as std::endl.
        template < typename ... _Arguments,
            typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0 && ::ScL::Meta::isDetected< ::ScL::Meta::LeftShiftMemberStrictOperation, Value, Value & (*)( Value & ) >() > >
        decltype(auto) operator << ( Value & (*right)( Value & ) )
        {
            using Function = Value & (*)( Value & );
            using LeftRefer = ThisType &;
            using RightRefer = Function &&;
            return ::ScL::Feature::Detail::Operator::Binary::LeftShiftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( *this ), ::std::forward< RightRefer >( right ) );
        }

        /* Type-casting */
        SCL_CAST_OPERATOR
    };
}}}

/* RIGHT-SIDE WRAPPER OPERATORS */

/* Arithmetic operators */
SCL_GLOBAL_BINARY_OPERATOR( *, Multiply )
SCL_GLOBAL_BINARY_OPERATOR( /, Divide )
SCL_GLOBAL_BINARY_OPERATOR( %, Modulo )
SCL_GLOBAL_BINARY_OPERATOR( +, Addition )
SCL_GLOBAL_BINARY_OPERATOR( -, Subtraction )
/* Compound assignment */
SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION( Assignment ) // NOTE: The 'operator =' cannot be implemented as global method.
SCL_GLOBAL_BINARY_OPERATOR( *=, MultiplyAssignment )
SCL_GLOBAL_BINARY_OPERATOR( /=, DivideAssignment )
SCL_GLOBAL_BINARY_OPERATOR( %=, ModuloAssignment )
SCL_GLOBAL_BINARY_OPERATOR( +=, AdditionAssignment )
SCL_GLOBAL_BINARY_OPERATOR( -=, SubtractionAssignment )
SCL_GLOBAL_BINARY_OPERATOR( <<=, LeftShiftAssignment )
SCL_GLOBAL_BINARY_OPERATOR( >>=, RightShiftAssignment )
SCL_GLOBAL_BINARY_OPERATOR( &=, BitwiseAndAssignment )
SCL_GLOBAL_BINARY_OPERATOR( ^=, BitwiseXorAssignment )
SCL_GLOBAL_BINARY_OPERATOR( |=, BitwiseOrAssignment )
/* Relational and comparison operators */
SCL_GLOBAL_BINARY_OPERATOR( ==, Equal )
SCL_GLOBAL_BINARY_OPERATOR( !=, NotEqual )
SCL_GLOBAL_BINARY_OPERATOR( <, Less )
SCL_GLOBAL_BINARY_OPERATOR( <=, LessOrEqual )
SCL_GLOBAL_BINARY_OPERATOR( >, Greater )
SCL_GLOBAL_BINARY_OPERATOR( >=, GreaterOrEqual )
/* Logical operators */
SCL_GLOBAL_BINARY_OPERATOR( &&, LogicalAnd )
SCL_GLOBAL_BINARY_OPERATOR( ||, LogicalOr )
/* Bitwise operators */
SCL_GLOBAL_BINARY_OPERATOR( &, BitwiseAnd )
SCL_GLOBAL_BINARY_OPERATOR( ^, BitwiseXor )
SCL_GLOBAL_BINARY_OPERATOR( |, BitwiseOr )
SCL_GLOBAL_BINARY_OPERATOR( <<, LeftShift )
SCL_GLOBAL_BINARY_OPERATOR( >>, RightShift )

/* HASH */
namespace std
{
    template< typename _Type, typename _Tool >
    struct hash< ::ScL::Feature::Detail::Wrapper< _Type, _Tool > >
    {
        using Key = ::ScL::Feature::Detail::Wrapper< _Type, _Tool >;
        ::std::size_t operator()( Key const & key ) const noexcept
        {
            using Type = ::std::remove_reference_t< decltype( *&::std::declval< Key >() ) >;
            return ::std::hash< Type >{}( *&key );
        }
    };
}

namespace ScL { namespace Feature
{
    template < typename _Value, typename _Tool >
    class MixIn< ::ScL::Feature::Detail::Wrapper< _Value, _Tool > > {};
}}

#endif
