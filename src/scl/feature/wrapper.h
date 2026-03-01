#pragma once
#ifndef SCL_FUTURE_WRAPPER_H
#define SCL_FUTURE_WRAPPER_H

#ifdef _MSC_VER
#pragma warning(disable: 4521 4522)
#endif

namespace scl::feature::Inplace
{
    struct Default;
} // namespace scl::feature::Inplace

namespace scl::feature::detail
{
    template <typename, typename>
    class wrapper;
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename _Value, typename... _Tools>
    struct MultiToolHelper;

    template <typename _Value>
    struct MultiToolHelper<_Value>
    {
        using Type = typename MultiToolHelper<_Value, ::scl::feature::Inplace::Default>::Type;
    };

    template <typename _Value, typename _Tool>
    struct MultiToolHelper<_Value, _Tool>
    {
        using Type = ::scl::feature::detail::wrapper<_Value, _Tool>;
    };

    template <typename _Value, typename _Tool>
    struct MultiToolHelper< ::scl::feature::detail::wrapper<_Value, _Tool> >
    {
        using Type = ::scl::feature::detail::wrapper<_Value, _Tool>;
    };

    template <typename _Value, typename _Tool, typename... _Tools>
    struct MultiToolHelper<_Value, _Tool, _Tools...>
    {
        using Type = typename MultiToolHelper< ::scl::feature::detail::wrapper<_Value, _Tool>,
            _Tools...>::Type;
    };

    template <typename _Value, typename _Tool, typename... _Tools>
    struct MultiToolHelper<_Value, _Tool, _Tool, _Tools...>
    {
        using Type = typename MultiToolHelper<_Value, _Tool, _Tools...>::Type;
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename _Type>
    struct WrapperSimplifyHelper
    {
        using Type = _Type;
    };

    template <typename _Value, typename _Tool, typename _OtherTool>
    struct WrapperSimplifyHelper<
        ::scl::feature::detail::wrapper< ::scl::feature::detail::wrapper<_Value, _Tool>,
            _OtherTool> >
    {
        using Type = ::scl::feature::detail::wrapper<
            typename WrapperSimplifyHelper< ::scl::feature::detail::wrapper<_Value, _Tool> >::Type,
            _OtherTool>;
    };

    template <typename _Value, typename _Tool>
    struct WrapperSimplifyHelper<
        ::scl::feature::detail::wrapper< ::scl::feature::detail::wrapper<_Value, _Tool>, _Tool> >
    {
        using Type = typename WrapperSimplifyHelper<
            ::scl::feature::detail::wrapper<_Value, _Tool> >::Type;
    };
} // namespace scl::feature::detail

//! This is definition of wrapper type. No tool duplicates are guarantee.
namespace scl::feature
{
    template <typename _Value, typename... _Tools>
    using wrapper = typename ::scl::feature::detail::WrapperSimplifyHelper<
        typename ::scl::feature::detail::MultiToolHelper<_Value, _Tools...>::Type>::Type;
} // namespace scl::feature

// ::scl::feature::wrapper must be defined before includes.
#include "detail/wrapper.h"

#endif
