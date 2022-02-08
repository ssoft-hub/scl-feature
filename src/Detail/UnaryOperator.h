#pragma once
#ifndef SCL_UNARY_OPERATOR_H
#define SCL_UNARY_OPERATOR_H

#include <ScL/Meta/Trait/IsMethodExists.h>
#include <ScL/Meta/Trait/IsOperatorExists.h>
#include <ScL/Utility/SingleArgument.h>
#include <ScL/Utility/SimilarRefer.h>
#include "ResultSwitch.h"

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator
    {
        namespace Unary
        {
            /* Cases for Holder method existing */
            struct HolderHasOperatorCase {};
            struct HolderHasNoOperatorCase {};
        }
    }
}}}

#define SCL_IS_UNARY_OPERATOR_EXISTS_TEST_TRAIT( Invokable ) \
    template < typename _Kind, typename _Refer, typename ... _Arguments > \
    struct Is ## Invokable ## OperatorExistsTestHelper; \
     \
    template < typename _Refer, typename ... _Arguments > \
    using Is ## Invokable ## OperatorExistsTest = Is ## Invokable ## OperatorExistsTestHelper< ::ScL::Feature::Detail::Operator::WrapperSwitchCase< _Refer >, _Refer, _Arguments ... >; \
     \
    template < typename _Refer, typename ... _Arguments > \
    /*inline*/ constexpr bool is_ ## Invokable ## _operator_exists_test = Is ## Invokable ## OperatorExistsTest< _Refer, _Arguments ... >::value; \
     \
    template < typename _Refer, typename ... _Arguments > \
    inline constexpr bool is ## Invokable ## OperatorExistsTest () { return Is ## Invokable ## OperatorExistsTest< _Refer, _Arguments ... >::value; } \
     \
    template < typename _Refer, typename ... _Arguments > \
    struct Is ## Invokable ## OperatorExistsTestHelper< ::ScL::Feature::Detail::Operator::NoneWrapperCase, _Refer, _Arguments ... > \
    { \
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." ); \
     \
        static const bool value = ::ScL::Feature::Detail::Operator::Unary::is_ ## Invokable ## _operator_exists< _Refer, _Arguments ... >; \
    }; \
     \
    template < typename _Refer, typename ... _Arguments > \
    struct Is ## Invokable ## OperatorExistsTestHelper< ::ScL::Feature::Detail::Operator::UnaryWrapperCase, _Refer, _Arguments ...  > \
    { \
        static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." ); \
        using WrapperRefer = _Refer; \
        using Wrapper = ::std::decay_t< WrapperRefer >; \
        using Holder = typename Wrapper::Holder; \
        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >; \
        using Value = typename Wrapper::Value; \
        using ValueRefer = ::ScL::SimilarRefer< Value, WrapperRefer >; \
     \
        static const bool value = ::ScL::Feature::Detail::Operator::Unary::is_operator ## Invokable ## _method_exists< Holder, void(HolderRefer, _Arguments ...) > \
            || ::ScL::Feature::Detail::Operator::Unary::is_ ## Invokable ## _operator_exists< ValueRefer, _Arguments ...  >; \
    }; \
     \

