#pragma once

#include <scl/feature/detail/executor_access.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <concepts>
#include <type_traits>

/// @internal
/// @brief Generates one constructor for `self_type cv_ref` via wrapper_constructor_resolver.
///
/// Covers all eight cv-ref combinations when expanded by SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
/// Using a macro eliminates eight near-identical definitions and eliminates the risk of
/// copy-paste mistakes (wrong cv-qualifier, wrong forward cast, wrong resolver argument).
// cppcheck-suppress noExplicitConstructor
#define SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)                               \
    constexpr wrapper(self_type cv_ref other) /**/                                       \
        noexcept(::std::is_nothrow_constructible_v<executor_type, executor_type cv_ref>) \
        requires(::std::constructible_from<executor_type, executor_type cv_ref>)         \
        : m_executor{::scl::forward_like<self_type cv_ref>(other.m_executor)}            \
    {}

/// @internal
/// @brief Expands to constructors for all eight cv-ref qualifications of `self_type`.
///
/// Suppresses the implicitly generated copy and move constructors so that
/// guard()/unguard() are called consistently for every cv-ref qualification.
#define SCL_WRAPPER_CONSTRUCTOR_FOR_SELF                         \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)                \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &)          \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)               \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &&)

/// @internal
/// @brief Single forwarding-reference constructor for any other wrapper type.
///
/// A forwarding reference deduces all cv-ref combinations in one definition.
/// The requires clause restricts this to wrapper types other than self_type,
/// preventing it from competing with SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
// cppcheck-suppress noExplicitConstructor
#define SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER                                                                           \
    template <typename Other>                                                                                       \
    constexpr wrapper(Other && other) /**/                                                                          \
        noexcept(noexcept(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve()))           \
        requires(::scl::feature::is_wrapper_v<Other> && !::std::same_as<::std::remove_cvref_t<Other>, self_type> && \
            ::std::constructible_from<executor_type,                                                                \
                decltype(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve())>)           \
        : m_executor{wrapper_constructor_resolver<self_type, Other &&>{::std::forward<Other>(other)}.resolve()}     \
    {}

/// @internal
/// @brief Emits the assignment dispatch (source resolver, performer functor, and the
///        predicate/helper set) shared by all wrapper assignment operators.
///
/// The wrapper's executor is non-assignable — an executor carries no assignment
/// operator of its own (see the executor concept) — so a wrapper never assigns
/// one executor to another.  Assignment is reflected through the executor with
/// the same dispatch every other reflected operator uses:
///
/// -# **Executor-override path** — if the executor exposes a static
///    @c operator_assign(self_exec, source) it is called directly, bypassing
///    @c execute().  The override receives the source operand, not the source
///    executor — the identical convention to the @c operator_##name overrides of
///    the reflected binary operators.
/// -# **Value path** — otherwise the wrapped value is assigned through
///    @c execute() as the expression @c access(self) @c = @c source.  The
///    expression form (rather than @c .operator=()) assigns fundamental and class
///    value types alike; resolving the wrapped value inside @c execute() keeps any
///    pre-write guard ahead of the write.
///
/// Each assignment operator resolves its right-hand operand to a value before
/// entering the dispatch: another wrapper is read through its own executor
/// (@c scl_assign_resolve), a plain value passes through unchanged.  Resolving in
/// the operator (rather than with @c wrapper_cast inside the performer) keeps the
/// dispatch on the value type, which lets a wrapper be assigned from a wrapper
/// holding a different value type or from a plain value through the one path — and
/// keeps the predicates satisfiable on MSVC, which mishandles a function whose
/// noexcept-specifier or requires-clause is evaluated inside another constraint.
#define SCL_WRAPPER_ASSIGNMENT_DISPATCH                                                                                     \
    template <typename ScLOther>                                                                                            \
    static constexpr decltype(auto) scl_assign_resolve(ScLOther && scl_other) noexcept                                      \
    {                                                                                                                       \
        if constexpr (::scl::feature::is_wrapper_v<::std::remove_cvref_t<ScLOther>>)                                        \
        {                                                                                                                   \
            using ScLOtherExec = ::std::remove_cvref_t<                                                                     \
                decltype(::scl::feature::detail::executor_access::get(::std::declval<ScLOther>()))>;                        \
            return ScLOtherExec::access(                                                                                    \
                ::scl::feature::detail::executor_access::get(::std::forward<ScLOther>(scl_other)));                         \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            return static_cast<ScLOther &&>(scl_other);                                                                     \
        }                                                                                                                   \
    }                                                                                                                       \
    struct scl_assign_value_performer                                                                                       \
    {                                                                                                                       \
        template <typename ScLSelfExec, typename ScLValue>                                                                  \
        constexpr auto operator()(ScLSelfExec && scl_self, ScLValue && scl_value) const                                     \
            -> decltype(executor_type::access(::std::forward<ScLSelfExec>(scl_self)) = ::std::forward<ScLValue>(scl_value)) \
        {                                                                                                                   \
            return executor_type::access(::std::forward<ScLSelfExec>(scl_self)) = ::std::forward<ScLValue>(scl_value);      \
        }                                                                                                                   \
    };                                                                                                                      \
    template <typename ScLValue>                                                                                            \
    static constexpr bool scl_assign_has_override =                                                                         \
        requires {                                                                                                          \
            executor_type::operator_assign(::std::declval<executor_type &>(), ::std::declval<ScLValue>());                  \
        };                                                                                                                  \
    template <typename ScLValue>                                                                                            \
    static constexpr bool scl_assign_value_viable =                                                                         \
        requires {                                                                                                          \
            executor_type::execute(::std::declval<executor_type &>(), scl_assign_value_performer{},                         \
                ::std::declval<executor_type &>(), ::std::declval<ScLValue>());                                             \
        };                                                                                                                  \
    template <typename ScLValue>                                                                                            \
    static constexpr bool scl_assignable = scl_assign_has_override<ScLValue> ||                                             \
        scl_assign_value_viable<ScLValue>;                                                                                  \
    template <typename ScLValue>                                                                                            \
    static constexpr bool scl_assign_noexcept = []() constexpr noexcept -> bool {                                           \
        if constexpr (scl_assign_has_override<ScLValue>)                                                                    \
            return noexcept(executor_type::operator_assign(::std::declval<executor_type &>(),                               \
                ::std::declval<ScLValue>()));                                                                               \
        else if constexpr (scl_assign_value_viable<ScLValue>)                                                               \
            return noexcept(executor_type::execute(::std::declval<executor_type &>(),                                       \
                scl_assign_value_performer{}, ::std::declval<executor_type &>(),                                            \
                ::std::declval<ScLValue>()));                                                                               \
        else                                                                                                                \
            return false;                                                                                                   \
    }();                                                                                                                    \
    template <typename ScLValue>                                                                                            \
    static constexpr decltype(auto)                                                                                         \
    scl_assign_dispatch(executor_type & scl_self_exec, ScLValue && scl_value) /**/                                          \
        noexcept(scl_assign_noexcept<ScLValue &&>)                                                                          \
    {                                                                                                                       \
        if constexpr (scl_assign_has_override<ScLValue &&>)                                                                 \
            return executor_type::operator_assign(scl_self_exec, ::std::forward<ScLValue>(scl_value));                      \
        else                                                                                                                \
            return executor_type::execute(scl_self_exec, scl_assign_value_performer{},                                      \
                scl_self_exec, ::std::forward<ScLValue>(scl_value));                                                        \
    }

