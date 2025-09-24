#pragma once
#ifndef SCL_FEATURE_WRAPPER_LOCK_H
#define SCL_FEATURE_WRAPPER_LOCK_H

#include "Detail/HolderInterface.h"

#include <ScL/Utility/SimilarRefer.h>

#include <cassert>
#include <utility>

namespace ScL::Feature::Detail
{
    template <typename _Value, typename _Tool>
    class Wrapper;
    template <typename _Refer>
    struct WrapperLockHelper;
} // namespace ScL::Feature::Detail

namespace ScL::Feature
{
    /*!
     * @brief This type activates a feature for the wrapped value when
     * the lock method is called, and deactivates a feature when
     * the unlock method is called or it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template <typename _Refer>
    using WrapperLock = typename ::ScL::Feature::Detail::WrapperLockHelper<_Refer>::Type;
} // namespace ScL::Feature

namespace ScL::Feature::Detail
{
    /*!
     * @brief This is the WrapperLock specialization for non wrapped value.
     * It does nothing around a value.
     */
    template <typename _Refer>
    class WrapperLockForNonWrapped
    {
        using ThisType = WrapperLockForNonWrapped<_Refer>;

    public:
        using Refer = _Refer;
        using Pointer = ::std::add_pointer_t< ::std::remove_reference_t<Refer> >;

        using ValueAccess = Refer;
        using PointerAccess = Pointer;

        static_assert(::std::is_reference<Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(!::ScL::Feature::isWrapper< ::std::decay_t<Refer> >(),
            "The template parameter _Refer must to be a not Wrapper type reference!");

    private:
        Refer m_refer;

    private:
        WrapperLockForNonWrapped(ThisType && other) = delete;
        WrapperLockForNonWrapped(ThisType const & other) = delete;

    public:
        constexpr WrapperLockForNonWrapped(Refer refer)
            : m_refer(::std::forward<Refer>(refer))
        {}

        constexpr void lock() {}

        constexpr void unlock() {}

        constexpr ValueAccess valueAccess() const { return ::std::forward<ValueAccess>(m_refer); }

        constexpr PointerAccess pointerAccess() const { return ::std::addressof(m_refer); }
    };

} // namespace ScL::Feature::Detail

namespace ScL::Feature::Detail
{

    /*!
     * @brief This is the WrapperLock specialization for wrapped value.
     * It activates a feature in the constructor and deactivate it in the destructor.
     */
    template <typename _Refer>
    class WrapperLockForWrapped
    {
        using ThisType = WrapperLockForWrapped<_Refer>;

    public:
        using WrapperRefer = _Refer;
        using Wrapper = ::std::decay_t<WrapperRefer>;
        using Value = typename Wrapper::Value;
        using ValueRefer = ::ScL::SimilarRefer<Value, WrapperRefer>;
        using Holder = typename Wrapper::Holder;
        using HolderRefer = ::ScL::SimilarRefer<Holder, WrapperRefer>;

        using WrapperAccess = ValueRefer;
        using HolderAccess = HolderRefer;

        static_assert(::std::is_reference<WrapperRefer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(::ScL::Feature::isWrapper<Wrapper>(),
            "The template parameter _Refer must to be a Wrapper type reference!");
        // static_assert( ::ScL::Feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and
        // ValueRefer must to be similar types!" );
        static_assert(::ScL::Feature::isSimilar<HolderRefer, WrapperRefer>(),
            "The Refer and HolderRefer must to be similar types!");

    private:
        bool m_is_locked{false};
        WrapperRefer m_refer;

    private:
        WrapperLockForWrapped(ThisType const & other) = delete;
        WrapperLockForWrapped(ThisType && other) = delete;

    public:
        constexpr WrapperLockForWrapped(WrapperRefer refer) noexcept
            : m_refer(::std::forward<WrapperRefer>(refer))
        {}

        ~WrapperLockForWrapped() { unlock(); }

        constexpr void lock()
        {
            if (!::std::exchange(m_is_locked, true))
                ::ScL::Feature::Detail::HolderInterface::guard<HolderRefer>(holderAccess());
        }

        constexpr void unlock()
        {
            if (::std::exchange(m_is_locked, false))
                ::ScL::Feature::Detail::HolderInterface::unguard<HolderRefer>(holderAccess());
        }

        constexpr WrapperAccess wrapperAccess() const noexcept
        {
            return ::ScL::Feature::Detail::HolderInterface::value<HolderRefer>(holderAccess());
        }

        constexpr HolderAccess holderAccess() const noexcept
        {
            return ::ScL::Feature::Detail::wrapperHolder<WrapperRefer>(
                ::std::forward<WrapperRefer>(m_refer));
        }

        constexpr WrapperRefer valueAccess() const noexcept
        {
            return ::std::forward<WrapperRefer>(m_refer);
        }
    };
} // namespace ScL::Feature::Detail

namespace ScL::Feature::Detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template <typename _Refer>
    using WrapperLockSwitchCase = ::std::conditional_t<
        ::ScL::Feature::isWrapper< ::std::remove_reference_t<_Refer> >(),
        WrappedCase,
        NonWrappedCase>;

    template <typename, typename>
    struct WrapperLockSwitch;

    template <typename _Refer>
    struct WrapperLockSwitch<NonWrappedCase, _Refer>
    {
        using Type = ::ScL::Feature::Detail::WrapperLockForNonWrapped<_Refer>;
    };

    template <typename _Refer>
    struct WrapperLockSwitch<WrappedCase, _Refer>
    {
        using Type = ::ScL::Feature::Detail::WrapperLockForWrapped<_Refer>;
    };

    template <typename _Refer>
    struct WrapperLockHelper
    {
        static_assert(::std::is_reference<_Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        using Type = typename WrapperLockSwitch<WrapperLockSwitchCase<_Refer>, _Refer>::Type;
    };
} // namespace ScL::Feature::Detail

#endif
