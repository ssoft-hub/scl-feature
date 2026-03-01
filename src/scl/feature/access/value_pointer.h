#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_POINTER_H
#define SCL_FEATURE_ACCESS_VALUE_POINTER_H

#include "value_guard.h"

#include <scl/utility/type_traits/detection/operator.h>

namespace scl::feature
{
    /*!
     * Указатель на экземпляр вложенного в wrapper базового значения, к которому применены
     * все особенности, реализуемые посредством используемых wrapper.
     */
    template <typename _WrapperRefer>
    class ValuePointer
    {
        template <typename, typename>
        friend class ::scl::feature::detail::wrapper;
        using ThisType = ValuePointer<_WrapperRefer>;

    public:
        using WrapperRefer = _WrapperRefer;
        using ValueGuard = ::scl::feature::ValueGuard<WrapperRefer>;
        using PointerAccess = typename ValueGuard::PointerAccess;
        using ValueAccess = typename ValueGuard::ValueAccess;

    private:
        ValueGuard m_value_guard;

    private:
        ValuePointer(WrapperRefer refer)
            : m_value_guard(::std::forward<WrapperRefer>(refer))
        {}

    public:
        ValuePointer(ThisType && other)
            : m_value_guard(::std::forward<ThisType &&>(other).m_value_guard)
        {}

        ValuePointer(ThisType const &) = delete;
        void operator=(ThisType const &) = delete;

        ValueAccess operator*() const { return m_value_guard.valueAccess(); }

        decltype(auto) operator->() const
        {
            if constexpr (::scl::has_indirection_v<ValueAccess>)
                return m_value_guard.valueAccess();
            else
                return m_value_guard.pointerAccess();
        }
    };
} // namespace scl::feature

#endif
