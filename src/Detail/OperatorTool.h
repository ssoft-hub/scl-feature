#pragma once
#ifndef SCL_FEATURE_TOOL_GUARD_OPERATOR_TOOL_H
#define SCL_FEATURE_TOOL_GUARD_OPERATOR_TOOL_H

#include <ScL/Feature/Access/WrapperGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Detail { namespace Guard
{
    template < typename _Invokable, typename _WrapperRefer, typename ... _Arguments >
    struct LeftTool
    {
        using WrapperRefer = _WrapperRefer;
        using Invokable = _Invokable;
        static_assert( ::std::is_reference< WrapperRefer >{},
            "The template parameter _WrapperRefer must to be a reference type." );

        template < typename _Type >
        struct Holder
        {
            using ThisType = Holder;
            using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
            using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
            using ResultRefer = _Type;

            static_assert( ::std::is_same< ResultRefer, ::std::result_of_t< Invokable( ValueRefer, _Arguments && ... ) > >{},
                "The result of _Invokable( _LeftRefer, _WrapperRefer ) must to be a _Type" );

            WrapperGuard m_feature_guard;
            ResultRefer m_result_refer;

            Holder (  Invokable && invokable, WrapperRefer wrapper, _Arguments && ... arguments )
                : m_feature_guard( ::std::forward< WrapperRefer >( wrapper ) )
                , m_result_refer( ::std::forward< ResultRefer >( invokable( ::std::forward< ValueRefer >( m_feature_guard.wrapperAccess() ), ::std::forward< _Arguments >( arguments ) ...  ) ) )
            {
            }

            Holder ( ThisType && other )
                : m_feature_guard( ::std::forward< WrapperGuard >( other.m_feature_guard ) )
                , m_result_refer( ::std::forward< ResultRefer >( other.m_result_refer ) )
            {
                assert( false ); // Restricted functionality
            }

            Holder ( const ThisType & other ) = delete;

            template < typename _HolderRefer,
                typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
            static constexpr ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > value ( _HolderRefer && holder )
            {
                return ::std::forward< ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > >( holder.m_result_refer );
            }
        };
    };
}}}}

namespace ScL { namespace Feature { namespace Detail { namespace Guard
{
    template < typename _Invokable, typename _LeftRefer, typename _WrapperRefer >
    struct RightTool
    {
        using LeftRefer = _LeftRefer;
        using WrapperRefer = _WrapperRefer;
        using Invokable = _Invokable;

        static_assert( ::std::is_reference< _LeftRefer >{},
            "The template parameter _LeftRefer must to be a reference type." );
        static_assert( ::std::is_reference< _WrapperRefer >{},
            "The template parameter _WrapperRefer must to be a reference type." );

        template < typename _Type >
        struct Holder
        {
            using ThisType = Holder;
            using WrapperGuard = ::ScL::Feature::Detail::WrapperGuard< WrapperRefer >;
            using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;
            using ResultRefer = _Type;

            static_assert( ::std::is_same< ResultRefer, ::std::result_of_t< Invokable( LeftRefer, ValueRefer ) > >{},
                "The result of _Invokable( _LeftRefer, _WrapperRefer ) must to be a _Type" );

            WrapperGuard m_feature_guard;
            ResultRefer m_result_refer;

            Holder (  Invokable && invokable, LeftRefer left, WrapperRefer wrapper )
                : m_feature_guard( ::std::forward< WrapperRefer >( wrapper ) )
                , m_result_refer( ::std::forward< ResultRefer >( invokable(  ::std::forward< LeftRefer >( left ), ::std::forward< ValueRefer >( m_feature_guard.wrapperAccess() ) ) ) )
            {
            }

            Holder ( ThisType && other )
                : m_feature_guard( ::std::forward< WrapperGuard >( other.m_feature_guard ) )
                , m_result_refer( ::std::forward< ResultRefer >( other.m_result_refer ) )
            {
                assert( false ); // Restricted functionality
            }

            Holder ( const ThisType & other ) = delete;

            template < typename _HolderRefer,
                typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
            static constexpr ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > value ( _HolderRefer && holder )
            {
                return ::std::forward< ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > >( holder.m_result_refer );
            }
        };
    };
}}}}

namespace ScL { namespace Feature { namespace Detail { namespace Guard
{
    template < typename _Invokable, typename _LeftWrapperRefer, typename _RightWrapperRefer >
    struct BothTool
    {
        using Invokable = _Invokable;
        using LeftWrapperRefer = _LeftWrapperRefer;
        using RightWrapperRefer = _RightWrapperRefer;
        static_assert( ::std::is_reference< LeftWrapperRefer >{},
            "The template parameter _LeftWrapperRefer must to be a reference type." );
        static_assert( ::std::is_reference< RightWrapperRefer >{},
            "The template parameter _RightWrapperRefer must to be a reference type." );

        template < typename _Type >
        struct Holder
        {
            using ThisType = Holder;
            using LeftWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< LeftWrapperRefer >;
            using RightWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< RightWrapperRefer >;
            using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >;
            using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >;
            using ResultRefer = _Type;

            static_assert( ::std::is_same< ResultRefer, ::std::result_of_t< Invokable( LeftValueRefer, RightValueRefer ) > >{},
                "The result of _Invokable( _LeftRefer, _WrapperRefer ) must to be a _Type" );

            LeftWrapperGuard m_left_feature_guard;
            RightWrapperGuard m_right_feature_guard;
            ResultRefer m_result_refer;

            Holder (  Invokable && invokable, LeftWrapperRefer left, RightWrapperRefer right )
                : m_left_feature_guard( ::std::forward< LeftWrapperRefer >( left ) )
                , m_right_feature_guard( ::std::forward< RightWrapperRefer >( right ) )
                , m_result_refer( invokable( ::std::forward< LeftValueRefer >( m_left_feature_guard.wrapperAccess() ), ::std::forward< RightValueRefer >( m_right_feature_guard.wrapperAccess() ) ) )
            {
            }

            Holder ( ThisType && other )
                : m_left_feature_guard( ::std::forward< LeftWrapperGuard >( other.m_left_feature_guard ) )
                , m_right_feature_guard( ::std::forward< RightWrapperGuard >( other.m_right_feature_guard ) )
                , m_result_refer( ::std::forward< ResultRefer >( other.m_result_refer ) )
            {
                assert( false ); // Restricted functionality
            }

            Holder ( const ThisType & other ) = delete;

            template < typename _HolderRefer,
                typename = ::std::enable_if_t< ::std::is_same< ThisType, ::std::decay_t< _HolderRefer > >{} > >
            static constexpr ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > value ( _HolderRefer && holder )
            {
                return ::std::forward< ::ScL::SimilarRefer< ResultRefer, _HolderRefer && > >( holder.m_result_refer );
            }
        };
    };
}}}}

#endif
