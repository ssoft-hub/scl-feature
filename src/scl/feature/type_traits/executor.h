#pragma once

#include <type_traits>
#include <utility>

/// @file
/// @brief Executor type traits: interface detection and noexcept helpers.
///
/// An *executor* is a class template that implements the strategy used by
/// @c scl::wrapper to store and access its value.  The required interface is:
///
/// | Method                              | Role                                         |
/// |-------------------------------------|----------------------------------------------|
/// | `static value(Self&&)`              | Returns a reference to the held value.       |
/// | `static execute(Self&&, Func&&...)` | Invokes `func` in the executor's context.    |
///
/// Optional methods recognized by the framework:
///
/// | Method              | Role                                              |
/// |---------------------|---------------------------------------------------|
/// | `static guard(Self&&)`   | Acquires any guard (e.g., mutex lock).       |
/// | `static unguard(Self&&)` | Releases the guard acquired by @c guard().   |
///
/// @c is_executor_v detects whether a type satisfies this interface.
/// @c executor_trait is a separate reflection customization point used by
/// @c SCL_REFLECT_METHOD to locate the executor at runtime.

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

    // -------------------------------------------------------------------------

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p ExecutorType has a @c value() method whose first (and
    ///        only) parameter type is exactly @p ExecutorRefer.
    ///
    /// Detection uses a function-pointer cast: the return type is deduced from a
    /// trial call, then @c &ExecutorType::value is cast to that exact pointer type.
    /// This rejects implicit conversions (e.g. binding an rvalue to @c const&)
    /// while correctly handling both template and non-template overloads.
    ///
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool has_value_v =
        requires {
            static_cast<decltype(ExecutorType::value(::std::declval<ExecutorRefer>())) (*)(ExecutorRefer)>(
                &ExecutorType::value);
        };

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p ExecutorType has an @c execute() method whose first
    ///        parameter type is exactly @p ExecutorRefer.
    ///
    /// The callable parameter is probed as @c void(*&)() (lvalue-reference to
    /// function pointer) so that template argument deduction for @c Func&& collapses
    /// to an lvalue reference and the cast target type matches exactly.
    /// This enforces exact matching on the executor argument while remaining
    /// compatible with the universal-reference callable parameter required by the
    /// executor interface.
    ///
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool has_execute_v =
        requires {
            static_cast<decltype(ExecutorType::execute(::std::declval<ExecutorRefer>(),
                ::std::declval<void (*&)()>())) (*)(ExecutorRefer, void (*&)())>(&ExecutorType::execute);
        };

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p ExecutorType has a @c guard() method whose first
    ///        (and only) parameter type is exactly @p ExecutorRefer.
    ///
    /// Detection uses a function-pointer cast, which enforces exact parameter
    /// matching — both template (via deduction from cast target) and non-template
    /// overloads are supported, but implicit conversions (e.g. binding an rvalue
    /// to @c const&) are rejected.
    ///
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool has_guard_v =
        requires { static_cast<void (*)(ExecutorRefer)>(&ExecutorType::guard); };

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p ExecutorType has an @c unguard() method whose first
    ///        (and only) parameter type is exactly @p ExecutorRefer.
    ///
    /// @copydetails has_guard_v
    ///
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool has_unguard_v =
        requires { static_cast<void (*)(ExecutorRefer)>(&ExecutorType::unguard); };

    // -------------------------------------------------------------------------

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @c ExecutorType::guard() for @c ExecutorRefer either
    ///        does not exist or is @c noexcept.
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool is_guard_noexcept_v = []() constexpr -> bool {
        if constexpr (has_guard_v<ExecutorType, ExecutorRefer>)
            return noexcept(ExecutorType::guard(::std::declval<ExecutorRefer>()));
        return true;
    }();

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @c ExecutorType::unguard() for @c ExecutorRefer either
    ///        does not exist or is @c noexcept.
    /// @tparam ExecutorType   The executor class (cv-unqualified).
    /// @tparam ExecutorRefer  The cv-ref-qualified executor reference type.
    template <typename ExecutorType, typename ExecutorRefer>
    inline constexpr bool is_unguard_noexcept_v = []() constexpr -> bool {
        if constexpr (has_unguard_v<ExecutorType, ExecutorRefer>)
            return noexcept(ExecutorType::unguard(::std::declval<ExecutorRefer>()));
        return true;
    }();
} // namespace scl::feature
