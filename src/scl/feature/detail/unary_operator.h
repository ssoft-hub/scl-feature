#pragma once
#ifndef SCL_UNARY_OPERATOR_H
#define SCL_UNARY_OPERATOR_H

#include "result_switch.h"

#include <scl/utility/preprocessor.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::detail::Operator::Unary
{
    /* Cases for Holder method existing */
    struct HolderHasOperatorCase;
    struct HolderHasNoOperatorCase;
} // namespace scl::feature::detail::Operator::Unary

// clang-format off
#define SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable, scl_name)                                  \
    template <typename _Kind, typename _Refer, typename... _Arguments>                             \
    struct Does##Invokable##OperatorExistHelper;                                                   \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    using Does##Invokable##OperatorExist = Does##Invokable##OperatorExistHelper<                   \
        ::scl::feature::detail::Operator::WrapperSwitchCase<_Refer>,                               \
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
    struct Does##Invokable##OperatorExistHelper<::scl::feature::detail::Operator::NoneWrapperCase, \
        _Refer,                                                                                    \
        _Arguments...>                                                                             \
    {                                                                                              \
        static_assert(::std::is_reference<_Refer>::value,                                          \
            "The template parameter _Refer must to be a reference type.");                         \
        static constexpr bool                                                                      \
            value = ::scl::is_detected_v< ::scl::scl_name##_operation,                             \
                _Refer,                                                                            \
                _Arguments...>;                                                                    \
        constexpr operator bool() const noexcept { return value; }                                 \
    };                                                                                             \
                                                                                                   \
    template <typename _Refer, typename... _Arguments>                                             \
    struct Does##Invokable##OperatorExistHelper<::scl::feature::detail::Operator::LeftWrapperCase, \
        _Refer,                                                                                    \
        _Arguments...>                                                                             \
    {                                                                                              \
        static_assert(::std::is_reference<_Refer>::value,                                          \
            "The template parameter _Refer must to be a reference type.");                         \
                                                                                                   \
        using WrapperRefer = _Refer;                                                               \
        using wrapper = ::std::decay_t<WrapperRefer>;                                              \
        using Holder = typename wrapper::Holder;                                                   \
        using Value = typename wrapper::Value;                                                     \
        using ValueRefer = ::scl::SimilarRefer<Value, WrapperRefer>;                               \
                                                                                                   \
        static constexpr bool                                                                      \
            value = ::scl::is_detected_v< ::scl::scl_name##_operation,                             \
                        ValueRefer,                                                                \
                        _Arguments...>                                                             \
            || ::scl::is_detected_v<::scl::feature::detail::Operator::Unary::                      \
                                        operator##Invokable##_static_method_exact_operation,       \
                Holder,                                                                            \
                WrapperRefer,                                                                      \
                _Arguments...>;                                                                    \
        constexpr operator bool() const noexcept { return value; }                                 \
    };

