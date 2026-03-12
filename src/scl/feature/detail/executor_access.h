#pragma once

#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>

namespace scl::feature::detail
{
    /// @internal
    /// @brief Single point of access to the private `m_executor` member of `wrapper`.
    ///
    /// Declared as `friend` in `wrapper`, so only this struct can reach the executor.
    struct executor_access
    {
        /// @brief Returns the executor of @p w, preserving cv-ref qualifiers.
        // NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
        template <typename WrapperRefer>
            requires ::scl::feature::is_wrapper_v<::std::remove_cvref_t<WrapperRefer>>
        static constexpr decltype(auto) get(WrapperRefer && w) noexcept
        {
            return ::scl::forward_like<WrapperRefer>(w.m_executor);
        }
        // NOLINTEND(cppcoreguidelines-missing-std-forward)
    };
} // namespace scl::feature::detail
