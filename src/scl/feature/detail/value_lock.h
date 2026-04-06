#pragma once

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/wrapper_lock.h>
#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::detail
{
    enum class value_lock_case : bool
    {
        value = false,
        wrapper = true,
    };

    template <typename Refer,
        value_lock_case Case = ::scl::feature::is_wrapper_v<Refer> ? value_lock_case::wrapper : value_lock_case::value>
        requires ::std::is_reference_v<Refer>
    class value_lock;

    // -------------------------------------------------------------------------
    // Non-wrapper specialisation — no-op lock, just holds the reference.
    // -------------------------------------------------------------------------

    template <typename Refer>
        requires ::std::is_reference_v<Refer>
    class value_lock<Refer, value_lock_case::value>
    {
    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;
        ~value_lock() = default;

        constexpr explicit value_lock(Refer ref) noexcept
            : m_ref{::std::forward<Refer>(ref)}
        {}

        template <typename Target>
        constexpr void lock_for() noexcept
            requires ::std::convertible_to<Refer, Target>
        {}

        template <typename Target>
        [[nodiscard]]
        constexpr Target value_as() noexcept(::std::is_nothrow_convertible_v<Refer, Target>)
            requires ::std::convertible_to<Refer, Target>
        {
            return ::std::forward<Refer>(m_ref);
        }

    private:
        Refer m_ref;
    };

    // -------------------------------------------------------------------------
    // Wrapper specialisation — recursive lazy lock through the wrapper chain.
    // -------------------------------------------------------------------------

    template <concepts::wrapper WrapperRefer>
        requires ::std::is_reference_v<WrapperRefer>
    class value_lock<WrapperRefer, value_lock_case::wrapper>
    {
        using wrapper_type = ::std::remove_cvref_t<WrapperRefer>;
        using value_type = typename wrapper_type::value_type;
        using value_refer = ::scl::forward_like_t<WrapperRefer, value_type>;

        using outer_lock_type = ::scl::feature::detail::wrapper_lock<WrapperRefer, wrapper_lock_case::wrapper>;
        using inner_lock_type = ::scl::feature::detail::value_lock<value_refer,
            ::scl::feature::is_wrapper_v<value_refer> ? value_lock_case::wrapper : value_lock_case::value>;

    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;

        constexpr explicit value_lock(WrapperRefer ref) noexcept(construct_noexcept())
            : m_outer_lock{::std::forward<WrapperRefer>(ref)}
            , m_inner_lock{m_outer_lock.value()}
        {}

        constexpr ~value_lock() = default;

        /// Activates guards for all wrapper layers needed to reach @p Target.
        /// If @p Target is @p WrapperRefer itself no guard is acquired.
        template <typename Target>
        constexpr void lock_for() noexcept(lock_for_noexcept<Target>())
            requires ::scl::feature::concepts::convertible_from<Target, WrapperRefer>
        {
            if constexpr (!::std::is_convertible_v<WrapperRefer, Target>)
            {
                m_outer_lock.lock();
                m_inner_lock.template lock_for<Target>();
            }
        }

        /// Returns the value at the level matching @p Target (lock_for should be held by the caller).
        template <typename Target>
        [[nodiscard]]
        constexpr Target value_as() noexcept(value_as_noexcept<Target>())
            requires ::scl::feature::concepts::convertible_from<Target, WrapperRefer>
        {
            if constexpr (!::std::is_convertible_v<WrapperRefer, Target>)
                return m_inner_lock.template value_as<Target>();
            else
                return m_outer_lock.wrapper_value();
        }

    private:
        static constexpr bool construct_noexcept() noexcept
        {
            return noexcept(outer_lock_type{::std::declval<WrapperRefer>()}) &&
                noexcept(::std::declval<outer_lock_type &>().value()) &&
                noexcept(inner_lock_type{::std::declval<outer_lock_type &>().value()});
        }

        template <typename Target>
        static constexpr bool lock_for_noexcept() noexcept
        {
            if constexpr (::std::is_convertible_v<WrapperRefer, Target>)
                return true;
            else
                return noexcept(::std::declval<outer_lock_type &>().lock()) &&
                    noexcept(::std::declval<inner_lock_type &>().template lock_for<Target>());
        }

        template <typename Target>
        static constexpr bool value_as_noexcept() noexcept
        {
            if constexpr (::std::is_convertible_v<WrapperRefer, Target>)
                return noexcept(::std::declval<outer_lock_type &>().wrapper_value());
            else
                return noexcept(::std::declval<inner_lock_type &>().template value_as<Target>());
        }

        outer_lock_type m_outer_lock;
        inner_lock_type m_inner_lock;
    };

} // namespace scl::feature::detail
