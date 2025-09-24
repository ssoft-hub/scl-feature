#pragma once
#ifndef SCL_UNARY_OPERATOR_H
#define SCL_UNARY_OPERATOR_H

#include "ResultSwitch.h"

#include <ScL/Meta/Trait/Detection/Method.h>
#include <ScL/Meta/Trait/Detection/Operator.h>
#include <ScL/Utility/SimilarRefer.h>
#include <ScL/Utility/SingleArgument.h>

namespace ScL::Feature::Detail::Operator::Unary
{
    /* Cases for Holder method existing */
    struct HolderHasOperatorCase;
    struct HolderHasNoOperatorCase;
} // namespace ScL::Feature::Detail::Operator::Unary

// clang-format off
#define SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable)                                           \
    template <typename _Kind, typename _Refer, typename... _Arguments>                             \
    struct Does##Invokable##OperatorExistHelper;                                                   \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    using Does##Invokable##OperatorExist = Does##Invokable##OperatorExistHelper<                   \
        ::ScL::Feature::Detail::Operator::WrapperSwitchCase<_Refer>,                               \
        _Refer,                                                                                    \
        _Arguments...>;                                                                            \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    inline constexpr bool does##Invokable##OperatorExist()                                         \
    {                                                                                              \
        return Does##Invokable##OperatorExist<_Refer, _Arguments...>::value;                       \
    }                                                                                              \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    struct Does##Invokable##OperatorExistHelper<::ScL::Feature::Detail::Operator::NoneWrapperCase, \
        _Refer,                                                                                    \
        _Arguments...>                                                                             \
    {                                                                                              \
        static_assert(::std::is_reference<_Refer>::value,                                          \
            "The template parameter _Refer must to be a reference type.");                         \
        static constexpr bool                                                                      \
            value = ::ScL::Meta::isDetected<::ScL::Meta::Invokable##UnstrictOperation,             \
                _Refer,                                                                            \
                _Arguments...>();                                                                  \
        constexpr operator bool() const noexcept { return value; }                                 \
    };                                                                                             \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    struct Does##Invokable##OperatorExistHelper<::ScL::Feature::Detail::Operator::LeftWrapperCase, \
        _Refer,                                                                                    \
        _Arguments...>                                                                             \
    {                                                                                              \
        static_assert(::std::is_reference<_Refer>::value,                                          \
            "The template parameter _Refer must to be a reference type.");                         \
                                                                                                   \
        using WrapperRefer = _Refer;                                                               \
        using Wrapper = ::std::decay_t<WrapperRefer>;                                              \
        using Holder = typename Wrapper::Holder;                                                   \
        using Value = typename Wrapper::Value;                                                     \
        using ValueRefer = ::ScL::SimilarRefer<Value, WrapperRefer>;                               \
                                                                                                   \
        static constexpr bool                                                                      \
            value = ::ScL::Meta::isDetected<::ScL::Meta::Invokable##UnstrictOperation,             \
                        ValueRefer,                                                                \
                        _Arguments...>()                                                           \
            || ::ScL::Meta::isDetected<::ScL::Feature::Detail::Operator::Unary::                   \
                                           Operator##Invokable##StaticMethodStrictOperation,       \
                Holder,                                                                            \
                WrapperRefer,                                                                      \
                _Arguments...>();                                                                  \
        constexpr operator bool() const noexcept { return value; }                                 \
    };

