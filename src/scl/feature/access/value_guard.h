#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_GUARD_H
#define SCL_FEATURE_ACCESS_VALUE_GUARD_H

#include <scl/feature/access/wrapper_guard.h>
#include <scl/feature/trait.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::detail
{
    template <typename _Value, typename _Tool>
    class wrapper;
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename>
    struct ValueGuardHelper;
} // namespace scl::feature::detail

namespace scl::feature
{
    /*!
     * @brief This type activates all features for the multi wrapped value when
     * it is constructed, and deactivates a feature when it is destroyed.
     * This type does nothing for non wrapped values.
     */
    template <typename _Refer>
    using ValueGuard = typename ::scl::feature::detail::ValueGuardHelper<_Refer>::Type;
} // namespace scl::feature

namespace scl::feature::detail
{
    /*!
     * @brief This is the WrapperGuard specialization for non wrapped value.
     * It does nothing around a value.
     */
    template <typename _Refer>
    class ValueGuardForNonWrapped
    {
        using ThisType = ValueGuardForNonWrapped<_Refer>;

    public:
        using Refer = _Refer;
        using WrapperGuard = ::scl::feature::WrapperGuard<_Refer>;

        using ValueAccess = typename WrapperGuard::ValueAccess;
        using PointerAccess = typename WrapperGuard::PointerAccess;

        static_assert(::std::is_reference<Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(!::scl::feature::isWrapper< ::std::decay_t<Refer> >(),
            "The template parameter _Refer must to be a not wrapper type reference!");

    private:
        WrapperGuard m_wrapper_guard;

    private:
        ValueGuardForNonWrapped(ThisType && other) = delete;
        ValueGuardForNonWrapped(ThisType const & other) = delete;

    public:
        constexpr ValueGuardForNonWrapped(Refer refer)
            : m_wrapper_guard(::std::forward<Refer>(refer))
        {}

        constexpr ValueGuardForNonWrapped(WrapperGuard && other)
            : m_wrapper_guard(::std::forward<WrapperGuard>(other))
        {}

        constexpr ValueAccess valueAccess() const { return m_wrapper_guard.valueAccess(); }

        constexpr PointerAccess pointerAccess() const { return m_wrapper_guard.pointerAccess(); }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * @brief This is the ValueGuard specialization for multi wrapped value.
     * It activates all features in the constructor and deactivate them in the destructor.
     */
    template <typename _Refer>
    class ValueGuardForWrapped
    {
        using ThisType = ValueGuardForWrapped<_Refer>;

    public:
        using WrapperRefer = _Refer;
        using wrapper = ::std::decay_t<WrapperRefer>;
        using Value = typename wrapper::Value;
        using ValueRefer = ::scl::SimilarRefer<Value, WrapperRefer>;
        using Holder = typename wrapper::Holder;
        using HolderRefer = ::scl::SimilarRefer<Holder, WrapperRefer>;

        using WrapperGuard = ::scl::feature::WrapperGuard<WrapperRefer>;
        using ValueGuard = ::scl::feature::ValueGuard<ValueRefer>;

        using WrapperAccess = typename WrapperGuard::WrapperAccess;
        using ValueAccess = typename ValueGuard::ValueAccess;
        using PointerAccess = typename ValueGuard::PointerAccess;

        static_assert(::std::is_reference<WrapperRefer>::value,
            "The template parameter _Refer must to be a reference type.");
        static_assert(::scl::feature::isWrapper<wrapper>(),
            "The template parameter _Refer must to be a wrapper type reference!");
        // static_assert( ::scl::feature::isSimilar< ValueRefer, WrapperRefer >(), "The Refer and
        // ValueRefer must to be similar types!" );
        static_assert(::scl::feature::isSimilar<HolderRefer, WrapperRefer>(),
            "The Refer and HolderRefer must to be similar types!");

    private:
        WrapperGuard m_wrapper_guard;
        ValueGuard m_value_guard;

    private:
        ValueGuardForWrapped(ThisType && other) = delete;
        ValueGuardForWrapped(ThisType const & other) = delete;

    public:
        constexpr ValueGuardForWrapped(WrapperRefer refer)
            : m_wrapper_guard(::std::forward<WrapperRefer>(refer))
            , m_value_guard(::scl::feature::detail::HolderInterface::value<HolderRefer>(
                  m_wrapper_guard.holderAccess()))
        {}

        constexpr ValueGuardForWrapped(WrapperGuard && other)
            : m_wrapper_guard(::std::forward<WrapperGuard>(other))
            , m_value_guard(::scl::feature::detail::HolderInterface::value<HolderRefer>(
                  m_wrapper_guard.holderAccess()))
        {}

        constexpr WrapperAccess wrapperAccess() const { return m_wrapper_guard.wrapperAccess(); }

        constexpr ValueAccess valueAccess() const { return m_value_guard.valueAccess(); }

        constexpr PointerAccess pointerAccess() const { return m_value_guard.pointerAccess(); }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    // Case tags
    struct NonWrappedCase;
    struct WrappedCase;

    // Choice of a case
    template <typename _Refer>
    using ValueGuardSwitchCase = ::std::conditional_t<
        ::scl::feature::isWrapper< ::std::remove_reference_t<_Refer> >(),
        WrappedCase,
        NonWrappedCase>;

    template <typename, typename>
    struct ValueGuardSwitch;

    template <typename _Refer>
    struct ValueGuardSwitch<NonWrappedCase, _Refer>
    {
        using Type = ::scl::feature::detail::ValueGuardForNonWrapped<_Refer>;
    };

    template <typename _Refer>
    struct ValueGuardSwitch<WrappedCase, _Refer>
    {
        using Type = ::scl::feature::detail::ValueGuardForWrapped<_Refer>;
    };

    template <typename _Refer>
    struct ValueGuardHelper
    {
        static_assert(::std::is_reference<_Refer>::value,
            "The template parameter _Refer must to be a reference type.");
        using Type = typename ValueGuardSwitch<ValueGuardSwitchCase<_Refer>, _Refer>::Type;
    };
} // namespace scl::feature::detail

#endif
