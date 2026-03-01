#pragma once
#ifndef SCL_FEATURE_WRAPPER_GUARD_H
#define SCL_FEATURE_WRAPPER_GUARD_H

#include "detail/holder_interface.h"

#include <scl/utility/type_traits.h>

#include <utility>

namespace scl::feature::detail
{
    template <typename _Value, typename _Tool>
    class wrapper;

    template <typename _Refer>
    struct WrapperGuardHelper;
} // namespace scl::feature::detail

namespace scl::feature
{
    /*!
     * @brief This type activates a feature for the wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template <typename _Refer>
    using WrapperGuard = typename ::scl::feature::detail::WrapperGuardHelper<_Refer>::Type;
} // namespace scl::feature

namespace scl::feature::detail
{
    /*!
     * @brief This is the WrapperGuard specialization for non wrapped value.
     * It does nothing around a value.
     */
    template <typename _Refer>
    class WrapperGuardForNonWrapped
    {
        using ThisType = WrapperGuardForNonWrapped<_Refer>;

    public:
        using Refer = _Refer;
        using Pointer = ::std::add_pointer_t< ::std::remove_reference_t<Refer> >;

        using ValueAccess = Refer;
        using PointerAccess = Pointer;

        static_assert(::std::is_reference<Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(!::scl::feature::isWrapper< ::std::decay_t<Refer> >(),
            "The template parameter _Refer must to be a not wrapper type reference!");

    private:
        Refer m_refer;

    private:
        WrapperGuardForNonWrapped(ThisType && other) = delete;
        WrapperGuardForNonWrapped(ThisType const & other) = delete;

    public:
        constexpr WrapperGuardForNonWrapped(Refer refer)
            : m_refer(::std::forward<Refer>(refer))
        {}

        constexpr ValueAccess valueAccess() const { return ::std::forward<ValueAccess>(m_refer); }

        constexpr PointerAccess pointerAccess() const { return ::std::addressof(m_refer); }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * @brief This is the WrapperGuard specialization for wrapped value.
     * It activates a feature in the constructor and deactivate it in the destructor.
     */
    template <typename _Refer>
    class WrapperGuardForWrapped
    {
        using ThisType = WrapperGuardForWrapped<_Refer>;

    public:
        using WrapperRefer = _Refer;
        using wrapper = ::std::decay_t<WrapperRefer>;
        using Value = typename wrapper::Value;
        using ValueRefer = ::scl::SimilarRefer<Value, WrapperRefer>;
        using Holder = typename wrapper::Holder;
        using HolderRefer = ::scl::SimilarRefer<Holder, WrapperRefer>;

        // using ReferPointer = ::scl::feature::detail::ReferPointer< WrapperRefer >;

        using WrapperAccess = ValueRefer;
        using HolderAccess = HolderRefer;

        static_assert(::std::is_reference<WrapperRefer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(::scl::feature::isWrapper<wrapper>(),
            "The template parameter _Refer must to be a wrapper type reference!");
        // static_assert( ::scl::feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and
        // ValueRefer must to be similar types!" );
        static_assert(::scl::feature::isSimilar<HolderRefer, WrapperRefer>(),
            "The Refer and HolderRefer must to be similar types!");

    private:
        WrapperRefer m_refer;

    private:
        WrapperGuardForWrapped(ThisType const & other) = delete;
        WrapperGuardForWrapped(ThisType && other) = delete;

    public:
        constexpr WrapperGuardForWrapped(WrapperRefer refer) noexcept
            : m_refer(::std::forward<WrapperRefer>(refer))
        {
            static_assert(
                ::scl::feature::detail::HolderInterface::doesValueStaticMethodExist<Holder,
                    HolderRefer>(),
                "There are no appropriate access methods for Holder.");
            ::scl::feature::detail::HolderInterface::guard<HolderRefer>(holderAccess());
        }

        ~WrapperGuardForWrapped()
        {
            ::scl::feature::detail::HolderInterface::unguard<HolderRefer>(holderAccess());
        }

        constexpr WrapperAccess wrapperAccess() const noexcept
        {
            return ::scl::feature::detail::HolderInterface::value<HolderRefer>(holderAccess());
        }

        constexpr HolderAccess holderAccess() const noexcept
        {
            return ::scl::feature::detail::wrapperHolder<WrapperRefer>(
                ::std::forward<WrapperRefer>(m_refer));
        }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template <typename _Refer>
    using WrapperGuardSwitchCase = ::std::conditional_t<
        ::scl::feature::isWrapper< ::std::remove_reference_t<_Refer> >(),
        WrappedCase,
        NonWrappedCase>;

    template <typename, typename>
    struct WrapperGuardSwitch;

    template <typename _Refer>
    struct WrapperGuardSwitch<NonWrappedCase, _Refer>
    {
        using Type = ::scl::feature::detail::WrapperGuardForNonWrapped<_Refer>;
    };

    template <typename _Refer>
    struct WrapperGuardSwitch<WrappedCase, _Refer>
    {
        using Type = ::scl::feature::detail::WrapperGuardForWrapped<_Refer>;
    };

    template <typename _Refer>
    struct WrapperGuardHelper
    {
        static_assert(::std::is_reference<_Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        using Type = typename WrapperGuardSwitch<WrapperGuardSwitchCase<_Refer>, _Refer>::Type;
    };
} // namespace scl::feature::detail

#endif
