#pragma once

#include <scl/feature/concepts/executor.h>
#include <scl/feature/concepts/wrapper.h>
#include <scl/feature/detail/executor_access.h>
#include <scl/feature/detail/wrapper_constructor_resolver.h>
#include <scl/feature/detail/wrapper_constructors.h>
#include <scl/feature/reflection/reflect.h>
#include <scl/utility/attribute.h>

#include <utility>

namespace scl::feature::detail
{
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-c-copy-assignment-signature)
    template <typename Value, template <typename> class Executor>
        requires ::scl::feature::concepts::executor<Executor<Value>>
    class wrapper
        : public ::scl::feature::reflect<wrapper<Value, Executor>, Executor<Value>, wrapper<Value, Executor>>
        , public ::scl::feature::reflect_operators<wrapper<Value, Executor>, Executor<Value>>
    {
        using self_type = wrapper<Value, Executor>;

    public:
        using value_type = Value;
        using executor_type = Executor<value_type>;

    private:
        SCL_NO_UNIQUE_ADDRESS
        executor_type m_executor;

        friend struct executor_access;

    public:
        /// Direct construction: forwards all arguments to the executor.
        template <typename... Args>
        constexpr explicit wrapper(Args &&... args)
            : m_executor{::std::forward<Args>(args)...}
        {}

        // clang-format off
        SCL_REFLECT_TYPE(self_type, executor_type)
        
        // cppcheck-suppress noExplicitConstructor
        SCL_WRAPPER_CONSTRUCTOR_FOR_SELF // NOLINT(performance-noexcept-move-constructor)
        // cppcheck-suppress noExplicitConstructor
        SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER

        // Assignment forwards to the reflected operator= in reflect_operators: the operand overload
        // for any non-self source, plus explicit self-type overloads that suppress the deleted
        // implicit copy/move assignment.
        SCL_WRAPPER_ASSIGNMENT_FOR_SELF
        SCL_WRAPPER_ASSIGNMENT_FOR_OPERAND
        // clang-format on
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-c-copy-assignment-signature)
} // namespace scl::feature::detail

/// @brief @c executor_trait specialization for @c detail::wrapper.
///
/// Provides the executor accessor required by @c SCL_REFLECT_METHOD.
/// Delegates to @c executor_access::get so that the executor member does not
/// need to be @c public.
template <typename Value, template <typename> class Executor>
struct scl::feature::executor_trait<::scl::feature::detail::wrapper<Value, Executor>>
{
    template <typename Self>
    static constexpr decltype(auto) executor(Self && self) noexcept
    {
        return ::scl::feature::detail::executor_access::get(::std::forward<Self>(self));
    }
};

namespace scl::feature
{
    /// @brief Strips cv/ref qualifiers from a cv/ref-qualified inner wrapper.
    ///
    /// Matches @c detail::wrapper\<QualifiedInner, OuterExecutor\> when
    /// @c QualifiedInner is a cv/ref-qualified wrapper type, and forwards to
    /// the unqualified specialization.  This collapses all 12 cv/ref variants
    /// into one rule so that qualifiers on the stored type do not produce a
    /// separate, incompatible reflection chain.
    template <typename Wrapper, typename Executor, typename QualifiedInner, template <typename> class OuterExecutor>
        requires(::scl::feature::concepts::wrapper<QualifiedInner> &&
            !::std::same_as<QualifiedInner, ::std::remove_cvref_t<QualifiedInner>>)
    class reflect<Wrapper, Executor, detail::wrapper<QualifiedInner, OuterExecutor>>
        : public reflect<Wrapper, Executor, detail::wrapper<::std::remove_cvref_t<QualifiedInner>, OuterExecutor>>
    {};

    /// @brief Reflection chain for a plain (non-nested) wrapper.
    ///
    /// Delegates to @c reflect\<Wrapper, Executor, Value\> so
    /// that the outer wrapper exposes all methods reflected for the held
    /// value type.
    template <typename Wrapper, typename Executor, typename Value, template <typename> class OuterExecutor>
    class reflect<Wrapper, Executor, detail::wrapper<Value, OuterExecutor>>
        : public reflect<Wrapper, Executor, Value>
    {};
} // namespace scl::feature
