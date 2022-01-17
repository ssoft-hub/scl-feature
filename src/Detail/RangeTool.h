#pragma once
#ifndef SCL_FEATURE_TOOL_GUARD_RANGE_TOOL_H
#define SCL_FEATURE_TOOL_GUARD_RANGE_TOOL_H

#include <ScL/Feature/Access/ValueGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>
#include <functional>

namespace ScL { namespace Feature { namespace Detail { namespace Guard
{
    template < typename _RangeRefer >
    struct RangeTool
    {
        using RangeRefer = _RangeRefer;
        static_assert( ::std::is_reference< RangeRefer >::value,
            "The template parameter _RangeRefer must to be a reference type." );

        template < typename _Iterator >
        struct Holder
        {
            using ThisType = Holder;
            using RangeGuard = ::ScL::Feature::ValueGuard< RangeRefer >;
            using Iterator = _Iterator;

            RangeGuard m_range_guard;
            Iterator m_iterator;

            template < typename _Invocable >
            Holder ( RangeRefer wrapper, _Invocable invocable )
                : m_range_guard( ::std::forward< RangeRefer >( wrapper ) )
                , m_iterator( invocable( m_range_guard.valueAccess() ) )
            {
            }

            Holder ( ThisType && other )
                : m_range_guard( ::std::forward< RangeGuard >( other.m_range_guard ) )
                , m_iterator( ::std::forward< Iterator >( other.m_iterator ) )
            {
                assert( false ); // Restricted functionality
            }

            Holder ( const ThisType & other ) = delete;

            template < typename _HolderRefer >
            static constexpr decltype(auto) value ( _HolderRefer && holder )
            {
                using HolderRefer = _HolderRefer &&;
                using IteratorRefer = ::ScL::SimilarRefer< Iterator, HolderRefer >;
                return ::std::forward< IteratorRefer >( holder.m_iterator );
            }
        };
    };
}}}}

#endif
