#pragma once
#ifndef SCL_ACCESS_HOLDER_INTERFACE_H
#define SCL_ACCESS_HOLDER_INTERFACE_H

#include <scl/feature/trait.h>
#include <scl/utility/type_traits.h>

#include <utility>

namespace scl::feature::detail
{
    template <typename _WrapperRefer>
    struct WrapperAccess
    {
        static_assert(::std::is_reference<_WrapperRefer>::value,
            "The template parameter _WrapperRefer must to be a reference type.");

        using WrapperRefer = _WrapperRefer;
        using wrapper = ::std::decay_t<WrapperRefer>;

        static_assert(::scl::feature::isWrapper<wrapper>(),
            "The template parameter _WrapperRefer must to be a wrapper reference type.");

        using Holder = typename wrapper::Holder;
        using HolderRefer = ::scl::SimilarRefer<Holder, WrapperRefer>;

        static constexpr HolderRefer holderRefer(WrapperRefer refer)
        {
            return ::std::forward<HolderRefer>(refer.m_holder);
        }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename _WrapperRefer,
        typename =
            ::std::enable_if_t< ::scl::feature::isWrapper< ::std::decay_t<_WrapperRefer> >()> >
    constexpr ::scl::SimilarRefer<typename ::std::decay_t<_WrapperRefer>::Holder, _WrapperRefer &&>
        wrapperHolder(_WrapperRefer && refer)
    {
        using WrapperRefer = _WrapperRefer &&;
        return ::scl::feature::detail::WrapperAccess<WrapperRefer>::holderRefer(
            ::std::forward<WrapperRefer>(refer));
    }
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename _HolderRefer, bool has_method>
    struct GuardHelper;

    template <typename _HolderRefer>
    struct GuardHelper<_HolderRefer, false>
    {
        static_assert(::std::is_reference<_HolderRefer>::value,
            "The template parameter _HolderRefer type must to be a reference type.");

        static constexpr void guard(_HolderRefer) {}
    };

    template <typename _HolderRefer>
    struct GuardHelper<_HolderRefer, true>
    {
        static_assert(::std::is_reference<_HolderRefer>::value,
            "The template parameter _HolderRefer type must to be a reference type.");
        using Holder = ::std::decay_t<_HolderRefer>;

        static constexpr void guard(_HolderRefer holder)
        {
            Holder::guard(::std::forward<_HolderRefer>(holder));
        }
    };

    template <typename _HolderRefer, bool has_method>
    struct UnguardHelper;

    template <typename _HolderRefer>
    struct UnguardHelper<_HolderRefer, false>
    {
        static_assert(::std::is_reference<_HolderRefer>::value,
            "The template parameter _HolderRefer type must to be a reference type.");

        static constexpr void unguard(_HolderRefer) {}
    };

    template <typename _HolderRefer>
    struct UnguardHelper<_HolderRefer, true>
    {
        static_assert(::std::is_reference<_HolderRefer>::value,
            "The template parameter _HolderRefer type must to be a reference type.");

        static constexpr void unguard(_HolderRefer holder)
        {
            using Holder = ::std::decay_t<_HolderRefer>;
            Holder::unguard(::std::forward<_HolderRefer>(holder));
        }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    struct HolderInterface
    {
        SCL_METHOD_DETECTION(guard)

        template <typename... _Arguments>
        inline static constexpr bool doesGuardStaticMethodExist()
        {
            return has_guard_method_v<_Arguments...>;
        }

        SCL_METHOD_DETECTION(unguard)

        template <typename... _Arguments>
        inline static constexpr bool doesUnguardStaticMethodExist()
        {
            return has_unguard_method_v<_Arguments...>;
        }

        SCL_METHOD_DETECTION(value)

        template <typename... _Arguments>
        inline static constexpr bool doesValueStaticMethodExist()
        {
            return has_value_method_v<_Arguments...>;
        }

        template <typename _HolderRefer,
            typename = ::std::enable_if_t<
                doesValueStaticMethodExist< ::std::decay_t<_HolderRefer>, _HolderRefer>()> >
        static constexpr void guard(_HolderRefer holder)
        {
            using Holder = ::std::decay_t<_HolderRefer>;
            ::scl::feature::detail::GuardHelper<_HolderRefer,
                doesGuardStaticMethodExist<Holder,
                    _HolderRefer>()>::guard(::std::forward<_HolderRefer>(holder));
        }

        template <typename _HolderRefer,
            typename = ::std::enable_if_t<
                doesValueStaticMethodExist< ::std::decay_t<_HolderRefer>, _HolderRefer>()> >
        static constexpr void unguard(_HolderRefer holder)
        {
            using Holder = ::std::decay_t<_HolderRefer>;
            ::scl::feature::detail::UnguardHelper<_HolderRefer,
                doesUnguardStaticMethodExist<Holder,
                    _HolderRefer>()>::unguard(::std::forward<_HolderRefer>(holder));
        }

        template <typename _HolderRefer,
            typename = ::std::enable_if_t<
                doesValueStaticMethodExist< ::std::decay_t<_HolderRefer>, _HolderRefer>()> >
        static constexpr decltype(auto) value(_HolderRefer holder)
        {
            using Holder = ::std::decay_t<_HolderRefer>;
            return Holder::value(::std::forward<_HolderRefer>(holder));
        }
    };
} // namespace scl::feature::detail

#endif
