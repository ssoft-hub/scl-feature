#pragma once
#ifndef SCL_FEATURE_OPERATOR_BINARY_H
#define SCL_FEATURE_OPERATOR_BINARY_H

#include <ScL/Meta/Trait/Detection/Method.h>
#include <ScL/Meta/Trait/Detection/Operator.h>
#include <ScL/Utility/SimilarRefer.h>
#include <ScL/Utility/SingleArgument.h>
#include "ResultSwitch.h"

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator { namespace Binary
    {
        /* Cases for Holder method existing */
        struct HolderHasOperatorCase {};
        struct HolderHasNoOperatorCase {};
    }}
}}}

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator { namespace Global
    {
            struct DefaultCase;
    }}
}}}

#define SCL_FEATURE_DOES_BINARY_OPERATOR_EXIST( Invokable ) \
    template < typename _Kind, typename _LeftRefer, typename _RightRefer > \
    struct Does ## Invokable ## OperatorExistHelper; \
     \
    template < typename _LeftRefer, typename _RightRefer > \
    using Does ## Invokable ## OperatorExist = Does ## Invokable ## OperatorExistHelper< ::ScL::Feature::Detail::Operator::WrapperSwitchCase< _LeftRefer, _RightRefer >, _LeftRefer, _RightRefer >; \
     \
    template < typename _LeftRefer, typename _RightRefer > \
    inline constexpr bool does ## Invokable ## OperatorExist () { return Does ## Invokable ## OperatorExist< _LeftRefer, _RightRefer >{}; } \
     \
    /* Non wrapper case */ \
    template < typename _LeftRefer, typename _RightRefer > \
    struct Does ## Invokable ## OperatorExistHelper< ::ScL::Feature::Detail::Operator::NoneWrapperCase, _LeftRefer, _RightRefer > \
    { \
        static_assert( ::std::is_reference< _LeftRefer >{}, "The template parameter _LeftRefer must to be a reference type." ); \
        static_assert( ::std::is_reference< _RightRefer >{}, "The template parameter _RightRefer must to be a reference type." ); \
     \
        static constexpr bool value = ::ScL::Meta::isDetected< ::ScL::Meta::Invokable ## UnstrictOperation, _LeftRefer, _RightRefer >(); \
        constexpr operator bool () const noexcept { return value; } \
    }; \
     \
    /* Member of Wrapper case */ \
    template < typename _LeftWrapperRefer, typename _RightRefer > \
    struct Does ## Invokable ## OperatorExistHelper< ::ScL::Feature::Detail::Operator::LeftWrapperCase, _LeftWrapperRefer, _RightRefer > \
    { \
        static_assert( ::std::is_reference< _LeftWrapperRefer >{}, "The template parameter _LeftWrapperRefer must to be a reference type." ); \
        static_assert( ::std::is_reference< _RightRefer >{} || ::std::is_function< _RightRefer >{}, "The template parameter _RightRefer must to be a reference type." ); \
     \
        using LeftWrapperRefer = _LeftWrapperRefer; \
        using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
        using LeftHolder = typename LeftWrapper::Holder; \
        using LeftValue = typename LeftWrapper::Value; \
        using LeftValueRefer = ::ScL::SimilarRefer< LeftValue, LeftWrapperRefer >; \
        using RightRefer = _RightRefer; \
     \
        static constexpr bool value = ::ScL::Meta::isDetected< ::ScL::Meta::Invokable ## UnstrictOperation, LeftValueRefer, RightRefer >() \
            || ::ScL::Meta::isDetected< Operator ## Invokable ## ForLeftWrapperStaticMethodStrictOperation, LeftHolder, LeftWrapperRefer, RightRefer >(); \
        constexpr operator bool () const noexcept { return value; } \
    }; \
     \
    /* Global case */ \
    template < typename _LeftRefer, typename _RightWrapperRefer > \
    struct Does ## Invokable ## OperatorExistHelper< ::ScL::Feature::Detail::Operator::RightWrapperCase, _LeftRefer, _RightWrapperRefer > \
    { \
        static_assert( ::std::is_reference< _LeftRefer >{}, "The template parameter _LeftRefer must to be a reference type." ); \
        static_assert( ::std::is_reference< _RightWrapperRefer >{}, "The template parameter _RightWrapperRefer must to be a reference type." ); \
     \
        using LeftRefer = _LeftRefer; \
        using RightWrapperRefer = _RightWrapperRefer; \
        using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
        using RightHolder = typename RightWrapper::Holder; \
        using RightValue = typename RightWrapper::Value; \
        using RightValueRefer = ::ScL::SimilarRefer< RightValue, RightWrapperRefer >; \
     \
        static constexpr bool value = ::ScL::Meta::isDetected< ::ScL::Meta::Invokable ## UnstrictOperation, LeftRefer, RightValueRefer >() \
            || ::ScL::Meta::isDetected< Operator ## Invokable ## ForRightWrapperStaticMethodStrictOperation, RightHolder, LeftRefer, RightWrapperRefer >(); \
        constexpr operator bool () const noexcept { return value; } \
    }; \
     \
    /* Member of Wrapper case */ \
    template < typename _LeftWrapperRefer, typename _RightWrapperRefer > \
    struct Does ## Invokable ## OperatorExistHelper< ::ScL::Feature::Detail::Operator::BothWrapperCase, _LeftWrapperRefer, _RightWrapperRefer > \
    { \
        static_assert( ::std::is_reference< _LeftWrapperRefer >{}, "The template parameter _LeftWrapperRefer must to be a reference type." ); \
        static_assert( ::std::is_reference< _RightWrapperRefer >{}, "The template parameter _RightWrapperRefer must to be a reference type." ); \
     \
        using LeftWrapperRefer = _LeftWrapperRefer; \
        using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
        using LeftHolder = typename LeftWrapper::Holder; \
        using LeftValue = typename LeftWrapper::Value; \
        using LeftValueRefer = ::ScL::SimilarRefer< LeftValue, LeftWrapperRefer >; \
        using RightWrapperRefer = _RightWrapperRefer; \
        using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
        using RightValue = typename RightWrapper::Value; \
        using RightValueRefer = ::ScL::SimilarRefer< RightValue, RightWrapperRefer >; \
     \
        static constexpr bool is_right_compatible_with_left = ::ScL::Feature::isThisCompatibleWithOther< RightWrapper, LeftWrapper >() \
            && ( ::ScL::Meta::isDetected< Operator ## Invokable ## StaticMethodStrictOperation, LeftHolder, LeftWrapperRefer, RightWrapperRefer >() \
                || does ## Invokable ## OperatorExist< LeftValueRefer, RightValueRefer >() ); \
     \
        static constexpr bool is_path_of_right_compatible_with_left = ::ScL::Feature::isPartOfThisCompatibleWithOther< RightWrapper, LeftWrapper >() \
            && does ## Invokable ## OperatorExist< LeftWrapperRefer, RightValueRefer >(); \
     \
        static constexpr bool is_right_compatible_with_path_of_left = ::ScL::Feature::isThisCompatibleWithPartOfOther< RightWrapper, LeftWrapper >() \
            && does ## Invokable ## OperatorExist< LeftValueRefer, RightWrapperRefer >(); \
     \
        static constexpr bool is_right_not_compatible_with_left = !::ScL::Feature::isThisCompatibleWithOther< RightWrapper, LeftWrapper >() \
            && does ## Invokable ## OperatorExist< LeftValueRefer, RightValueRefer >(); \
     \
        static constexpr bool value = is_right_compatible_with_left \
            || is_path_of_right_compatible_with_left \
            || is_right_compatible_with_path_of_left \
            || is_right_not_compatible_with_left; \
        constexpr operator bool () const noexcept { return value; } \
    }; \

#define SCL_BINARY_OPERATOR_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            SCL_META_METHOD_DETECTION( operator ## Invokable, Operator ## Invokable ) \
            template < typename ... _Arguments > \
            inline static constexpr bool does ## Operator ## Invokable ## StaticMethodExist () { return ::ScL::Meta::isDetected< Operator ## Invokable ## StaticMethodStrictOperation, _Arguments ... >(); } \
            SCL_META_METHOD_DETECTION( operator ## Invokable ## ForLeftWrapper, Operator ## Invokable ## ForLeftWrapper ) \
            template < typename ... _Arguments > \
            inline static constexpr bool does ## Operator ## Invokable ## ForLeftWrapper ## StaticMethodExist () { return ::ScL::Meta::isDetected< Operator ## Invokable ## ForLeftWrapper ## StaticMethodStrictOperation, _Arguments ... >(); } \
            SCL_META_METHOD_DETECTION( operator ## Invokable ## ForRightWrapper, Operator ## Invokable ## ForRightWrapper ) \
            template < typename ... _Arguments > \
            inline static constexpr bool does ## Operator ## Invokable ## ForRightWrapper ## StaticMethodExist () { return ::ScL::Meta::isDetected< Operator ## Invokable ## ForRightWrapper ## StaticMethodStrictOperation, _Arguments ... >(); } \
            SCL_FEATURE_DOES_BINARY_OPERATOR_EXIST( Invokable ) \
        }} \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Global \
        { \
            struct Invokable ## Case; \
        }} \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            template < typename > \
            struct Invokable ## Operator; \
             \
            template <> \
            struct Invokable ## Operator< ::ScL::Feature::Detail::Operator::Global::DefaultCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftRefer = _Left &&; \
                    using RightRefer = _Right &&; \
                    return ::std::forward< LeftRefer >( left ) symbol ::std::forward< RightRefer >( right ); \
                } \
            }; \
             \
            struct Invokable \
            { \
                template < typename _Left, typename _Right > \
                decltype(auto) operator () ( _Left && left, _Right && right ) \
                { \
                    using LeftRefer = _Left &&; \
                    using RightRefer = _Right &&; \
                    using Invokable ## OperatorSwitchCase = ::std::conditional_t< \
                        ::ScL::Feature::isWrapper< ::std::decay_t< RightRefer > >(), \
                            ::ScL::Feature::Detail::Operator::Global::Invokable ## Case, \
                            ::ScL::Feature::Detail::Operator::Global::DefaultCase >; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Operator< Invokable ## OperatorSwitchCase >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
        }} \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            template < typename, typename > \
            struct Invokable ## Switch; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftHolder = typename ::std::decay_t< LeftWrapperRefer >::Holder; \
                    using RightRefer = _Right &&; \
                    return LeftHolder::operator ## Invokable ## ForLeftWrapper( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                    using RightRefer = _Right &&; \
                    using Invokable = ::ScL::Feature::Detail::Operator::Binary::Invokable; \
     \
                    using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightRefer >; \
                    return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                        ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftRefer = _Left &&; \
                    using RightWrapperRefer = _Right &&; \
                    using RightHolder = typename ::std::decay_t< RightWrapperRefer >::Holder; \
                    return RightHolder::operator ## Invokable ## ForRightWrapper( ::std::forward< LeftRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftRefer = _Left &&; \
                    using RightWrapperRefer = _Right &&; \
                    using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
                    using Invokable = ::ScL::Feature::Detail::Operator::Binary::Invokable; \
     \
                    using Returned = ::std::invoke_result_t< Invokable, LeftRefer, RightValueRefer >; \
                    return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, RightValueRefer > > \
                        ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftHolder = typename ::std::decay_t< LeftWrapperRefer >::Holder; \
                    using RightWrapperRefer = _Right &&; \
                    return LeftHolder::operator ## Invokable ( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using RightWrapperRefer = _Right &&; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< ::ScL::Feature::Detail::Operator::ExposingSwitchCase< LeftWrapperRefer, RightWrapperRefer >, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                        ::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                    using RightWrapperRefer = _Right &&; \
                    using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
                    using Invokable = ::ScL::Feature::Detail::Operator::Binary::Invokable; \
     \
                    using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightValueRefer >; \
                    return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                        ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                    using RightWrapperRefer = _Right &&; \
                    using Invokable = ::ScL::Feature::Detail::Operator::Binary::Invokable; \
     \
                    using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightWrapperRefer >; \
                    return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                        ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
            { \
                template < typename _Left, typename _Right > \
                static decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using RightWrapperRefer = _Right &&; \
                    using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
                    using Invokable = ::ScL::Feature::Detail::Operator::Binary::Invokable; \
     \
                    using Returned = ::std::invoke_result_t< Invokable, LeftWrapperRefer, RightValueRefer >; \
                    return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftWrapperRefer > > \
                        ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
        }} \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            template < typename > \
            struct Invokable ## WrapperSwitch; \
     \
            template <> \
            struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::LeftWrapperCase > \
            { \
                template < typename _Left, typename _Right > \
                static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
                    using LeftHolder = typename LeftWrapper::Holder; \
                    using RightRefer = _Right &&; \
     \
                    constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::doesOperator ## Invokable ## ForLeftWrapperStaticMethodExist< LeftHolder, LeftWrapperRefer, RightRefer >(); \
                    using OperatorSwitchCase = ::std::conditional_t< holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< LeftWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::RightWrapperCase > \
            { \
                template < typename _Left, typename _Right > \
                static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using RightWrapperRefer = _Right &&; \
                    using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
                    using RightHolder = typename RightWrapper::Holder; \
                    using LeftRefer = _Left &&; \
     \
                    constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::doesOperator ## Invokable ## ForRightWrapperStaticMethodExist< RightHolder, LeftRefer, RightWrapperRefer >(); \
                    using OperatorSwitchCase = ::std::conditional_t< holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< RightWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
     \
            template <> \
            struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::BothWrapperCase > \
            { \
                template < typename _Left, typename _Right > \
                static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                { \
                    using LeftWrapperRefer = _Left &&; \
                    using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
                    using LeftHolder = typename LeftWrapper::Holder; \
                    using RightWrapperRefer = _Right &&; \
                    using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
                    /*using RightHolder = typename RightWrapper::Holder;*/ \
     \
                    constexpr bool is_left_compatible_to_right = ::ScL::Feature::IsThisCompatibleWithOther< RightWrapper, LeftWrapper >{}; \
                    constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::doesOperator ## Invokable ## StaticMethodExist< LeftHolder, LeftWrapperRefer, RightWrapperRefer >(); \
                    using OperatorSwitchCase = ::std::conditional_t< is_left_compatible_to_right && holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< BothWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                } \
            }; \
        }} \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator { namespace Binary \
        { \
            template < typename _LeftRefer, typename _RightRefer > \
            struct Invokable ## Helper \
            { \
                static_assert( ::std::is_reference< _LeftRefer >{}, "The template parameter _LeftRefer must to be a reference type." ); \
                static_assert( ::std::is_reference< _RightRefer >{} || ::std::is_function<  _RightRefer >{}, "The template parameter _RightRefer must to be a reference type." ); \
     \
                using LeftRefer = _LeftRefer; \
                using RightRefer = _RightRefer; \
     \
                static constexpr decltype(auto) invoke( LeftRefer left, RightRefer right ) \
                { \
                    return ::ScL::Feature::Detail::Operator::Binary::Invokable ## WrapperSwitch< ::ScL::Feature::Detail::Operator::WrapperSwitchCase< LeftRefer, RightRefer > > \
                        ::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                } \
            }; \
        }} \
    }}} \

#endif
