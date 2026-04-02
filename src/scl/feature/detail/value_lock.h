#pragma once

#include <scl/feature/detail/wrapper_lock.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>

namespace scl::feature::detail
{
    enum class value_lock_case : bool
    {
        value = false,
        wrapper = true,
    };

    template <typename Refer, value_lock_case Case>
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
            requires ::std::is_same_v<Target, Refer>
        constexpr void lock_for() noexcept
        {}

        template <typename Target>
            requires ::std::is_same_v<Target, Refer>
        constexpr Target value_as() const noexcept
        {
            return ::std::forward<Refer>(m_ref);
        }

    private:
        Refer m_ref;
    };

    // -------------------------------------------------------------------------
    // Wrapper specialisation — recursive lazy lock through the wrapper chain.
    // -------------------------------------------------------------------------

    template <typename WrapperRefer>
        requires ::std::is_reference_v<WrapperRefer> &&
        ::scl::feature::is_wrapper_v<::std::remove_cvref_t<WrapperRefer>>
    class value_lock<WrapperRefer, value_lock_case::wrapper>
    {
        using wrapper_type = ::std::remove_cvref_t<WrapperRefer>;
        using value_type = typename wrapper_type::value_type;
        using value_refer = ::scl::forward_like_t<WrapperRefer, value_type>;

        using outer_lock_type = ::scl::feature::detail::wrapper_lock<WrapperRefer, wrapper_lock_case::wrapper>;
        using inner_lock_type = ::scl::feature::detail::value_lock<value_refer,
            ::scl::feature::is_wrapper_v<::std::remove_cvref_t<value_refer>> ? value_lock_case::wrapper : value_lock_case::value>;

    public:
        value_lock(value_lock &&) = delete;
        value_lock(value_lock const &) = delete;
        value_lock & operator=(value_lock &&) = delete;
        value_lock & operator=(value_lock const &) = delete;

        constexpr explicit value_lock(WrapperRefer ref)
            : m_outer_lock{::std::forward<WrapperRefer>(ref)}
            , m_inner_lock{m_outer_lock.value()}
        {}

        constexpr ~value_lock() = default;

        /// Activates guards for all wrapper layers needed to reach @p Target.
        /// If @p Target is @p WrapperRefer itself no guard is acquired.
        template <typename Target>
        constexpr void lock_for()
        {
            if constexpr (!::std::is_same_v<WrapperRefer, Target>)
            {
                m_outer_lock.lock();
                m_inner_lock.template lock_for<Target>();
            }
        }

        /// Returns the value at the level matching @p Target.
        template <typename Target>
        constexpr Target value_as() const
        {
            if constexpr (::std::is_same_v<WrapperRefer, Target>)
                return m_outer_lock.wrapper_value();
            else
                return m_inner_lock.template value_as<Target>();
        }

    private:
        outer_lock_type m_outer_lock;
        inner_lock_type m_inner_lock;
    };

} // namespace scl::feature::detail