#define SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION(Invokable)                                         \
    namespace scl::feature::detail::Operator::Unary                                                \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##Switch;                                                                  \
        template <>                                                                                \
        struct Invokable##Switch<::scl::feature::detail::Operator::Unary::HolderHasOperatorCase>   \
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
        struct Invokable##Switch<::scl::feature::detail::Operator::Unary::HolderHasNoOperatorCase> \
        {                                                                                          \
            template <typename _Wrapper, typename... _Arguments>                                   \
            static decltype(auto) invoke(_Wrapper && wrapper, _Arguments &&... arguments)          \
            {                                                                                      \
                using WrapperRefer = _Wrapper &&;                                                  \
                using ValueRefer = ::scl::SimilarRefer<                                            \
                    typename ::std::decay_t<WrapperRefer>::Value,                                  \
                    WrapperRefer>;                                                                 \
                using Returned = ::std::invoke_result_t<                                           \
                    ::scl::feature::detail::Operator::Unary::Invokable,                            \
                    ValueRefer,                                                                    \
                    _Arguments &&...>;                                                             \
                                                                                                   \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::LeftWrapperCase,                             \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        ValueRefer>>::invoke(::scl::feature::detail::Operator::Unary::Invokable{}, \
                    ::std::forward<WrapperRefer>(wrapper),                                         \
                    ::std::forward<_Arguments>(arguments)...);                                     \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Unary                                                \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##WrapperSwitch;                                                           \
        template <>                                                                                \
        struct Invokable##WrapperSwitch<::scl::feature::detail::Operator::LeftWrapperCase>         \
        {                                                                                          \
            template <typename _WrapperRefer, typename... _Arguments>                              \
            static constexpr decltype(auto) invoke(                                                \
                _WrapperRefer && value, _Arguments &&... arguments)                                \
            {                                                                                      \
                using WrapperRefer = _WrapperRefer &&;                                             \
                using wrapper = ::std::decay_t<WrapperRefer>;                                      \
                using Holder = typename wrapper::Holder;                                           \
                using HolderRefer = ::scl::SimilarRefer<Holder, WrapperRefer>;                     \
                                                                                                   \
                constexpr bool holder_has_method_for_operator = ::scl::feature::detail::Operator:: \
                    Unary::doesOperator##Invokable##StaticMethodExist<Holder,                      \
                        HolderRefer,                                                               \
                        _Arguments &&...>();                                                       \
                using OperatorSwitchCase = ::std::conditional_t<holder_has_method_for_operator,    \
                    ::scl::feature::detail::Operator::Unary::HolderHasOperatorCase,                \
                    ::scl::feature::detail::Operator::Unary::HolderHasNoOperatorCase>;             \
                return ::scl::feature::detail::Operator::Unary::Invokable##Switch<                 \
                    OperatorSwitchCase>::invoke(::std::forward<WrapperRefer>(value),               \
                    ::std::forward<_Arguments>(arguments)...);                                     \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Unary                                                \
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
                return ::scl::feature::detail::Operator::Unary::Invokable##WrapperSwitch<          \
                    ::scl::feature::detail::Operator::WrapperSwitchCase<                           \
                        Refer>>::invoke(::std::forward<Refer>(value),                              \
                    ::std::forward<_Arguments &&>(arguments)...);                                  \
            }                                                                                      \
        };                                                                                         \
    }

#define SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION(symbol, Invokable, scl_name)                \
    namespace scl ::feature::detail::Operator::Unary                                         \
    {                                                                                        \
        SCL_METHOD_DETECTION(operator##Invokable)                                            \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::scl::is_detected_v<operator##Invokable##_static_method_exact_operation, \
                _Arguments...>;                                                              \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable, scl_name)                           \
    }                                                                                        \
                                                                                             \
    namespace scl::feature::detail::Operator::Unary                                          \
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

#define SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION(symbol, Invokable, scl_name)                 \
    namespace scl::feature::detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_METHOD_DETECTION(operator##Invokable)                                            \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::scl::is_detected_v<operator##Invokable##_static_method_exact_operation, \
                _Arguments...>;                                                              \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable, scl_name)                           \
    }                                                                                        \
                                                                                             \
    namespace scl::feature::detail::Operator::Unary                                          \
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

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION(symbol, Invokable, scl_name)  \
    namespace scl::feature::detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_METHOD_DETECTION(operator##Invokable)                                            \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::scl::is_detected_v<operator##Invokable##_static_method_exact_operation, \
                _Arguments...>;                                                              \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable, scl_name)                           \
    }                                                                                        \
                                                                                             \
    namespace scl::feature::detail::Operator::Unary                                          \
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

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION(symbol, Invokable, scl_name) \
    namespace scl::feature::detail::Operator::Unary                                          \
    {                                                                                        \
        SCL_METHOD_DETECTION(operator##Invokable)                                            \
        template <typename... _Arguments>                                                    \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()            \
        {                                                                                    \
            return ::scl::is_detected_v<operator##Invokable##_static_method_exact_operation, \
                _Arguments...>;                                                              \
        }                                                                                    \
        SCL_FEATURE_DOES_UNARY_OPERATOR_EXIST(Invokable, scl_name)                           \
    }                                                                                        \
                                                                                             \
    namespace scl::feature::detail::Operator::Unary                                          \
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
