#pragma once
#ifndef SCL_GUARD_FEATURED_TOOL_H
#define SCL_GUARD_FEATURED_TOOL_H

#include <scl/feature/access/value_guard.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::Guard
{
    template <typename _Wrapper>
    struct Featured
    {
        using WrapperRefer = _Wrapper;

        template <typename _Value>
        struct Holder
        {
            using ThisType = Holder<_Value>;
            using Value = _Value;
            using ValueGuard = ::scl::feature::ValueGuard<WrapperRefer>;

            ValueGuard m_value_guard;

            Holder(WrapperRefer refer)
                : m_value_guard(::std::forward<WrapperRefer>(refer))
            {}

            //! Access to internal value of Holder for any king of referencies.
            template <typename _HolderRefer,
                typename = ::std::enable_if_t<
                    ::std::is_same<ThisType, ::std::decay_t<_HolderRefer> >::value> >
            static constexpr decltype(auto) value(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::scl::SimilarRefer<Value, HolderRefer>;
                return ::std::forward<ValueRefer>(holder.m_value_guard.valueAccess());
            }
        };
    };
} // namespace scl::feature::Guard

#endif
