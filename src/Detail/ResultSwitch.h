#pragma once
#ifndef SCL_FEATURE_OPERATOR_RESULT_SWITCH_H
#define SCL_FEATURE_OPERATOR_RESULT_SWITCH_H

#include <ScL/Feature/Tool/Inplace/Default.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>
#include "OperatorTool.h"

namespace ScL { namespace Feature { namespace Inplace { struct Default; }}}

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator
    {
        struct NoneWrapperCase;
        struct LeftWrapperCase;
        struct RightWrapperCase;
        struct BothWrapperCase;

        template < typename _Left, typename _Right >
        struct WrapperCaseHelper
        {
            using Left = ::std::decay_t< _Left >;
            using Right = ::std::decay_t< _Right >;
            using Type = ::std::conditional_t< ::ScL::Feature::isWrapper< Left >() && ::ScL::Feature::isWrapper< Right >(),
                BothWrapperCase,
                ::std::conditional_t< ::ScL::Feature::isWrapper< Left >(),
                    LeftWrapperCase,
                    ::std::conditional_t< ::ScL::Feature::isWrapper< Right >(),
                        RightWrapperCase,
                        NoneWrapperCase > > >;
        };

        template < typename _Left, typename _Right = void >
        using WrapperSwitchCase = typename WrapperCaseHelper< _Left, _Right >::Type;

        struct BothExposingCase;
        struct RightExposingCase;
        struct LeftExposingCase;

        template < typename _Left, typename _Right >
        struct ExposingCaseHelper
        {
            using Left = ::std::decay_t< _Left >;
            using Right = ::std::decay_t< _Right >;
            using Type = ::std::conditional_t< ::ScL::Feature::isPartOfThisCompatibleWithOther< Right, Left >(),
                    RightExposingCase,          // The Right Wrapper should be exposed
                    ::std::conditional_t< ::ScL::Feature::isThisCompatibleWithPartOfOther< Right, Left >(),
                        LeftExposingCase,       // The Left Wrapper should be exposed
                        BothExposingCase > >;   // The Both Wrappers should be exposed
        };

        template < typename _Left, typename _Right >
        using ExposingSwitchCase = typename ExposingCaseHelper< _Left, _Right >::Type;
    }
}}}

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator
    {
        /* Cases for operator result type */
        struct FundamentalCase {};
        struct ThisCase {};
        struct DefaultCase {};
        struct BlockedCase {};

        template < bool, typename _Returned, typename _Refer >
        struct ResultCaseWrapperHelper;

        template < typename _Returned, typename _Refer >
        struct ResultCaseWrapperHelper< true, _Returned, _Refer >
        {
            using Returned = _Returned;
            using Wrapper = ::std::decay_t< Returned >;
            static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "" );
            using Value = typename Wrapper::Value;

            using Type = ::std::conditional_t< ::std::is_reference< Value >{},
                ::ScL::Feature::Detail::Operator::BlockedCase,
                ::ScL::Feature::Detail::Operator::DefaultCase >;
        };

        template < typename _Returned, typename _Refer >
        struct ResultCaseWrapperHelper< false, _Returned, _Refer >
        {
            using Returned = _Returned;
            using Value = ::std::decay_t< _Refer >;

            static constexpr bool returned_is_not_wrappable = ::std::is_fundamental< Returned >{} || ::std::is_enum< Returned >{};
            static constexpr bool returned_is_reference = ::std::is_reference< Returned >{};
            static constexpr bool returned_is_same_value = ::std::is_same< _Returned, _Refer >{};

            using Type = ::std::conditional_t< returned_is_not_wrappable,
                ::ScL::Feature::Detail::Operator::FundamentalCase,
                ::std::conditional_t< returned_is_same_value,
                    ::ScL::Feature::Detail::Operator::ThisCase,
                    ::std::conditional_t< returned_is_reference,
                        ::ScL::Feature::Detail::Operator::BlockedCase,
                        ::ScL::Feature::Detail::Operator::DefaultCase > > >;
        };

        template < typename _Returned, typename _Refer >
        struct ResultCaseHelper
        {
            using Type = typename ResultCaseWrapperHelper< ::ScL::Feature::isWrapper< _Returned >(), _Returned, _Refer >::Type;
        };

        template < typename _Returned, typename _Value >
        using ResultSwitchCase = typename ResultCaseHelper< _Returned, _Value >::Type;
    }
}}}

namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator
    {
        template < typename, typename >
        struct ResultSwitch;

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::FundamentalCase >
        {
            template < typename _Invokable, typename _Wrapper, typename ... _Arguments >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Wrapper && wrapper, _Arguments && ... arguments )
            {
                using WrapperRefer = _Wrapper &&;
                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                return invokable( WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess(), ::std::forward< _Arguments >( arguments ) ... );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::ThisCase >
        {
            template < typename _Invokable, typename _Wrapper, typename ... _Arguments >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Wrapper && wrapper, _Arguments && ... arguments )
            {
                using WrapperRefer = _Wrapper &&;
                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                invokable( WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess(), ::std::forward< _Arguments >( arguments ) ... );
                return ::std::forward< WrapperRefer >( wrapper );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::DefaultCase >
        {
            template < typename _Invokable, typename _Wrapper, typename ... _Arguments >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Wrapper && wrapper, _Arguments && ... arguments )
            {
                using WrapperRefer = _Wrapper &&;
                using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
                using Invokable = _Invokable;
                using Returned = ::std::invoke_result_t< Invokable, ValueRefer, _Arguments && ... >;

                static_assert( !::std::is_reference< Returned >{},
                    "The type of return parameter must to be not a reference type." );

                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned >;
                return ResultWrapper( invokable( WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess(), ::std::forward< _Arguments >( arguments ) ... ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::BlockedCase >
        {
            template < typename _Invokable, typename _Wrapper, typename ... _Arguments >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Wrapper && wrapper, _Arguments && ... arguments )
            {
                using WrapperRefer = _Wrapper &&;
                using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
                using Invokable = _Invokable;
                using InvokableRefer = _Invokable &&;
                using Returned = ::std::invoke_result_t< Invokable, ValueRefer, _Arguments && ... >;

                static_assert( ::std::is_reference< Returned >{},
                    "The type of return parameter must to be a reference type." );

                using GuardTool = ::ScL::Feature::Detail::Guard::LeftTool< Invokable, WrapperRefer, _Arguments && ... >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned, GuardTool >;
                return ResultWrapper( ::std::forward< InvokableRefer >( invokable ), ::std::forward< WrapperRefer >( wrapper ), ::std::forward< _Arguments && >( arguments ) ... );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::FundamentalCase >
        {
            template < typename _Invokable, typename _Left, typename _Wrapper >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Wrapper && wrapper )
            {
                using LeftRefer = _Left &&;
                using WrapperRefer = _Wrapper &&;
                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                return invokable( ::std::forward< LeftRefer >( left ), WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess() );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::ThisCase >
        {
            template < typename _Invokable, typename _Left, typename _Wrapper >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Wrapper && wrapper )
            {
                using LeftRefer = _Left &&;
                using WrapperRefer = _Wrapper &&;
                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                invokable( ::std::forward< LeftRefer >( left ), WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess() );
                return ::std::forward< WrapperRefer >( wrapper );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::DefaultCase >
        {
            template < typename _Invokable, typename _Left, typename _Wrapper >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Wrapper && wrapper )
            {
                using LeftRefer = _Left &&;
                using WrapperRefer = _Wrapper &&;
                using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
                using Invokable = _Invokable;
                using Returned = ::std::invoke_result_t< Invokable, LeftRefer, ValueRefer >;

                static_assert( !::std::is_reference< Returned >{},
                    "The type of return parameter must to be not a reference type." );

                using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned >;
                return ResultWrapper( invokable( ::std::forward< LeftRefer >( left ), WrapperGuard( ::std::forward< WrapperRefer >( wrapper ) ).wrapperAccess() ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::BlockedCase >
        {
            template < typename _Invokable, typename _Left, typename _Wrapper >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Wrapper && wrapper )
            {
                using LeftRefer = _Left &&;
                using WrapperRefer = _Wrapper &&;
                using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
                using Invokable = _Invokable;
                using InvokableRefer = _Invokable &&;
                using Returned = ::std::invoke_result_t< Invokable, LeftRefer, ValueRefer >;

                static_assert( ::std::is_reference< Returned >{},
                    "The type of return parameter must to be a reference type." );

                using GuardTool = ::ScL::Feature::Detail::Guard::RightTool< Invokable, LeftRefer, WrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned, GuardTool >;
                return ResultWrapper( ::std::forward< InvokableRefer >( invokable ), ::std::forward< LeftRefer >( left ), ::std::forward< WrapperRefer >( wrapper ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::FundamentalCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                return invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::FundamentalCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                return invokable( ::std::forward< LeftWrapperRefer >( left ),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::FundamentalCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                return invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    ::std::forward< RightWrapperRefer >( right ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::ThisCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() );
                return ::std::forward< LeftWrapperRefer >( left );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::ThisCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                invokable( ::std::forward< LeftWrapperRefer >( left ),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() );
                return ::std::forward< LeftWrapperRefer >( left );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::ThisCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    ::std::forward< RightWrapperRefer >( right ) );
                return ::std::forward< LeftWrapperRefer >( left );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::DefaultCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >;
                using RightWrapperRefer = _Right &&;
                using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >;
                using Invokable = _Invokable;
                using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightValueRefer >;

                static_assert( !::std::is_reference< Returned >{},
                    "The type of return parameter must to be not a reference type." );

                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned >;
                return ResultWrapper( invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::DefaultCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >;
                using Invokable = _Invokable;
                using Returned = ::std::invoke_result_t< Invokable, LeftWrapperRefer, RightValueRefer >;

                static_assert( !::std::is_reference< Returned >{},
                    "The type of return parameter must to be not a reference type." );

                using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned >;
                return ResultWrapper( invokable( ::std::forward< LeftWrapperRefer >( left ),
                    RightWrapperGuard( ::std::forward< RightWrapperRefer >( right ) ).wrapperAccess() ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::DefaultCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >;
                using RightWrapperRefer = _Right &&;
                using Invokable = _Invokable;
                using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightWrapperRefer >;

                static_assert( !::std::is_reference< Returned >{},
                    "The type of return parameter must to be not a reference type." );

                using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned >;
                return ResultWrapper( invokable( LeftWrapperGuard( ::std::forward< LeftWrapperRefer >( left ) ).wrapperAccess(),
                    ::std::forward< RightWrapperRefer >( right ) ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::BlockedCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >;
                using RightWrapperRefer = _Right &&;
                using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >;
                using Invokable = _Invokable;
                using InvokableRefer = _Invokable &&;
                using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightValueRefer >;

                using GuardTool = ::ScL::Feature::Detail::Guard::BothTool< Invokable, LeftWrapperRefer, RightWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned, GuardTool >;
                return ResultWrapper( ::std::forward< InvokableRefer >( invokable ), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::BlockedCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using RightWrapperRefer = _Right &&;
                using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >;
                using Invokable = _Invokable;
                using InvokableRefer = _Invokable &&;
                using Returned = ::std::invoke_result_t< Invokable, LeftWrapperRefer, RightValueRefer >;

                using GuardTool = ::ScL::Feature::Detail::Guard::RightTool< Invokable, LeftWrapperRefer, RightWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned, GuardTool >;
                return ResultWrapper( ::std::forward< InvokableRefer >( invokable ), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) );
            }
        };

        template <>
        struct ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::BlockedCase >
        {
            template < typename _Invokable, typename _Left, typename _Right >
            static constexpr decltype(auto) invoke ( _Invokable && invokable, _Left && left, _Right && right )
            {
                using LeftWrapperRefer = _Left &&;
                using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >;
                using RightWrapperRefer = _Right &&;
                using Invokable = _Invokable;
                using InvokableRefer = _Invokable &&;
                using Returned = ::std::invoke_result_t< Invokable, LeftValueRefer, RightWrapperRefer >;

                using GuardTool = ::ScL::Feature::Detail::Guard::LeftTool< Invokable, LeftWrapperRefer, RightWrapperRefer >;
                using ResultWrapper = ::ScL::Feature::Wrapper< Returned, GuardTool >;
                return ResultWrapper( ::std::forward< InvokableRefer >( invokable ), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) );
            }
        };
    }
}}}

#endif
