#pragma once

#include <scl/feature/detail/executor_access.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    enum class wrapper_lock_case : bool
    {
        value = false,
        wrapper = true,
    };

    template <typename Refer, wrapper_lock_case Case>
        requires ::std::is_reference_v<Refer>
    class wrapper_lock;

    // -------------------------------------------------------------------------
    // Non-wrapper specialisation — no-op lock, just holds the reference.
    // -------------------------------------------------------------------------

    template <typename Refer>
        requires ::std::is_reference_v<Refer>
    class wrapper_lock<Refer, wrapper_lock_case::value>
    {
    public:
        wrapper_lock(wrapper_lock &&) = delete;
        wrapper_lock(wrapper_lock const &) = delete;
        wrapper_lock & operator=(wrapper_lock &&) = delete;
        wrapper_lock & operator=(wrapper_lock const &) = delete;
        ~wrapper_lock() = default;

        constexpr explicit wrapper_lock(Refer ref) noexcept
            : m_ref{::std::forward<Refer>(ref)}
        {}

        constexpr void lock() noexcept {}

        constexpr void unlock() noexcept {}

        /// Returns the held reference (identity).
        constexpr Refer value() const noexcept { return ::std::forward<Refer>(m_ref); }

    private:
        Refer m_ref;
    };

    // -------------------------------------------------------------------------
    // Wrapper specialisation — lazy guard: lock()/unlock() call guard()/unguard().
    // -------------------------------------------------------------------------

    template <typename WrapperRefer>
        requires ::std::is_reference_v<WrapperRefer> &&
        ::scl::feature::is_wrapper_v<::std::remove_cvref_t<WrapperRefer>>
    class wrapper_lock<WrapperRefer, wrapper_lock_case::wrapper>
    {
        using wrapper_type = ::std::remove_cvref_t<WrapperRefer>;
        using executor_type = typename wrapper_type::executor_type;
        using executor_refer = ::scl::forward_like_t<WrapperRefer, executor_type>;
        using value_type = typename wrapper_type::value_type;
        using value_refer = ::scl::forward_like_t<WrapperRefer, value_type>;

    public:
        wrapper_lock(wrapper_lock &&) = delete;
        wrapper_lock(wrapper_lock const &) = delete;
        wrapper_lock & operator=(wrapper_lock &&) = delete;
        wrapper_lock & operator=(wrapper_lock const &) = delete;

        constexpr explicit wrapper_lock(WrapperRefer ref) noexcept
            : m_ref{::std::forward<WrapperRefer>(ref)}
        {}

        constexpr ~wrapper_lock() { unlock(); }

        constexpr void lock()
        {
            if (!::std::exchange(m_locked, true))
                if constexpr (requires { executor_type::template guard<executor_refer>(executor()); })
                    executor_type::template guard<executor_refer>(executor());
        }

        constexpr void unlock()
        {
            if (::std::exchange(m_locked, false))
                if constexpr (
                    requires { executor_type::template unguard<executor_refer>(executor()); })
                    executor_type::template unguard<executor_refer>(executor());
        }

        /// Returns the wrapper reference itself (does not require the lock).
        constexpr WrapperRefer wrapper_value() const noexcept
        {
            return ::std::forward<WrapperRefer>(m_ref);
        }

        /// Returns the inner value through the executor (lock should be held by the caller).
        constexpr value_refer value() const noexcept(noexcept(
            executor_type::template value<executor_refer>(::std::declval<executor_refer>())))
            requires requires {
                         executor_type::template value<executor_refer>(::std::declval<executor_refer>());
                     }
        {
            return executor_type::template value<executor_refer>(executor());
        }

    private:
        constexpr executor_refer executor() const noexcept
        {
            return executor_access::get(::std::forward<WrapperRefer>(m_ref));
        }

        bool m_locked{false};
        WrapperRefer m_ref;
    };

} // namespace scl::feature::detail