#define SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION( Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                template < typename > \
                struct Invokable ## Switch; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::Unary::HolderHasOperatorCase > \
                { \
                    template < typename _Wrapper, typename ... _Arguments > \
                    static decltype(auto) invoke ( _Wrapper && wrapper, _Arguments && ... arguments ) \
                    { \
                        using WrapperRefer = _Wrapper &&; \
                        using Holder = typename ::std::decay_t< WrapperRefer >::Holder; \
                        /*return Holder::operator ## Invokable( ::std::forward< WrapperRefer >( wrapper ), ::std::forward< _Arguments >( arguments ) ... );*/ \
                        Holder::operator ## Invokable( ::ScL::Feature::Detail::wrapperHolder( ::std::forward< WrapperRefer >( wrapper ) ), ::std::forward< _Arguments >( arguments ) ... ); \
                        return ::std::forward< WrapperRefer >( wrapper ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::Unary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Wrapper, typename ... _Arguments > \
                    static decltype(auto) invoke ( _Wrapper && wrapper, _Arguments && ... arguments ) \
                    { \
                        using WrapperRefer = _Wrapper &&; \
                        using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >; \
                        using Returned = ::std::result_of_t< Invokable( ValueRefer, _Arguments && ... ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::UnaryWrapperCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, ValueRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Unary::Invokable(), ::std::forward< WrapperRefer >( wrapper ), ::std::forward< _Arguments >( arguments ) ... ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                template < typename > \
                struct Invokable ## WrapperSwitch; \
         \
                template <> \
                struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::UnaryWrapperCase > \
                { \
                    template < typename _Refer, typename ... _Arguments > \
                    static constexpr decltype(auto) invoke ( _Refer && value, _Arguments && ... arguments ) \
                    { \
                        using WrapperRefer = _Refer &&; \
                        using Wrapper = ::std::decay_t< WrapperRefer >; \
                        using Holder = typename Wrapper::Holder; \
                        using HolderRefer = ::ScL::SimilarRefer< Holder, WrapperRefer >; \
         \
                        constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Unary::is_operator ## Invokable ## _method_exists< Holder, void( HolderRefer, _Arguments && ... ) >; \
                        using OperatorSwitchCase = ::std::conditional_t< holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Unary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Unary::HolderHasNoOperatorCase >; \
                        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## Switch< OperatorSwitchCase >::invoke( ::std::forward< WrapperRefer >( value ), ::std::forward< _Arguments >( arguments ) ... ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
           namespace Unary \
           { \
               template < typename _Refer, typename ... _Arguments > \
               struct Invokable ## Helper \
               { \
                   static_assert( ::std::is_reference< _Refer >::value, "The template parameter _Refer must to be a reference type." ); \
                   using Refer = _Refer; \
         \
                    static constexpr decltype(auto) invoke( Refer value, _Arguments && ... arguments ) \
                    { \
                        return ::ScL::Feature::Detail::Operator::Unary::Invokable ## WrapperSwitch< ::ScL::Feature::Detail::Operator::WrapperSwitchCase< Refer > > \
                            ::invoke( ::std::forward< Refer >( value ), ::std::forward< _Arguments && >( arguments ) ... ); \
                    } \
                }; \
            } \
        } \
    }}} \

#define SCL_POSTFIX_UNARY_OPERATOR_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                SCL_IS_POSTFIX_UNARY_OPERATOR_EXISTS_TRAIT( SCL_SINGLE_ARG( symbol ), Invokable ) \
                SCL_IS_METHOD_EXISTS_TRAIT( operator ## Invokable ) \
                SCL_IS_UNARY_OPERATOR_EXISTS_TEST_TRAIT( Invokable ) \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                struct Invokable \
                { \
                    template < typename _Value > \
                    decltype(auto) operator () ( _Value && value ) \
                    { \
                        return ::std::forward< _Value && >( value ) symbol; \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION( Invokable ) \

#define SCL_PREFIX_UNARY_OPERATOR_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                SCL_IS_PREFIX_UNARY_OPERATOR_EXISTS_TRAIT( SCL_SINGLE_ARG( symbol ), Invokable ) \
                SCL_IS_METHOD_EXISTS_TRAIT( operator ## Invokable ) \
                SCL_IS_UNARY_OPERATOR_EXISTS_TEST_TRAIT( Invokable ) \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                struct Invokable \
                { \
                    template < typename _Value > \
                    decltype(auto) operator () ( _Value && value ) \
                    { \
                        return symbol ::std::forward< _Value >( value ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION( Invokable ) \


#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                SCL_IS_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENT_EXISTS_TRAIT( SCL_SINGLE_ARG( symbol ), Invokable ) \
                SCL_IS_METHOD_EXISTS_TRAIT( operator ## Invokable ) \
                SCL_IS_UNARY_OPERATOR_EXISTS_TEST_TRAIT( Invokable ) \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                struct Invokable \
                { \
                    template < typename _Value, typename _Argument > \
                    decltype(auto) operator () ( _Value && value, _Argument && argument ) \
                    { \
                        return ::std::forward< _Value >( value ).operator symbol ( ::std::forward< _Argument >( argument ) ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION( Invokable ) \

#define SCL_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                SCL_IS_POSTFIX_UNARY_OPERATOR_WITH_ARGUMENTS_EXISTS_TRAIT( SCL_SINGLE_ARG( symbol ), Invokable ) \
                SCL_IS_METHOD_EXISTS_TRAIT( operator ## Invokable ) \
                SCL_IS_UNARY_OPERATOR_EXISTS_TEST_TRAIT( Invokable ) \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Unary \
            { \
                struct Invokable \
                { \
                    template < typename _Value, typename ... _Arguments > \
                    decltype(auto) operator () ( _Value && value, _Arguments && ... arguments ) \
                    { \
                        return ::std::forward< _Value >( value ).operator symbol ( ::std::forward< _Arguments >( arguments ) ... ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    SCL_COMMON_UNARY_OPERATOR_IMPLEMENTAION( Invokable ) \

#endif