#define SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)                                         \
    namespace ScL::Feature::Detail::Operator::Unary                                                \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##Switch;                                                                  \
        template <>                                                                                \
        struct Invokable##Switch<::ScL::Feature::Detail::Operator::Unary::HolderHasOperatorCase>   \
        {                                                                                          \
            template <typename _Wrapper, typename... _Arguments>                                   \
            static decltype(auto) invoke(_Wrapper && wrapper, _Arguments &&... arguments)          \
            {                                                                                      \
                using WrapperRefer = _Wrapper &&;                                                  \
                using Holder = typename ::std::decay_t<WrapperRefer>::Holder;                      \
                return Holder::operator##Invokable(::std::forward<WrapperRefer>(wrapper),          \
                    ::std::forward<_Arguments>(arguments)...);                                     \
            }                                                                                      \
        };                                                                                         \
        template <>                                                                                \
        struct Invokable##Switch<::ScL::Feature::Detail::Operator::Unary::HolderHasNoOperatorCase> \
        {                                                                                          \
            template <typename _Wrapper, typename... _Arguments>                                   \
            static decltype(auto) invoke(_Wrapper && wrapper, _Arguments &&... arguments)          \
            {                                                                                      \
                using WrapperRefer = _Wrapper &&;                                                  \
                using ValueRefer = ::ScL::SimilarRefer<                                            \
                    typename ::std::decay_t<WrapperRefer>::Value,                                  \
                    WrapperRefer>;                                                                 \
                using Returned = ::std::invoke_result_t<                                           \
                    ::ScL::Feature::Detail::Operator::Unary::Invokable,                            \
                    ValueRefer,                                                                    \
                    _Arguments &&...>;                                                             \
                                                                                                   \
                return ::ScL::Feature::Detail::Operator::ResultSwitch<                             \
                    ::ScL::Feature::Detail::Operator::LeftWrapperCase,                             \
                    ::ScL::Feature::Detail::Operator::ResultSwitchCase<Returned,                   \
                        ValueRefer>>::invoke(::ScL::Feature::Detail::Operator::Unary::Invokable{}, \
                    ::std::forward<WrapperRefer>(wrapper),                                         \
                    ::std::forward<_Arguments>(arguments)...);                                     \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace ScL::Feature::Detail::Operator::Unary                                                \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##WrapperSwitch;                                                           \
        template <>                                                                                \
        struct Invokable##WrapperSwitch<::ScL::Feature::Detail::Operator::LeftWrapperCase>         \
        {                                                                                          \
            template <typename _WrapperRefer, typename... _Arguments>                              \
            static constexpr decltype(auto) invoke(                                                \
                _WrapperRefer && value, _Arguments &&... arguments)                                \
            {                                                                                      \
                using WrapperRefer = _WrapperRefer &&;                                             \
                using Wrapper = ::std::decay_t<WrapperRefer>;                                      \
                using Holder = typename Wrapper::Holder;                                           \
                using HolderRefer = ::ScL::SimilarRefer<Holder, WrapperRefer>;                     \
                                                                                                   \
                constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator:: \
                    Unary::doesOperator##Invokable##StaticMethodExist<Holder,                      \
                        HolderRefer,                                                               \
                        _Arguments &&...>();                                                       \
                using OperatorSwitchCase = ::std::conditional_t<holder_has_method_for_operator,    \
                    ::ScL::Feature::Detail::Operator::Unary::HolderHasOperatorCase,                \
                    ::ScL::Feature::Detail::Operator::Unary::HolderHasNoOperatorCase>;             \
                return ::ScL::Feature::Detail::Operator::Unary::Invokable##Switch<                 \
                    OperatorSwitchCase>::invoke(::std::forward<WrapperRefer>(value),               \
                    ::std::forward<_Arguments>(arguments)...);                                     \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace ScL::Feature::Detail::Operator::Unary                                                \
    {                                                                                              \
        template <typename _Refer, typename... _Arguments>                                         \
        struct Invokable##Helper                                                                   \
        {                                                                                          \
            static_assert(::std::is_reference<_Refer>::value,                                      \
                "The template parameter _Refer must to be a "                                      \
                "reference type.");                                                                \
            using Refer = _Refer;                                                                  \
            static constexpr decltype(auto) invoke(Refer value, _Arguments &&... arguments)        \
            {                                                                                      \
                return ::ScL::Feature::Detail::Operator::Unary::Invokable##WrapperSwitch<          \
                    ::ScL::Feature::Detail::Operator::WrapperSwitchCase<                           \
                        Refer>>::invoke(::std::forward<Refer>(value),                              \
                    ::std::forward<_Arguments &&>(arguments)...);                                  \
            }                                                                                      \
        };                                                                                         \
    }

#define SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION(symbol, Invokable)                          \
    namespace ScL ::Feature::Detail::Operator::Unary                                         \
    {                                                                                        \
        SCL_META_METHOD_DETECTION(operator##Invokable, Operator##Invokable)                  \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::ScL::Meta::isDetected<Operator##Invokable##StaticMethodStrictOperation, \
                _Arguments...>();                                                            \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable)                                     \
    }                                                                                        \
                                                                                             \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        struct Invokable                                                                     \
        {                                                                                    \
            template <typename _Value>                                                       \
            decltype(auto) operator()(_Value && value)                                       \
            {                                                                                \
                return ::std::forward<_Value &&>(value) symbol;                              \
            }                                                                                \
        };                                                                                   \
    }                                                                                        \
                                                                                             \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)

#define SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(symbol, Invokable)                           \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_META_METHOD_DETECTION(operator##Invokable, Operator##Invokable)                  \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::ScL::Meta::isDetected<Operator##Invokable##StaticMethodStrictOperation, \
                _Arguments...>();                                                            \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable)                                     \
    }                                                                                        \
                                                                                             \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        struct Invokable                                                                     \
        {                                                                                    \
            template <typename _Value>                                                       \
            decltype(auto) operator()(_Value && value)                                       \
            {                                                                                \
                return symbol ::std::forward<_Value>(value);                                 \
            }                                                                                \
        };                                                                                   \
    }                                                                                        \
                                                                                             \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION(symbol, Invokable)            \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_META_METHOD_DETECTION(operator##Invokable, Operator##Invokable)                  \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::ScL::Meta::isDetected<Operator##Invokable##StaticMethodStrictOperation, \
                _Arguments...>();                                                            \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable)                                     \
    }                                                                                        \
                                                                                             \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        struct Invokable                                                                     \
        {                                                                                    \
            template <typename _Value, typename _Argument>                                   \
            decltype(auto) operator()(_Value && value, _Argument && argument)                \
            {                                                                                \
                return ::std::forward<_Value>(value).operator symbol(                        \
                    ::std::forward<_Argument>(argument));                                    \
            }                                                                                \
        };                                                                                   \
    }                                                                                        \
                                                                                             \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION(symbol, Invokable)           \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_META_METHOD_DETECTION(operator##Invokable, Operator##Invokable)                  \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::ScL::Meta::isDetected<Operator##Invokable##StaticMethodStrictOperation, \
                _Arguments...>();                                                            \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable)                                     \
    }                                                                                        \
                                                                                             \
    namespace ScL::Feature::Detail::Operator::Unary                                          \
    {                                                                                        \
        struct Invokable                                                                     \
        {                                                                                    \
            template <typename _Value, typename... _Arguments>                               \
            decltype(auto) operator()(_Value && value, _Arguments &&... arguments)           \
            {                                                                                \
                return ::std::forward<_Value>(value).operator symbol(                        \
                    ::std::forward<_Arguments>(arguments)...);                               \
            }                                                                                \
        };                                                                                   \
    }                                                                                        \
                                                                                             \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)
// clang-format on

#endif
