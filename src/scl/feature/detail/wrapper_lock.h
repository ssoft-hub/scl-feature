#pragma once

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/executor_access.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/concepts/reference.h>
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

    template <::scl::concepts::reference Refer,
        wrapper_lock_case Case = ::scl::feature::is_wrapper_v<Refer> ? wrapper_lock_case::wrapper : wrapper_lock_case::value>
    class wrapper_lock;

    template <::scl::concepts::reference Refer>
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

        static constexpr void lock() noexcept {}

        static constexpr void unlock() noexcept {}

        [[nodiscard]]
        constexpr Refer value() const noexcept
        {
            return ::std::forward<Refer>(m_ref);
        }

    private:
        Refer m_ref;
    };

    template <concepts::wrapper_reference WrapperRefer>
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

        constexpr ~wrapper_lock() noexcept(::scl::feature::is_unguard_noexcept_v<executor_type, executor_refer>)
        {
            unlock();
        }

        constexpr void lock() /**/
            noexcept(::scl::feature::is_guard_noexcept_v<executor_type, executor_refer>)
        {
            if (!m_locked)
            {
                if constexpr (::scl::feature::has_guard_v<executor_type, executor_refer>)
                    executor_type::guard(executor());
                m_locked = true;
            }
        }

        constexpr void unlock() /**/
            noexcept(::scl::feature::is_unguard_noexcept_v<executor_type, executor_refer>)
        {
            if (m_locked)
            {
                if constexpr (::scl::feature::has_unguard_v<executor_type, executor_refer>)
                    executor_type::unguard(executor());
                m_locked = false;
            }
        }

        [[nodiscard]]
        constexpr WrapperRefer wrapper_value() const noexcept
        {
            return ::std::forward<WrapperRefer>(m_ref);
        }

        [[nodiscard]]
        constexpr value_refer value() const /**/
            noexcept(noexcept(executor_type::template access<executor_refer>(::std::declval<executor_refer>())))
            requires requires {
                         executor_type::template access<executor_refer>(::std::declval<executor_refer>());
                     }
        {
            return executor_type::template access<executor_refer>(executor());
        }

    private:
        [[nodiscard]]
        constexpr executor_refer executor() const noexcept
        {
            return executor_access::get(::std::forward<WrapperRefer>(m_ref));
        }

        bool m_locked{false};
        WrapperRefer m_ref;
    };

} // namespace scl::feature::detail
