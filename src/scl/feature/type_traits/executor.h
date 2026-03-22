#pragma once

/// @file
/// @brief Customization point for locating the executor inside a wrapper type.
///
/// @c executor_trait maps a wrapper type to its executor member.
/// There is no default implementation — specialize for each wrapper type:
/// @code{.cpp}
/// template <>
/// struct scl::feature::executor_trait<MyWrapper>
/// {
///     template <typename Self>
///     static constexpr decltype(auto) executor(Self && self)
///     {
///         return ::scl::forward_like<Self>(self.m_impl);
///     }
/// };
/// @endcode
///
/// @tparam T  The wrapper type (cv-ref stripped).

namespace scl::feature
{
    /// @ingroup scl_feature_type_traits
    /// @brief Trait that provides access to the executor inside a wrapper.
    ///
    /// No default implementation.  Each wrapper type must provide a
    /// specialization with a static @c executor(Self&&) method that
    /// returns a reference to the executor, preserving cv-ref qualifiers.
    ///
    /// @tparam T  The wrapper type (cv-ref stripped).
    template <typename T>
    struct executor_trait;
} // namespace scl::feature
