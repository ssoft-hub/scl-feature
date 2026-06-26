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
/// | `static access(Self&&)`              | Returns a reference to the held value.       |
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
    /// @brief @c true if @p ExecutorType has a @c access() method whose first (and
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
    inline constexpr bool has_access_v =
        requires {
            static_cast<decltype(ExecutorType::access(::std::declval<ExecutorRefer>())) (*)(
                ExecutorRefer)>(&ExecutorType::access);
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

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p ExecutorType has a static @c operator_assign() callable as
    ///        @c operator_assign(SelfRefer, SourceRefer).
    ///
    /// @tparam ExecutorType  The executor class (cv-unqualified).
    /// @tparam SelfRefer     cv-ref-qualified reference to the target executor.
    /// @tparam SourceRefer   cv-ref-qualified reference to the source executor.
    template <typename ExecutorType, typename SelfRefer, typename SourceRefer>
    inline constexpr bool has_operator_assign_v =
        requires { static_cast<void (*)(SelfRefer, SourceRefer)>(&ExecutorType::operator_assign); };

    /// @ingroup scl_feature_type_traits
    /// @brief Noexcept value for executor assignment dispatch.
    ///
    /// @c true if assigning from @p SourceRefer into @p SelfRefer is @c noexcept —
    /// checking @c operator_assign when it exists, falling back to @c operator=.
    template <typename ExecutorType, typename SelfRefer, typename SourceRefer>
    inline constexpr bool is_executor_assign_noexcept_v = []() constexpr -> bool {
        if constexpr (has_operator_assign_v<ExecutorType, SelfRefer, SourceRefer>)
            return noexcept(ExecutorType::operator_assign(::std::declval<SelfRefer>(),
                ::std::declval<SourceRefer>()));
        else
            return ::std::is_nothrow_assignable_v<SelfRefer, SourceRefer>;
    }();

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if executor assignment from @p SourceRefer to @p SelfRefer is possible
    ///        via @c operator_assign or @c operator=.
    template <typename ExecutorType, typename SelfRefer, typename SourceRefer>
    inline constexpr bool executor_assign_possible_v = has_operator_assign_v<ExecutorType, SelfRefer, SourceRefer> ||
        ::std::assignable_from<SelfRefer, SourceRefer>;

    // -------------------------------------------------------------------------

    namespace detail
    {
        /// @c true if @p Type is a class and has static @c access() and @c execute()
        /// callable for the three primary value categories: @c Type&, @c Type&&,
        /// @c Type const&.  Non-class types (e.g. @c void, scalars) yield @c false.
        template <typename Type>
        inline constexpr bool has_executor_minimal_interface_v = []() constexpr -> bool {
            if constexpr (::std::is_class_v<Type>)
                return has_access_v<Type, Type &> && has_access_v<Type, Type &&> &&
                    has_access_v<Type, Type const &> && has_execute_v<Type, Type &> &&
                    has_execute_v<Type, Type &&> && has_execute_v<Type, Type const &>;
            return false;
        }();
    } // namespace detail

    // -------------------------------------------------------------------------

    /// @ingroup scl_feature_type_traits
    /// @brief @c true if @p Type satisfies the executor interface.
    ///
    /// The check requires that the following static methods are callable for
    /// the three primary value categories (@c Type&, @c Type&&, @c Type const&):
    ///
    /// - @c Type::access(Self&&) — returns a reference to the held value.
    /// - @c Type::execute(Self&&, Func&&) — invokes a callable in the
    ///   executor's context; checked with a zero-argument @c void(*)().
    ///
    /// Optional methods (@c guard, @c unguard) are not part of this check.
    /// Non-class types (e.g. @c void, scalars) always yield @c false.
    /// cv-ref qualifiers on @p Type are stripped before the check (@c remove_cvref_t).
    ///
    /// @tparam Type  Type to check (cv-ref qualifiers are stripped).
    ///
    /// @see scl::feature::concepts::executor
    ///
    /// @code{.cpp}
    ///   static_assert( is_executor_v<feature::inplace::plain<int>>);
    ///   static_assert( is_executor_v<feature::inplace::plain<int> const>);  // cv-ref stripped
    ///   static_assert(!is_executor_v<int>);
    ///   static_assert(!is_executor_v<void>);
    ///
    ///   template <typename T>
    ///   struct MyExecutor {
    ///       using value_type = T;
    ///       T m_value{};
    ///       template <typename Self, typename Func>
    ///       static constexpr decltype(auto) execute(Self&&, Func&& f) { return f(); }
    ///       template <typename Self>
    ///       static constexpr decltype(auto) access(Self&& self) { return forward_like<Self>(self.m_value); }
    ///   };
    ///   static_assert(is_executor_v<MyExecutor<int>>);
    /// @endcode
    template <typename Type>
    inline constexpr bool is_executor_v = ::std::is_same_v<::std::remove_cvref_t<Type>, Type>
        ? detail::has_executor_minimal_interface_v<Type>
        : is_executor_v<::std::remove_cvref_t<Type>>;

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
