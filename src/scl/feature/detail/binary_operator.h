#pragma once
#ifndef SCL_FEATURE_OPERATOR_BINARY_H
#define SCL_FEATURE_OPERATOR_BINARY_H

#include "result_switch.h"

#include <scl/utility/preprocessor.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::detail::Operator::Binary
{
    /* Cases for Holder method existing */
    struct HolderHasOperatorCase
    {};
    struct HolderHasNoOperatorCase
    {};
} // namespace scl::feature::detail::Operator::Binary

namespace scl::feature::detail::Operator::Global
{
    struct DefaultCase;
} // namespace scl::feature::detail::Operator::Global

// clang-format off
#define SCL_FEATURE_DOES_BINARY_OPERATOR_EXIST(Invokable, scl_name)                             \
    template <typename _Kind, typename _LeftRefer, typename _RightRefer>                       \
    struct Does##Invokable##OperatorExistHelper;                                               \
                                                                                               \
    template <typename _LeftRefer, typename _RightRefer>                                       \
    using Does##Invokable##OperatorExist = Does##Invokable##OperatorExistHelper<               \
        ::scl::feature::detail::Operator::WrapperSwitchCase<_LeftRefer, _RightRefer>,          \
        _LeftRefer,                                                                            \
        _RightRefer>;                                                                          \
                                                                                               \
    template <typename _LeftRefer, typename _RightRefer>                                       \
    inline constexpr bool does##Invokable##OperatorExist()                                     \
    {                                                                                          \
        return Does##Invokable##OperatorExist<_LeftRefer, _RightRefer>{};                      \
    }                                                                                          \
                                                                                               \
    /* Non wrapper case */                                                                     \
    template <typename _LeftRefer, typename _RightRefer>                                       \
    struct Does##Invokable##OperatorExistHelper<                                               \
        ::scl::feature::detail::Operator::NoneWrapperCase,                                     \
            _LeftRefer,                                                                        \
            _RightRefer>                                                                       \
    {                                                                                          \
        static_assert(::std::is_reference<_LeftRefer>{},                                       \
            "The template parameter _LeftRefer must to be a reference type.");                 \
        static_assert(::std::is_reference<_RightRefer>{},                                      \
            "The template parameter _RightRefer must to be a reference type.");                \
                                                                                               \
        static constexpr bool                                                                  \
            value = ::scl::is_detected_v< ::scl::scl_name##_operation,                        \
                _LeftRefer,                                                                    \
                _RightRefer>;                                                                  \
        constexpr operator bool() const noexcept { return value; }                             \
    };                                                                                         \
                                                                                               \
    /* Member of wrapper case */                                                               \
    template <typename _LeftWrapperRefer, typename _RightRefer>                                \
    struct Does##Invokable##OperatorExistHelper<                                               \
            ::scl::feature::detail::Operator::LeftWrapperCase,                                 \
            _LeftWrapperRefer,                                                                 \
            _RightRefer>                                                                       \
    {                                                                                          \
        static_assert(::std::is_reference<_LeftWrapperRefer>{},                                \
            "The template parameter _LeftWrapperRefer must to be a reference type.");          \
        static_assert(::std::is_reference<_RightRefer>{} || ::std::is_function<_RightRefer>{}, \
            "The template parameter _RightRefer must to be a reference type.");                \
                                                                                               \
        using LeftWrapperRefer = _LeftWrapperRefer;                                            \
        using LeftWrapper = ::std::decay_t<LeftWrapperRefer>;                                  \
        using LeftHolder = typename LeftWrapper::Holder;                                       \
        using LeftValue = typename LeftWrapper::Value;                                         \
        using LeftValueRefer = ::scl::SimilarRefer<LeftValue, LeftWrapperRefer>;               \
        using RightRefer = _RightRefer;                                                        \
                                                                                               \
        static constexpr bool                                                                  \
            value = ::scl::is_detected_v< ::scl::scl_name##_operation,                        \
                        LeftValueRefer,                                                        \
                        RightRefer>                                                            \
            || ::scl::is_detected_v<                                                           \
                operator##Invokable##ForLeftWrapper_static_method_exact_operation,             \
                LeftHolder,                                                                    \
                LeftWrapperRefer,                                                              \
                RightRefer>;                                                                   \
        constexpr operator bool() const noexcept { return value; }                             \
    };                                                                                         \
                                                                                               \
    /* Global case */                                                                          \
    template <typename _LeftRefer, typename _RightWrapperRefer>                                \
    struct Does##Invokable##OperatorExistHelper<                                               \
            ::scl::feature::detail::Operator::RightWrapperCase,                                \
            _LeftRefer,                                                                        \
            _RightWrapperRefer>                                                                \
    {                                                                                          \
        static_assert(::std::is_reference<_LeftRefer>{},                                       \
            "The template parameter _LeftRefer must to be a reference type.");                 \
        static_assert(::std::is_reference<_RightWrapperRefer>{},                               \
            "The template parameter _RightWrapperRefer must to be a reference type.");         \
                                                                                               \
        using LeftRefer = _LeftRefer;                                                          \
        using RightWrapperRefer = _RightWrapperRefer;                                          \
        using RightWrapper = ::std::decay_t<RightWrapperRefer>;                                \
        using RightHolder = typename RightWrapper::Holder;                                     \
        using RightValue = typename RightWrapper::Value;                                       \
        using RightValueRefer = ::scl::SimilarRefer<RightValue, RightWrapperRefer>;            \
                                                                                               \
        static constexpr bool                                                                  \
            value = ::scl::is_detected_v< ::scl::scl_name##_operation,                        \
                        LeftRefer,                                                             \
                        RightValueRefer>                                                       \
            || ::scl::is_detected_v<                                                           \
                operator##Invokable##ForRightWrapper_static_method_exact_operation,            \
                RightHolder,                                                                   \
                LeftRefer,                                                                     \
                RightWrapperRefer>;                                                            \
        constexpr operator bool() const noexcept { return value; }                             \
    };                                                                                         \
                                                                                               \
    /* Member of wrapper case */                                                               \
    template <typename _LeftWrapperRefer, typename _RightWrapperRefer>                         \
    struct Does##Invokable##OperatorExistHelper<                                               \
            ::scl::feature::detail::Operator::BothWrapperCase,                                 \
            _LeftWrapperRefer,                                                                 \
            _RightWrapperRefer>                                                                \
    {                                                                                          \
        static_assert(::std::is_reference<_LeftWrapperRefer>{},                                \
            "The template parameter _LeftWrapperRefer must to be a reference type.");          \
        static_assert(::std::is_reference<_RightWrapperRefer>{},                               \
            "The template parameter _RightWrapperRefer must to be a reference type.");         \
                                                                                               \
        using LeftWrapperRefer = _LeftWrapperRefer;                                            \
        using LeftWrapper = ::std::decay_t<LeftWrapperRefer>;                                  \
        using LeftHolder = typename LeftWrapper::Holder;                                       \
        using LeftValue = typename LeftWrapper::Value;                                         \
        using LeftValueRefer = ::scl::SimilarRefer<LeftValue, LeftWrapperRefer>;               \
        using RightWrapperRefer = _RightWrapperRefer;                                          \
        using RightWrapper = ::std::decay_t<RightWrapperRefer>;                                \
        using RightValue = typename RightWrapper::Value;                                       \
        using RightValueRefer = ::scl::SimilarRefer<RightValue, RightWrapperRefer>;            \
                                                                                               \
        static constexpr bool                                                                  \
            is_right_compatible_with_left = ::scl::feature::isThisCompatibleWithOther<         \
                                                RightWrapper,                                  \
                                                LeftWrapper>()                                 \
            && (::scl::is_detected_v<operator##Invokable##_static_method_exact_operation,       \
                    LeftHolder,                                                                \
                    LeftWrapperRefer,                                                          \
                    RightWrapperRefer>                                                         \
                || does##Invokable##OperatorExist<LeftValueRefer, RightValueRefer>());         \
                                                                                               \
        static constexpr bool is_path_of_right_compatible_with_left =                          \
            ::scl::feature::isPartOfThisCompatibleWithOther<RightWrapper, LeftWrapper>()       \
            && does##Invokable##OperatorExist<LeftWrapperRefer, RightValueRefer>();            \
                                                                                               \
        static constexpr bool is_right_compatible_with_path_of_left =                          \
            ::scl::feature::isThisCompatibleWithPartOfOther<RightWrapper, LeftWrapper>()       \
            && does##Invokable##OperatorExist<LeftValueRefer, RightWrapperRefer>();            \
                                                                                               \
        static constexpr bool                                                                  \
            is_right_not_compatible_with_left = !::scl::feature::isThisCompatibleWithOther<    \
                                                    RightWrapper,                              \
                                                    LeftWrapper>()                             \
            && does##Invokable##OperatorExist<LeftValueRefer, RightValueRefer>();              \
                                                                                               \
        static constexpr bool value = is_right_compatible_with_left                            \
            || is_path_of_right_compatible_with_left || is_right_compatible_with_path_of_left  \
            || is_right_not_compatible_with_left;                                              \
        constexpr operator bool() const noexcept { return value; }                             \
    };

#define SCL_BINARY_OPERATOR_IMPLEMENTAION(symbol, Invokable, scl_name)                            \
    namespace scl::feature::detail::Operator::Binary                                       \
    {                                                                                              \
        SCL_METHOD_DETECTION(operator##Invokable)                                                  \
        template <typename... _Arguments>                                                          \
        inline static constexpr bool doesOperator##Invokable##StaticMethodExist()                  \
        {                                                                                          \
            return ::scl::is_detected_v<operator##Invokable##_static_method_exact_operation,       \
                _Arguments...>;                                                                    \
        }                                                                                          \
        SCL_METHOD_DETECTION(operator##Invokable##ForLeftWrapper)                                  \
        template <typename... _Arguments>                                                          \
        inline static constexpr bool doesOperator##Invokable##ForLeftWrapper##StaticMethodExist()  \
        {                                                                                          \
            return ::scl::is_detected_v<                                                           \
                operator##Invokable##ForLeftWrapper_static_method_exact_operation,                 \
                _Arguments...>;                                                                    \
        }                                                                                          \
        SCL_METHOD_DETECTION(operator##Invokable##ForRightWrapper)                                 \
        template <typename... _Arguments>                                                          \
        inline static constexpr bool doesOperator##Invokable##ForRightWrapper##StaticMethodExist() \
        {                                                                                          \
            return ::scl::is_detected_v<                                                           \
                operator##Invokable##ForRightWrapper_static_method_exact_operation,                \
                _Arguments...>;                                                                    \
        }                                                                                          \
        SCL_FEATURE_DOES_BINARY_OPERATOR_EXIST(Invokable, scl_name)                                \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Global                                               \
    {                                                                                              \
        struct Invokable##Case;                                                                    \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Binary                                               \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##Operator;                                                                \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Operator< ::scl::feature::detail::Operator::Global::DefaultCase>         \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftRefer = _Left &&;                                                        \
                using RightRefer = _Right &&;                                                      \
                return ::std::forward<LeftRefer>(left) symbol ::std::forward<RightRefer>(right);   \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        struct Invokable                                                                           \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            decltype(auto) operator()(_Left && left, _Right && right)                              \
            {                                                                                      \
                using LeftRefer = _Left &&;                                                        \
                using RightRefer = _Right &&;                                                      \
                using Invokable##OperatorSwitchCase = ::std::conditional_t<                        \
                    ::scl::feature::isWrapper< ::std::decay_t<RightRefer> >(),                     \
                    ::scl::feature::detail::Operator::Global::Invokable##Case,                     \
                    ::scl::feature::detail::Operator::Global::DefaultCase>;                        \
                return ::scl::feature::detail::Operator::Binary::Invokable##Operator<              \
                    Invokable##OperatorSwitchCase>::invoke(::std::forward<LeftRefer>(left),        \
                    ::std::forward<RightRefer>(right));                                            \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Binary                                               \
    {                                                                                              \
        template <typename, typename>                                                              \
        struct Invokable##Switch;                                                                  \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::LeftWrapperCase,               \
            ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase>                       \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftHolder = typename ::std::decay_t<LeftWrapperRefer>::Holder;              \
                using RightRefer = _Right &&;                                                      \
                return LeftHolder::operator##Invokable##ForLeftWrapper(                            \
                    ::std::forward<LeftWrapperRefer>(left), ::std::forward<RightRefer>(right));    \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::LeftWrapperCase,               \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftValueRefer = ::scl::SimilarRefer<                                        \
                    typename ::std::decay_t<LeftWrapperRefer>::Value,                              \
                    LeftWrapperRefer>;                                                             \
                using RightRefer = _Right &&;                                                      \
                using Invokable = ::scl::feature::detail::Operator::Binary::Invokable;             \
                                                                                                   \
                using Returned = ::std::invoke_result_t<Invokable, LeftValueRefer, RightRefer>;    \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::LeftWrapperCase,                             \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        LeftValueRefer> >::                                                        \
                    invoke(::scl::feature::detail::Operator::Binary::Invokable(),                  \
                        ::std::forward<LeftWrapperRefer>(left),                                    \
                        ::std::forward<RightRefer>(right));                                        \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::RightWrapperCase,              \
            ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase>                       \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftRefer = _Left &&;                                                        \
                using RightWrapperRefer = _Right &&;                                               \
                using RightHolder = typename ::std::decay_t<RightWrapperRefer>::Holder;            \
                return RightHolder::operator##Invokable##ForRightWrapper(                          \
                    ::std::forward<LeftRefer>(left), ::std::forward<RightWrapperRefer>(right));    \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::RightWrapperCase,              \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftRefer = _Left &&;                                                        \
                using RightWrapperRefer = _Right &&;                                               \
                using RightValueRefer = ::scl::SimilarRefer<                                       \
                    typename ::std::decay_t<RightWrapperRefer>::Value,                             \
                    RightWrapperRefer>;                                                            \
                using Invokable = ::scl::feature::detail::Operator::Binary::Invokable;             \
                                                                                                   \
                using Returned = ::std::invoke_result_t<Invokable, LeftRefer, RightValueRefer>;    \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::RightWrapperCase,                            \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        RightValueRefer> >::                                                       \
                    invoke(::scl::feature::detail::Operator::Binary::Invokable(),                  \
                        ::std::forward<LeftRefer>(left),                                           \
                        ::std::forward<RightWrapperRefer>(right));                                 \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::BothWrapperCase,               \
            ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase>                       \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftHolder = typename ::std::decay_t<LeftWrapperRefer>::Holder;              \
                using RightWrapperRefer = _Right &&;                                               \
                return LeftHolder::operator##Invokable(::std::forward<LeftWrapperRefer>(left),     \
                    ::std::forward<RightWrapperRefer>(right));                                     \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::BothWrapperCase,               \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using RightWrapperRefer = _Right &&;                                               \
                return ::scl::feature::detail::Operator::Binary::Invokable##Switch<                \
                    ::scl::feature::detail::Operator::ExposingSwitchCase<LeftWrapperRefer,         \
                        RightWrapperRefer>,                                                        \
                    ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>::           \
                    invoke(::std::forward<LeftWrapperRefer>(left),                                 \
                        ::std::forward<RightWrapperRefer>(right));                                 \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::BothExposingCase,              \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftValueRefer = ::scl::SimilarRefer<                                        \
                    typename ::std::decay_t<LeftWrapperRefer>::Value,                              \
                    LeftWrapperRefer>;                                                             \
                using RightWrapperRefer = _Right &&;                                               \
                using RightValueRefer = ::scl::SimilarRefer<                                       \
                    typename ::std::decay_t<RightWrapperRefer>::Value,                             \
                    RightWrapperRefer>;                                                            \
                using Invokable = ::scl::feature::detail::Operator::Binary::Invokable;             \
                                                                                                   \
                using Returned = ::std::invoke_result_t<Invokable,                                 \
                    LeftValueRefer,                                                                \
                    RightValueRefer>;                                                              \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::BothExposingCase,                            \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        LeftValueRefer> >::                                                        \
                    invoke(::scl::feature::detail::Operator::Binary::Invokable(),                  \
                        ::std::forward<LeftWrapperRefer>(left),                                    \
                        ::std::forward<RightWrapperRefer>(right));                                 \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::LeftExposingCase,              \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftValueRefer = ::scl::SimilarRefer<                                        \
                    typename ::std::decay_t<LeftWrapperRefer>::Value,                              \
                    LeftWrapperRefer>;                                                             \
                using RightWrapperRefer = _Right &&;                                               \
                using Invokable = ::scl::feature::detail::Operator::Binary::Invokable;             \
                                                                                                   \
                using Returned = ::std::invoke_result_t<Invokable,                                 \
                    LeftValueRefer,                                                                \
                    RightWrapperRefer>;                                                            \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::LeftExposingCase,                            \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        LeftValueRefer> >::                                                        \
                    invoke(::scl::feature::detail::Operator::Binary::Invokable(),                  \
                        ::std::forward<LeftWrapperRefer>(left),                                    \
                        ::std::forward<RightWrapperRefer>(right));                                 \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##Switch< ::scl::feature::detail::Operator::RightExposingCase,             \
            ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>                     \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static decltype(auto) invoke(_Left && left, _Right && right)                           \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using RightWrapperRefer = _Right &&;                                               \
                using RightValueRefer = ::scl::SimilarRefer<                                       \
                    typename ::std::decay_t<RightWrapperRefer>::Value,                             \
                    RightWrapperRefer>;                                                            \
                using Invokable = ::scl::feature::detail::Operator::Binary::Invokable;             \
                                                                                                   \
                using Returned = ::std::invoke_result_t<Invokable,                                 \
                    LeftWrapperRefer,                                                              \
                    RightValueRefer>;                                                              \
                return ::scl::feature::detail::Operator::ResultSwitch<                             \
                    ::scl::feature::detail::Operator::RightExposingCase,                           \
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,                   \
                        LeftWrapperRefer> >::                                                      \
                    invoke(::scl::feature::detail::Operator::Binary::Invokable(),                  \
                        ::std::forward<LeftWrapperRefer>(left),                                    \
                        ::std::forward<RightWrapperRefer>(right));                                 \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
                                                                                                   \
    namespace scl::feature::detail::Operator::Binary                                               \
    {                                                                                              \
        template <typename>                                                                        \
        struct Invokable##WrapperSwitch;                                                           \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##WrapperSwitch< ::scl::feature::detail::Operator::LeftWrapperCase>        \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static constexpr decltype(auto) invoke(_Left && left, _Right && right)                 \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftWrapper = ::std::decay_t<LeftWrapperRefer>;                              \
                using LeftHolder = typename LeftWrapper::Holder;                                   \
                using RightRefer = _Right &&;                                                      \
                                                                                                   \
                constexpr bool holder_has_method_for_operator = ::scl::feature::detail::Operator:: \
                    Binary::doesOperator##Invokable##ForLeftWrapperStaticMethodExist<LeftHolder,   \
                        LeftWrapperRefer,                                                          \
                        RightRefer>();                                                             \
                using OperatorSwitchCase = ::std::conditional_t<holder_has_method_for_operator,    \
                    ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase,               \
                    ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>;            \
                return ::scl::feature::detail::Operator::Binary::Invokable##Switch<                \
                    LeftWrapperCase,                                                               \
                    OperatorSwitchCase>::invoke(::std::forward<LeftWrapperRefer>(left),            \
                    ::std::forward<RightRefer>(right));                                            \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##WrapperSwitch< ::scl::feature::detail::Operator::RightWrapperCase>       \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static constexpr decltype(auto) invoke(_Left && left, _Right && right)                 \
            {                                                                                      \
                using RightWrapperRefer = _Right &&;                                               \
                using RightWrapper = ::std::decay_t<RightWrapperRefer>;                            \
                using RightHolder = typename RightWrapper::Holder;                                 \
                using LeftRefer = _Left &&;                                                        \
                                                                                                   \
                constexpr bool holder_has_method_for_operator = ::scl::feature::detail::Operator:: \
                    Binary::doesOperator##Invokable##ForRightWrapperStaticMethodExist<RightHolder, \
                        LeftRefer,                                                                 \
                        RightWrapperRefer>();                                                      \
                using OperatorSwitchCase = ::std::conditional_t<holder_has_method_for_operator,    \
                    ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase,               \
                    ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>;            \
                return ::scl::feature::detail::Operator::Binary::Invokable##Switch<                \
                    RightWrapperCase,                                                              \
                    OperatorSwitchCase>::invoke(::std::forward<LeftRefer>(left),                   \
                    ::std::forward<RightWrapperRefer>(right));                                     \
            }                                                                                      \
        };                                                                                         \
                                                                                                   \
        template <>                                                                                \
        struct Invokable##WrapperSwitch< ::scl::feature::detail::Operator::BothWrapperCase>        \
        {                                                                                          \
            template <typename _Left, typename _Right>                                             \
            static constexpr decltype(auto) invoke(_Left && left, _Right && right)                 \
            {                                                                                      \
                using LeftWrapperRefer = _Left &&;                                                 \
                using LeftWrapper = ::std::decay_t<LeftWrapperRefer>;                              \
                using LeftHolder = typename LeftWrapper::Holder;                                   \
                using RightWrapperRefer = _Right &&;                                               \
                using RightWrapper = ::std::decay_t<RightWrapperRefer>;                            \
                /*using RightHolder = typename RightWrapper::Holder;*/                             \
                                                                                                   \
                constexpr bool                                                                     \
                    is_left_compatible_to_right = ::scl::feature::IsThisCompatibleWithOther<       \
                        RightWrapper,                                                              \
                        LeftWrapper>{};                                                            \
                constexpr bool holder_has_method_for_operator = ::scl::feature::detail::Operator:: \
                    Binary::doesOperator##Invokable##StaticMethodExist<LeftHolder,                 \
                        LeftWrapperRefer,                                                          \
                        RightWrapperRefer>();                                                      \
                using OperatorSwitchCase = ::std::conditional_t<is_left_compatible_to_right        \
                        && holder_has_method_for_operator,                                         \
                    ::scl::feature::detail::Operator::Binary::HolderHasOperatorCase,               \
                    ::scl::feature::detail::Operator::Binary::HolderHasNoOperatorCase>;            \
                return ::scl::feature::detail::Operator::Binary::Invokable##Switch<                \
                    BothWrapperCase,                                                               \
                    OperatorSwitchCase>::invoke(::std::forward<LeftWrapperRefer>(left),            \
                    ::std::forward<RightWrapperRefer>(right));                                     \
            }                                                                                      \
        };                                                                                         \
    }                                                                                              \
    namespace scl::feature::detail::Operator::Binary                                               \
    {                                                                                              \
        template <typename _LeftRefer, typename _RightRefer>                                       \
        struct Invokable##Helper                                                                   \
        {                                                                                          \
            static_assert(::std::is_reference<_LeftRefer>{},                                       \
                "The template parameter _LeftRefer must to be a "                                  \
                "reference type.");                                                                \
            static_assert(::std::is_reference<_RightRefer>{} || ::std::is_function<_RightRefer>{}, \
                "The template parameter _RightRefer must to be a "                                 \
                "reference type.");                                                                \
                                                                                                   \
            using LeftRefer = _LeftRefer;                                                          \
            using RightRefer = _RightRefer;                                                        \
                                                                                                   \
            static constexpr decltype(auto) invoke(LeftRefer left, RightRefer right)               \
            {                                                                                      \
                return ::scl::feature::detail::Operator::Binary::Invokable##WrapperSwitch<         \
                    ::scl::feature::detail::Operator::WrapperSwitchCase<LeftRefer,                 \
                        RightRefer> >::invoke(::std::forward<LeftRefer>(left),                     \
                    ::std::forward<RightRefer>(right));                                            \
            }                                                                                      \
        };                                                                                         \
    }
// clang-format on

#endif
