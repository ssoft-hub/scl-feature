#pragma once
#ifndef SCL_FEATURE_OPERATOR_H
#define SCL_FEATURE_OPERATOR_H

#include "binary_operator.h"
#include "unary_operator.h"

#include <scl/feature/access/holder_guard.h>
#include <scl/feature/access/value_pointer.h>
#include <scl/utility/preprocessor.h>

SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(&, AddressOf, address_of)

SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(*, Indirection, indirection)
SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION(->*, MemberIndirection, arrow_to_pointer)
SCL_BINARY_OPERATOR_IMPLEMENTAION(SCL_FORWARD(, ), Comma, comma)

SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION([], SquareBrackets, subscript)
SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION((), RoundBrackets, call)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(+, PrefixPlus, unary_plus)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(-, PrefixMinus, unary_minus)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(++, PrefixPlusPlus, prefix_increment)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(--, PrefixMinusMinus, prefix_decrement)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(~, PrefixBitwiseNot, bitwise_not)
SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(!, PrefixLogicalNot, logical_not)

SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION(++, PostfixPlusPlus, postfix_increment)
SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION(--, PostfixMinusMinus, postfix_decrement)

SCL_BINARY_OPERATOR_IMPLEMENTAION(==, Equal, equal_to)
SCL_BINARY_OPERATOR_IMPLEMENTAION(!=, NotEqual, not_equal_to)
SCL_BINARY_OPERATOR_IMPLEMENTAION(<, Less, less)
SCL_BINARY_OPERATOR_IMPLEMENTAION(<=, LessOrEqual, less_equal)
SCL_BINARY_OPERATOR_IMPLEMENTAION(>, Greater, greater)
SCL_BINARY_OPERATOR_IMPLEMENTAION(>=, GreaterOrEqual, greater_equal)

SCL_BINARY_OPERATOR_IMPLEMENTAION(*, Multiply, multiply)
SCL_BINARY_OPERATOR_IMPLEMENTAION(/, Divide, divide)
SCL_BINARY_OPERATOR_IMPLEMENTAION(%, Modulo, modulo)
SCL_BINARY_OPERATOR_IMPLEMENTAION(+, Addition, plus)
SCL_BINARY_OPERATOR_IMPLEMENTAION(-, Subtraction, minus)

SCL_BINARY_OPERATOR_IMPLEMENTAION(<<, LeftShift, left_shift)
SCL_BINARY_OPERATOR_IMPLEMENTAION(>>, RightShift, right_shift)

SCL_BINARY_OPERATOR_IMPLEMENTAION(&, BitwiseAnd, bitwise_and)
SCL_BINARY_OPERATOR_IMPLEMENTAION(|, BitwiseOr, bitwise_or)
SCL_BINARY_OPERATOR_IMPLEMENTAION(^, BitwiseXor, bitwise_xor)

SCL_BINARY_OPERATOR_IMPLEMENTAION(&&, LogicalAnd, logical_and)
SCL_BINARY_OPERATOR_IMPLEMENTAION(||, LogicalOr, logical_or)

