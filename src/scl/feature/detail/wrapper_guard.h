#pragma once

#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/executor_access.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    enum class wrapper_guard_case : bool
    {
        value = false,
        wrapper = true,
    };

    template <typename Refer,
        wrapper_guard_case Case = ::scl::feature::is_wrapper_v<Refer> ? wrapper_guard_case::wrapper : wrapper_guard_case::value>
        requires ::std::is_reference_v<Refer>
    class wrapper_guard;

    template <typename Refer>
        requires ::std::is_reference_v<Refer>
    class wrapper_guard<Refer, wrapper_guard_case::value>
    {
    public:
        wrapper_guard(wrapper_guard &&) = delete;
        wrapper_guard(wrapper_guard const &) = delete;
        wrapper_guard & operator=(wrapper_guard &&) = delete;
        wrapper_guard & operator=(wrapper_guard const &) = delete;
        ~wrapper_guard() = default;

        constexpr explicit wrapper_guard(Refer v) noexcept
            : m_value{::std::forward<Refer>(v)}
        {}

        [[nodiscard]]
        constexpr decltype(auto) value() const noexcept
        {
            return ::std::forward<Refer>(m_value);
        }

    private:
        Refer m_value;
    };

    template <concepts::wrapper WrapperRefer>
        requires ::std::is_reference_v<WrapperRefer>
    class wrapper_guard<WrapperRefer, wrapper_guard_case::wrapper>
    {
        using wrapper_type = ::std::remove_cvref_t<WrapperRefer>;
        using executor_type = typename wrapper_type::executor_type;
        using executor_refer = scl::forward_like_t<WrapperRefer, executor_type>;

    public:
        wrapper_guard(wrapper_guard &&) = delete;
        wrapper_guard(wrapper_guard const &) = delete;
        wrapper_guard & operator=(wrapper_guard &&) = delete;
        wrapper_guard & operator=(wrapper_guard const &) = delete;

        constexpr explicit wrapper_guard(WrapperRefer w)
            noexcept(::scl::feature::is_guard_noexcept_v<executor_type, executor_refer>)
            : m_executor{executor_access::get(::std::forward<WrapperRefer>(w))}
        {
            if constexpr (::scl::feature::has_guard_v<executor_type, executor_refer>)
                executor_type::guard(::std::forward<executor_refer>(m_executor));
        }

        constexpr ~wrapper_guard() noexcept(::scl::feature::is_unguard_noexcept_v<executor_type, executor_refer>)
        {
            if constexpr (::scl::feature::has_unguard_v<executor_type, executor_refer>)
                executor_type::unguard(::std::forward<executor_refer>(m_executor));
        }

        [[nodiscard]]
        constexpr decltype(auto) value() const noexcept
            requires requires {
                         executor_type::template value<executor_refer>(::std::declval<executor_refer>());
                     }
        {
            return executor_type::template value<executor_refer>(::std::forward<executor_refer>(m_executor));
        }

    private:
        executor_refer m_executor;
    };
} // namespace scl::feature::detail
