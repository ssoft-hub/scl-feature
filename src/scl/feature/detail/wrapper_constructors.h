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
/// @brief Emits the assignment performer (one functor + one helper) shared by all
///        wrapper assignment operators.
///
/// The wrapper never assigns one executor to another (executors are
/// non-assignable, per the executor concept).  Instead assignment is reflected
/// through the executor, mirroring @c SCL_REFLECT_MEMBER_BINARY_OPERATOR(=, ...):
///
/// -# **Executor-override path** — if the executor exposes a static
///    @c operator_assign(self_exec, source_exec) (e.g. a copy-on-write executor
///    that adopts the source's shared block), it is called directly, bypassing
///    @c execute().
/// -# **Value path** — otherwise the wrapped value is assigned value-semantically
///    (@c access(self) = @c access(source)) through @c execute(), so guarding
///    executors run their guard() before the write.
///
/// @c scl_assign_value_performer is a named functor (not a lambda) so its type can
/// appear in the class-scope predicate variable templates below.  Those predicates
/// (rather than inline constraints on the helper) keep the dispatch satisfiable on
/// MSVC, which mishandles a function whose noexcept-specifier or requires-clause is
/// evaluated inside another constraint.  The source value is read through the
/// *source's* executor type, so a wrapper may be assigned from a wrapper holding a
/// different value type on the same executor template.
#define SCL_WRAPPER_ASSIGNMENT_PERFORMER                                                                      \
    struct scl_assign_value_performer                                                                         \
    {                                                                                                         \
        template <typename ScLSelfExec, typename ScLSourceExec>                                               \
        constexpr auto operator()(ScLSelfExec && scl_self, ScLSourceExec && scl_source) const                 \
            -> decltype(executor_type::access(::std::forward<ScLSelfExec>(scl_self)) = ::std::remove_cvref_t< \
                            ScLSourceExec>::access(::std::forward<ScLSourceExec>(scl_source)))                \
        {                                                                                                     \
            return executor_type::access(::std::forward<ScLSelfExec>(scl_self)) = ::std::remove_cvref_t<      \
                       ScLSourceExec>::access(::std::forward<ScLSourceExec>(scl_source));                     \
        }                                                                                                     \
    };                                                                                                        \
    template <typename ScLSourceExec>                                                                         \
    static constexpr bool scl_assign_has_override =                                                           \
        requires {                                                                                            \
            executor_type::operator_assign(::std::declval<executor_type &>(),                                 \
                ::std::declval<ScLSourceExec>());                                                             \
        };                                                                                                    \
    template <typename ScLSourceExec>                                                                         \
    static constexpr bool scl_assign_value_viable =                                                           \
        requires {                                                                                            \
            executor_type::execute(::std::declval<executor_type &>(), scl_assign_value_performer{},           \
                ::std::declval<executor_type &>(), ::std::declval<ScLSourceExec>());                          \
        };                                                                                                    \
    template <typename ScLSourceExec>                                                                         \
    static constexpr bool scl_assignable = scl_assign_has_override<ScLSourceExec> ||                          \
        scl_assign_value_viable<ScLSourceExec>;                                                               \
    template <typename ScLSourceExec>                                                                         \
    static constexpr bool scl_assign_noexcept = []() constexpr noexcept -> bool {                             \
        if constexpr (scl_assign_has_override<ScLSourceExec>)                                                 \
            return noexcept(executor_type::operator_assign(::std::declval<executor_type &>(),                 \
                ::std::declval<ScLSourceExec>()));                                                            \
        else if constexpr (scl_assign_value_viable<ScLSourceExec>)                                            \
            return noexcept(executor_type::execute(::std::declval<executor_type &>(),                         \
                scl_assign_value_performer{}, ::std::declval<executor_type &>(),                              \
                ::std::declval<ScLSourceExec>()));                                                            \
        else                                                                                                  \
            return false;                                                                                     \
    }();                                                                                                      \
    template <typename ScLSourceExec>                                                                         \
    static constexpr decltype(auto)                                                                           \
    scl_assign_through_executor(executor_type & scl_self_exec, ScLSourceExec && scl_source_exec) /**/         \
        noexcept(scl_assign_noexcept<ScLSourceExec>)                                                          \
    {                                                                                                         \
        if constexpr (scl_assign_has_override<ScLSourceExec>)                                                 \
            return executor_type::operator_assign(scl_self_exec,                                              \
                ::std::forward<ScLSourceExec>(scl_source_exec));                                              \
        else                                                                                                  \
            return executor_type::execute(scl_self_exec, scl_assign_value_performer{},                        \
                scl_self_exec, ::std::forward<ScLSourceExec>(scl_source_exec));                               \
    }

/// @internal
/// @brief Generates one assignment operator for `self_type cv_ref` source.
///
/// Forwards the source wrapper's executor (carrying its cv-ref qualification) to
/// the assignment performer (see SCL_WRAPPER_ASSIGNMENT_PERFORMER).  Like every
/// reflected operator, it returns the performer's result (@c decltype(auto)),
/// not @c *this; the explicit overload exists only because the implicit copy/move
/// assignment is deleted (the executor member is non-assignable).
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)             \
    constexpr decltype(auto) operator=(self_type cv_ref other) /**/   \
        noexcept(scl_assign_noexcept<executor_type cv_ref>)           \
        requires(scl_assignable<executor_type cv_ref>)                \
    {                                                                 \
        return scl_assign_through_executor(m_executor,                \
            ::scl::forward_like<self_type cv_ref>(other.m_executor)); \
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
/// @brief Single forwarding-reference assignment for another wrapper type.
///
/// Reflects assignment from any other wrapper through the assignment performer
/// (see SCL_WRAPPER_ASSIGNMENT_PERFORMER), reading the source via its own
/// executor.  Constrained out for @c self_type (handled by the FOR_SELF
/// overloads) and for any source whose value the performer cannot assign.
#define SCL_WRAPPER_ASSIGNMENT_FOR_OTHER                                                                            \
    template <typename Other>                                                                                       \
    constexpr decltype(auto) operator=(Other && other) /**/                                                         \
        noexcept(scl_assign_noexcept<                                                                               \
            decltype(::scl::feature::detail::executor_access::get(::std::declval<Other>()))>)                       \
        requires(::scl::feature::is_wrapper_v<Other> && !::std::same_as<::std::remove_cvref_t<Other>, self_type> && \
            scl_assignable<decltype(::scl::feature::detail::executor_access::get(::std::declval<Other>()))>)        \
    {                                                                                                               \
        return scl_assign_through_executor(m_executor,                                                              \
            ::scl::feature::detail::executor_access::get(::std::forward<Other>(other)));                            \
    }