SCL_BINARY_OPERATOR_IMPLEMENTAION(=, Assignment, assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(*=, MultiplyAssignment, multiply_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(/=, DivideAssignment, divide_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(%=, ModuloAssignment, modulo_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(+=, AdditionAssignment, plus_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(-=, SubtractionAssignment, minus_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(<<=, LeftShiftAssignment, left_shift_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(>>=, RightShiftAssignment, right_shift_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(&=, BitwiseAndAssignment, bitwise_and_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(|=, BitwiseOrAssignment, bitwise_or_assign)
SCL_BINARY_OPERATOR_IMPLEMENTAION(^=, BitwiseXorAssignment, bitwise_xor_assign)

#define SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION(Invokable)                                  \
    namespace ScL::Feature::Detail::Operator::Binary                                          \
    {                                                                                         \
        template <>                                                                           \
        struct Invokable##Operator<Global::Invokable##Case>                                   \
        {                                                                                     \
            template <typename _Left, typename _Right>                                        \
            static decltype(auto) invoke(_Left && left, _Right && right)                      \
            {                                                                                 \
                using LeftRefer = _Left &&;                                                   \
                using RightRefer = _Right &&;                                                 \
                return ::ScL::Feature::Detail::Operator::Binary::Invokable##Helper<LeftRefer, \
                    RightRefer>::invoke(::std::forward<LeftRefer>(left),                      \
                    ::std::forward<RightRefer>(right));                                       \
            }                                                                                 \
        };                                                                                    \
    }

#define SCL_GLOBAL_BINARY_OPERATOR(symbol, Invokable)                                             \
    SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION(Invokable)                                          \
    template <typename _Left,                                                                     \
        typename _Right,                                                                          \
        typename = ::std::enable_if_t<!::ScL::Feature::isWrapper< ::std::decay_t<_Left> >()       \
            && ::ScL::Feature::isWrapper< ::std::decay_t<_Right> >()                              \
            && ::ScL::Feature::Detail::Operator::Binary::does##Invokable##OperatorExist<_Left &&, \
                ::ScL::SimilarRefer<typename ::std::decay_t<_Right>::Value, _Right &&> >()> >     \
    constexpr decltype(auto) operator symbol(_Left && left, _Right && right)                      \
    {                                                                                             \
        using LeftRefer = _Left &&;                                                               \
        using RightRefer = _Right &&;                                                             \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable##Helper<LeftRefer,             \
            RightRefer>::invoke(::std::forward<LeftRefer>(left),                                  \
            ::std::forward<RightRefer>(right));                                                   \
    }

#define SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(symbol, this_refer, other_refer, Invokable)     \
    /*template < typename ... _Arguments,                                                    \ \
        typename = ::std::enable_if_t< sizeof...( _Arguments ) == 0                          \ \
            && ::ScL::Feature::Detail::Operator::Binary::does ## Invokable ## OperatorExist< \ \
       ThisType this_refer, ThisType other_refer >() > >*/                                     \
    constexpr decltype(auto) operator symbol(ThisType other_refer right) this_refer            \
    {                                                                                          \
        using LeftRefer = ThisType this_refer;                                                 \
        using RightRefer = ThisType other_refer;                                               \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable##Helper<LeftRefer,          \
            RightRefer>::invoke(::std::forward<LeftRefer>(*this),                              \
            ::std::forward<RightRefer>(right));                                                \
    }

#define SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(symbol, this_refer, Invokable)          \
    template <typename _Right,                                                        \
        typename = ::std::enable_if_t<                                                \
            ::ScL::Feature::Detail::Operator::Binary::does##Invokable##OperatorExist< \
                ThisType this_refer,                                                  \
                _Right &&>()> >                                                       \
    constexpr decltype(auto) operator symbol(_Right && right) this_refer              \
    {                                                                                 \
        using LeftRefer = ThisType this_refer;                                        \
        using RightRefer = _Right &&;                                                 \
        return ::ScL::Feature::Detail::Operator::Binary::Invokable##Helper<LeftRefer, \
            RightRefer>::invoke(::std::forward<LeftRefer>(*this),                     \
            ::std::forward<RightRefer>(right));                                       \
    }

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(symbol, this_refer, Invokable) \
    template <typename _Argument,                                                         \
        typename = ::std::enable_if_t<                                                    \
            ::ScL::Feature::Detail::Operator::Unary::does##Invokable##OperatorExist<      \
                Value this_refer,                                                         \
                _Argument &&>()> >                                                        \
    constexpr decltype(auto) operator symbol(_Argument && argument) this_refer            \
    {                                                                                     \
        using ThisRefer = ThisType this_refer;                                            \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable##Helper<ThisRefer,      \
            _Argument &&>::invoke(::std::forward<ThisRefer>(*this),                       \
            ::std::forward<_Argument &&>(argument));                                      \
    }

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(symbol, this_refer, Invokable) \
    template <typename... _Arguments,                                                      \
        typename = ::std::enable_if_t<                                                     \
            ::ScL::Feature::Detail::Operator::Unary::does##Invokable##OperatorExist<       \
                Value this_refer,                                                          \
                _Arguments &&...>()> >                                                     \
    constexpr decltype(auto) operator symbol(_Arguments &&... arguments) this_refer        \
    {                                                                                      \
        using ThisRefer = ThisType this_refer;                                             \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable##Helper<ThisRefer,       \
            _Arguments &&...>::invoke(::std::forward<ThisRefer>(*this),                    \
            ::std::forward<_Arguments &&>(arguments)...);                                  \
    }

#define SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(symbol, this_refer, Invokable)                    \
    template <typename... _Arguments,                                                         \
        typename = ::std::enable_if_t<sizeof...(_Arguments) == 0                              \
            && ::ScL::Feature::Detail::Operator::Unary::does##Invokable##OperatorExist<       \
                Value this_refer,                                                             \
                _Arguments &&...>()> >                                                        \
    constexpr decltype(auto) operator symbol() this_refer                                     \
    {                                                                                         \
        using ThisRefer = ThisType this_refer;                                                \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable##Helper<ThisRefer>::invoke( \
            ::std::forward<ThisRefer>(*this));                                                \
    }

#define SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(symbol, this_refer, Invokable)          \
    template <typename... _Arguments,                                                         \
        typename = ::std::enable_if_t<sizeof...(_Arguments) == 0                              \
            && ::ScL::Feature::Detail::Operator::Unary::does##Invokable##OperatorExist<       \
                Value this_refer,                                                             \
                _Arguments &&...>()> >                                                        \
    constexpr decltype(auto) operator symbol(int) this_refer                                  \
    {                                                                                         \
        using ThisRefer = ThisType this_refer;                                                \
        return ::ScL::Feature::Detail::Operator::Unary::Invokable##Helper<ThisRefer>::invoke( \
            ::std::forward<ThisRefer>(*this));                                                \
    }

#define SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(symbol, this_refer)                                     \
    template <typename... _Arguments, typename = ::std::enable_if_t<sizeof...(_Arguments) == 0> > \
    constexpr ::ScL::Feature::ValuePointer<ThisType this_refer> operator symbol() this_refer      \
    {                                                                                             \
        return ::ScL::Feature::ValuePointer<ThisType this_refer>(                                 \
            ::std::forward<ThisType this_refer>(*this));                                          \
    }

#define SCL_DEREFERENCE_OPERATOR_PROTOTYPE(symbol, this_refer)                                    \
    template <typename... _Arguments, typename = ::std::enable_if_t<sizeof...(_Arguments) == 0> > \
    constexpr ::ScL::Feature::ValuePointer<ThisType this_refer> operator symbol() this_refer      \
    {                                                                                             \
        return ::ScL::Feature::ValuePointer<ThisType this_refer>(                                 \
            ::std::forward<ThisType this_refer>(*this));                                          \
    }

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(other_refer)                             \
    constexpr Wrapper(ThisType other_refer other)                                           \
        : m_holder{::ScL::Feature::Detail::WrapperResolver<ThisType, ThisType other_refer>{ \
              ::std::forward<ThisType other_refer>(other)}                                  \
                  .resolve()}                                                               \
    {}

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE_V2(other_refer)                          \
    template <typename... _Arguments,                                                       \
        typename = ::std::enable_if_t<sizeof...(_Arguments) == 0                            \
            && ::std::is_constructible<ThisType, ThisType other_refer>::value> >            \
    constexpr Wrapper(ThisType other_refer other)                                           \
        : m_holder(::ScL::Feature::Detail::WrapperResolver<ThisType, ThisType other_refer>( \
              ::std::forward<ThisType other_refer>(other))                                  \
                  .resolve())                                                               \
    {}

#define SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(other_refer)                   \
    template <typename _OtherValue, typename _OtherTool>                           \
    constexpr Wrapper(Wrapper<_OtherValue, _OtherTool> other_refer other)          \
        : m_holder{::ScL::Feature::Detail::WrapperResolver<ThisType,               \
              Wrapper<_OtherValue, _OtherTool> other_refer>{                       \
              ::std::forward<Wrapper<_OtherValue, _OtherTool> other_refer>(other)} \
                  .resolve()}                                                      \
    {}

#define SCL_CONSTRUCTOR_FOR_THIS_WRAPPER                          \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(&&)                \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(const &&)          \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(volatile &&)       \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(const volatile &&) \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(&)                 \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(const &)           \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(volatile &)        \
    SCL_CONSTRUCTOR_FOR_THIS_WRAPPER_PROTOTYPE(const volatile &)

#define SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER                          \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(&&)                \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(const &&)          \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(volatile &&)       \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(const volatile &&) \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(&)                 \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(const &)           \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(volatile &)        \
    SCL_CONSTRUCTOR_FOR_OTHER_WRAPPER_PROTOTYPE(const volatile &)

#define SCL_ADDRESS_OF_OPERATOR(symbol)                                       \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &&)                \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &&)          \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &&)       \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &&) \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &)                 \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &)           \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &)        \
    SCL_ADDRESS_OF_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &)

#define SCL_DEREFERENCE_OPERATOR(symbol)                                       \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &&)                \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &&)          \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &&)       \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &&) \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &)                 \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &)           \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &)        \
    SCL_DEREFERENCE_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &)

#define SCL_PREFIX_UNARY_OPERATOR(symbol, Invokable)                                       \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &&, Invokable)                \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &&, Invokable)          \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &&, Invokable)       \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &&, Invokable) \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), &, Invokable)                 \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const &, Invokable)           \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), volatile &, Invokable)        \
    SCL_PREFIX_UNARY_OPERATOR_PROTOTYPE(SCL_FORWARD(symbol), const volatile &, Invokable)

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT(symbol, Invokable)                                \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(SCL_FORWARD(symbol), &&, Invokable)         \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(SCL_FORWARD(symbol), const &&, Invokable)   \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(                                            \
        SCL_FORWARD(symbol), volatile &&, Invokable)                                               \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(                                            \
        SCL_FORWARD(symbol), const volatile &&, Invokable)                                         \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(SCL_FORWARD(symbol), &, Invokable)          \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(SCL_FORWARD(symbol), const &, Invokable)    \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(SCL_FORWARD(symbol), volatile &, Invokable) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENT(                                            \
        SCL_FORWARD(symbol), const volatile &, Invokable)

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS(symbol, Invokable)                              \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(SCL_FORWARD(symbol), &&, Invokable)       \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(SCL_FORWARD(symbol), const &&, Invokable) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(                                          \
        SCL_FORWARD(symbol), volatile &&, Invokable)                                              \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(                                          \
        SCL_FORWARD(symbol), const volatile &&, Invokable)                                        \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(SCL_FORWARD(symbol), &, Invokable)        \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(SCL_FORWARD(symbol), const &, Invokable)  \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(                                          \
        SCL_FORWARD(symbol), volatile &, Invokable)                                               \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_ARGUMENTS(                                          \
        SCL_FORWARD(symbol), const volatile &, Invokable)

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_INT(symbol, Invokable)                                 \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), &&, Invokable)          \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), const &&, Invokable)    \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), volatile &&, Invokable) \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(                                             \
        SCL_FORWARD(symbol), const volatile &&, Invokable)                                     \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), &, Invokable)           \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), const &, Invokable)     \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), volatile &, Invokable)  \
    SCL_POSTFIX_UNARY_OPERATOR_PROTOTYPE_WITH_INT(SCL_FORWARD(symbol), const volatile &, Invokable)

