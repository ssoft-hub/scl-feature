#pragma once
#ifndef SCL_FEATURE_TOOL_GUARD_RANGE_TOOL_H
#define SCL_FEATURE_TOOL_GUARD_RANGE_TOOL_H

#include <cassert>
#include <scl/feature/access/value_guard.h>
#include <scl/feature/trait.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::detail::Guard
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
            using RangeGuard = ::scl::feature::ValueGuard< RangeRefer >;
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
                using IteratorRefer = ::scl::SimilarRefer< Iterator, HolderRefer >;
                return ::std::forward< IteratorRefer >( holder.m_iterator );
            }
        };
    };
} // namespace scl::feature::detail::Guard

#endif
