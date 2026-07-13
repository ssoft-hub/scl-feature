#pragma once

#include <scl/feature/type_traits/executor.h>

#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    /// @internal
    /// @brief RAII guard around a single executor, exposing its held value.
    ///
    /// Acquires the executor's guard (if it has one) on construction and releases
    /// it on destruction, and exposes the wrapped value through @c value()
    /// (a single level of unwrapping via @c ExecutorType::access).
    ///
    /// Unlike @c wrapper_guard, this helper operates on the **executor** directly
    /// rather than on a @c detail::wrapper.  Reflection is used by arbitrary user
    /// types (via @c SCL_REFLECT_TYPE + @c executor_trait), not only by
    /// @c detail::wrapper, so guarding must go through the executor interface —
    /// @c wrapper_guard would treat a non-@c detail::wrapper reflection type as a
    /// leaf and return the object itself instead of its value.
    ///
    /// @tparam ExecutorRefer  cv-ref-qualified reference type of the executor.
    template <typename ExecutorRefer>
    class executor_guard
    {
        using executor_type = ::std::remove_cvref_t<ExecutorRefer>;

    public:
        executor_guard(executor_guard &&) = delete;
        executor_guard(executor_guard const &) = delete;
        executor_guard & operator=(executor_guard &&) = delete;
        executor_guard & operator=(executor_guard const &) = delete;

        constexpr explicit executor_guard(ExecutorRefer exec) /**/
            noexcept(::scl::feature::is_guard_noexcept_v<executor_type, ExecutorRefer>)
            : m_exec{::std::forward<ExecutorRefer>(exec)}
        {
            if constexpr (::scl::feature::has_guard_v<executor_type, ExecutorRefer>)
                executor_type::guard(::std::forward<ExecutorRefer>(m_exec));
        }

        constexpr ~executor_guard() /**/
            noexcept(::scl::feature::is_unguard_noexcept_v<executor_type, ExecutorRefer>)
        {
            if constexpr (::scl::feature::has_unguard_v<executor_type, ExecutorRefer>)
                executor_type::unguard(::std::forward<ExecutorRefer>(m_exec));
        }

        /// Returns the wrapped value one level down, through the executor.
        /// The guard is held for the lifetime of @c *this, so the caller must
        /// consume the result before this guard is destroyed.
        [[nodiscard]]
        constexpr decltype(auto) value() const /**/
            noexcept(noexcept(executor_type::access(::std::declval<ExecutorRefer>())))
        {
            return executor_type::access(::std::forward<ExecutorRefer>(m_exec));
        }

    private:
        ExecutorRefer m_exec;
    };

} // namespace scl::feature::detail
