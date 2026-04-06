#pragma once

#include <scl/feature/concepts/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

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
        template <typename Wrapper>
        static constexpr decltype(auto) get(Wrapper && w) noexcept
            requires ::scl::feature::concepts::wrapper<Wrapper>
        {
            return ::scl::forward_like<Wrapper>(w.m_executor);
        }
        // NOLINTEND(cppcoreguidelines-missing-std-forward)
    };
} // namespace scl::feature::detail
