#pragma once
#ifndef SCL_FEATURE_RANGE_H
#define SCL_FEATURE_RANGE_H

#include <vector>
//< The range global methods definitions
#include "RangeTool.h"
#include <utility>

// namespace ScL::Feature::Inplace { struct Default; }
namespace ScL::Feature::Detail
{
    template <typename, typename>
    class Wrapper;
} // namespace ScL::Feature::Detail

// ::std::begin, ::std::cbegin, ::std::end, ::std::cend, ::std::size, ::std::ssize, ::std::data,
// ::std::cdata
// ::std::ranges::begin, ::std::ranges::cbegin, ::std::ranges::end, ::std::ranges::cend,
// ::std::ranges::size, ::std::ranges::ssize, ::std::ranges::data, ::std::ranges::cdata

#define SCL_RANGE_METHOD_PROTOTYPE(method, refer)                                             \
    template <typename _Type, typename _Tool, typename = ::std::enable_if_t<true> >           \
    inline decltype(auto) method(::ScL::Feature::Detail::Wrapper<_Type, _Tool> refer value)   \
    {                                                                                         \
        using RangeWrapperRefer = ::ScL::Feature::Detail::Wrapper<_Type, _Tool> refer;        \
        using RangeRefer = decltype(*&::std::declval<RangeWrapperRefer>());                   \
        using IteratorType = decltype(::std::method(::std::declval<RangeRefer>()));           \
        using IteratorWrapper = ::ScL::Feature::Detail::Wrapper<IteratorType,                 \
            ::ScL::Feature::Detail::Guard::RangeTool<RangeWrapperRefer> >;                    \
        return IteratorWrapper(value, [](RangeRefer range) { return ::std::method(range); }); \
    }

#define SCL_RANGE_METHOD(method)                          \
    SCL_RANGE_METHOD_PROTOTYPE(method, &&)                \
    SCL_RANGE_METHOD_PROTOTYPE(method, const &&)          \
    SCL_RANGE_METHOD_PROTOTYPE(method, volatile &&)       \
    SCL_RANGE_METHOD_PROTOTYPE(method, const volatile &&) \
    SCL_RANGE_METHOD_PROTOTYPE(method, &)                 \
    SCL_RANGE_METHOD_PROTOTYPE(method, const &)           \
    SCL_RANGE_METHOD_PROTOTYPE(method, volatile &)        \
    SCL_RANGE_METHOD_PROTOTYPE(method, const volatile &)

namespace std
{
    SCL_RANGE_METHOD(begin)
    SCL_RANGE_METHOD(cbegin)
    SCL_RANGE_METHOD(end)
    SCL_RANGE_METHOD(cend)
    SCL_RANGE_METHOD(size)
    SCL_RANGE_METHOD(ssize)
    SCL_RANGE_METHOD(data)
    SCL_RANGE_METHOD(csize)
} // namespace std

// namespace std { namespace ranges
//{
//     SCL_RANGE_METHOD( begin )
//     SCL_RANGE_METHOD( cbegin )
//     SCL_RANGE_METHOD( end )
//     SCL_RANGE_METHOD( cend )
//     SCL_RANGE_METHOD( size )
//     SCL_RANGE_METHOD( ssize )
//     SCL_RANGE_METHOD( data )
//     SCL_RANGE_METHOD( csize )
// }}

#endif