#define SCL_BINARY_OPERATOR_FOR_ANY(symbol, Invokable)                                       \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), &&, Invokable)                \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), const &&, Invokable)          \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), volatile &&, Invokable)       \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), &, Invokable)                 \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), const &, Invokable)           \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), volatile &, Invokable)        \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_ANY(SCL_FORWARD(symbol), const volatile &, Invokable)

/*
 * NOTE: Закомментирована реализация из-за проблем со сборкой ::std::map< Wrapper, Value >
 * Для оператора присвоения '=' константный экземпляр слева является экзотикой,
 * поэтому такое решение может быть хоть как-то оправдано.
 */
// clang-format off
#define SCL_ASSIGNMENT_OPERATOR_FOR_THIS(symbol, Invokable)                                        \
    /*                                                                                             \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, &&, Invokable)              \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, const &&, Invokable)        \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, volatile &&, Invokable)     \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, &, Invokable)               \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, const &, Invokable)         \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &&, volatile &, Invokable)      \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS( SCL_FORWARD(symbol), &&, const volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, &&, Invokable)        \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, const &&, Invokable)  \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, const volatile &&, Invokable)\
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, &, Invokable)         \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, const &, Invokable)   \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &&, const volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, &&, Invokable)     \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, const &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, &, Invokable)      \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, const &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &&, const volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, const &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, const &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &&, const volatile &, Invokable) \                                                                                                 \
    */                                                                                             \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, &&, Invokable)               \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, const &&, Invokable)         \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, volatile &&, Invokable)      \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, &, Invokable)                \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, const &, Invokable)          \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, volatile &, Invokable)       \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), &, const volatile &, Invokable) \
    /*                                                                                             \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, &&, Invokable)         \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, const &&, Invokable)   \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, &, Invokable)          \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, const &, Invokable)    \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const &, const volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, &&, Invokable)      \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, const &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, &, Invokable)       \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, const &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), volatile &, const volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, const &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, const volatile &&, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, const &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, volatile &, Invokable) \
    SCL_BINARY_OPERATOR_PROTOTYPE_FOR_THIS(SCL_FORWARD(symbol), const volatile &, const volatile &, Invokable) \
*/
// clang-format on

#undef SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION
#undef SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION
#undef SCL_BINARY_OPERATOR_IMPLEMENTAION

#endif