/// @internal
/// @brief Generates one assignment operator for a `self_type cv_ref` source.
///
/// The wrapper's executor member is non-assignable, so the implicitly-declared
/// copy and move assignment operators would be defined as deleted.  These
/// explicit self-type overloads are user-declared copy/move assignment
/// operators; declaring them suppresses the deleted implicit ones.  They resolve
/// the source's value through its executor and feed it to the shared assignment
/// dispatch (see SCL_WRAPPER_ASSIGNMENT_DISPATCH), and — like every reflected
/// operator — return the dispatch result (@c decltype(auto)), not @c *this.
///
/// Unlike the general operand overload these are ordinary (non-template) member
/// functions, so their @c requires clause and @c noexcept specifier are evaluated
/// while the wrapper is still an incomplete type.  The source is therefore read
/// through @c executor_type::access (the executor is complete) rather than through
/// the wrapper, keeping the predicates keyed on the value type.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)                                                      \
    constexpr decltype(auto) operator=(self_type cv_ref other) /**/                                            \
        noexcept(scl_assign_noexcept<decltype(executor_type::access(::std::declval<executor_type cv_ref>()))>) \
        requires(scl_assignable<decltype(executor_type::access(::std::declval<executor_type cv_ref>()))>)      \
    {                                                                                                          \
        return scl_assign_dispatch(m_executor,                                                                 \
            executor_type::access(::scl::forward_like<self_type cv_ref>(other.m_executor)));                   \
    }

/// @internal
/// @brief Expands to assignment operators for all eight cv-ref qualifications of `self_type`.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF                         \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(&)                \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &)          \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(&&)               \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &&)

/// @internal
/// @brief Single forwarding-reference assignment for any non-self operand.
///
/// Reflects assignment from any operand other than @c self_type — another wrapper
/// type (read through its own executor by @c scl_assign_resolve) or a plain value
/// (passed through) — via the shared assignment dispatch.  Constrained out for
/// @c self_type (handled by the FOR_SELF overloads) and for any operand the
/// dispatch cannot assign.  Accepting a plain value is what makes
/// @c wrapper @c = @c value assign the wrapped value, matching every other
/// reflected operator.
#define SCL_WRAPPER_ASSIGNMENT_FOR_OPERAND                                                        \
    template <typename Other>                                                                     \
    constexpr decltype(auto) operator=(Other && other) /**/                                       \
        noexcept(scl_assign_noexcept<decltype(scl_assign_resolve(::std::declval<Other>()))>)      \
        requires(!::std::same_as<::std::remove_cvref_t<Other>, self_type> &&                      \
            scl_assignable<decltype(scl_assign_resolve(::std::declval<Other>()))>)                \
    {                                                                                             \
        return scl_assign_dispatch(m_executor, scl_assign_resolve(::std::forward<Other>(other))); \
    }
