#pragma once

#include <scl/feature/detail/executor_access.h>
#include <scl/feature/detail/executor_guard.h>
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
/// @brief Base that owns the reflected @c operator= dispatch the self-type overloads share.
///
/// The general operand assignment (@c w @c = @c value, @c w @c = @c other_wrapper) is the
/// reflected @c operator= @c reflect_operators generates via @c SCL_REFLECT_MEMBER_BINARY_OPERATOR
/// — the same free-path dispatch every other operator uses; a wrapper un-hides it with @c using.
/// The self-type overloads reuse that operator's applier so they run the identical
/// @c value @c = @c value expression.
#define SCL_WRAPPER_ASSIGNMENT_SELF_BASE ::scl::feature::reflect_operators<self_type, executor_type>

/// @internal
/// @brief Executor-level assignment for a same-type source, reusing the reflected applier.
///
/// The self-type overloads cannot route through the base @c operator= itself: their @c requires
/// and @c noexcept are evaluated while the wrapper is still incomplete, and the base operator's
/// body resolves the executor via @c executor_trait\<wrapper\> — a downcast to the incomplete
/// wrapper.  This helper stays at the executor level (the executor is complete) yet runs the same
/// @c operator_assign override / free applier as the reflected operator, guarding the target and
/// the source once each.
///
/// The free path is a named performer (not a lambda) so its viability is a @c requires predicate:
/// an executor whose @c execute takes no forwarded operands (e.g. the property holder) simply
/// leaves the same-type assignment un-viable rather than hard-erroring.  The performer's trailing
/// return type keeps a non-assignable value type SFINAE-friendly.
#define SCL_WRAPPER_ASSIGNMENT_SELF_HELPER                                                                  \
    struct operator_assign_scl_self_performer                                                               \
    {                                                                                                       \
        template <typename ScLTargetExec, typename ScLSourceExec>                                           \
        constexpr auto operator()(ScLTargetExec && scl_t, ScLSourceExec && scl_s) const                     \
            -> decltype(SCL_WRAPPER_ASSIGNMENT_SELF_BASE::operator_assign_scl_infix_applier::apply(         \
                executor_type::access(::std::declval<ScLTargetExec>()),                                     \
                executor_type::access(::std::declval<ScLSourceExec>())))                                    \
        {                                                                                                   \
            ::scl::feature::detail::executor_guard<ScLTargetExec &&> scl_target{                            \
                ::std::forward<ScLTargetExec>(scl_t)};                                                      \
            ::scl::feature::detail::executor_guard<ScLSourceExec &&> scl_source{                            \
                ::std::forward<ScLSourceExec>(scl_s)};                                                      \
            return SCL_WRAPPER_ASSIGNMENT_SELF_BASE::operator_assign_scl_infix_applier::apply(              \
                scl_target.value(), scl_source.value());                                                    \
        }                                                                                                   \
    };                                                                                                      \
    template <typename ScLSrcExec>                                                                          \
    static constexpr bool operator_assign_scl_self_has_override =                                           \
        SCL_WRAPPER_ASSIGNMENT_SELF_BASE::template operator_assign_scl_has_exec_override<                   \
            executor_type &, decltype(executor_type::access(::std::declval<ScLSrcExec>()))>;                \
    template <typename ScLSrcExec>                                                                          \
    static constexpr bool operator_assign_scl_self_free =                                                   \
        requires {                                                                                          \
            executor_type::execute(::std::declval<executor_type &>(), operator_assign_scl_self_performer{}, \
                ::std::declval<executor_type &>(), ::std::declval<ScLSrcExec>());                           \
        };                                                                                                  \
    template <typename ScLSrcExec>                                                                          \
    static constexpr bool operator_assign_scl_self_viable =                                                 \
        operator_assign_scl_self_has_override<ScLSrcExec> || operator_assign_scl_self_free<ScLSrcExec>;     \
    template <typename ScLSrcExec>                                                                          \
    static constexpr bool operator_assign_scl_self_noexcept = []() constexpr noexcept -> bool {             \
        if constexpr (operator_assign_scl_self_has_override<ScLSrcExec>)                                    \
            return noexcept(executor_type::operator_assign(::std::declval<executor_type &>(),               \
                executor_type::access(::std::declval<ScLSrcExec>())));                                      \
        else if constexpr (operator_assign_scl_self_free<ScLSrcExec>)                                       \
            return noexcept(executor_type::execute(::std::declval<executor_type &>(),                       \
                operator_assign_scl_self_performer{}, ::std::declval<executor_type &>(),                    \
                ::std::declval<ScLSrcExec>()));                                                             \
        else                                                                                                \
            return false;                                                                                   \
    }();                                                                                                    \
    template <typename ScLSrcExec>                                                                          \
    static constexpr decltype(auto)                                                                         \
    operator_assign_scl_self(executor_type & scl_self_exec, ScLSrcExec && scl_src_exec) /**/                \
        noexcept(operator_assign_scl_self_noexcept<ScLSrcExec>)                                             \
    {                                                                                                       \
        if constexpr (operator_assign_scl_self_has_override<ScLSrcExec>)                                    \
            return executor_type::operator_assign(scl_self_exec,                                            \
                executor_type::access(::std::forward<ScLSrcExec>(scl_src_exec)));                           \
        else                                                                                                \
            return executor_type::execute(scl_self_exec, operator_assign_scl_self_performer{},              \
                scl_self_exec, ::std::forward<ScLSrcExec>(scl_src_exec));                                   \
    }

/// @internal
/// @brief Generates one assignment operator for a `self_type cv_ref` source.
///
/// The wrapper's executor member is non-assignable, so the implicitly-declared copy and move
/// assignment operators would be defined as deleted.  These explicit self-type overloads are
/// user-declared copy/move assignment operators; declaring them suppresses the deleted implicit
/// ones.  Their body forwards to @c operator_assign_scl_self, which runs the same applier as the
/// reflected @c operator= with both operands guarded — so a same-type assignment behaves like
/// every other operand and returns the dispatch result (@c decltype(auto)), not @c *this.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)                 \
    constexpr decltype(auto) operator=(self_type cv_ref other) /**/       \
        noexcept(operator_assign_scl_self_noexcept<executor_type cv_ref>) \
        requires(operator_assign_scl_self_viable<executor_type cv_ref>)   \
    {                                                                     \
        return operator_assign_scl_self<executor_type cv_ref>(m_executor, \
            ::scl::forward_like<self_type cv_ref>(other.m_executor));     \
    }

/// @internal
/// @brief Expands to the shared helper plus assignment operators for all eight cv-ref
///        qualifications of `self_type`.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF                         \
    SCL_WRAPPER_ASSIGNMENT_SELF_HELPER                          \
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
/// A plain value (@c w @c = @c 42) or a different wrapper type — anything but @c self_type
/// (handled by the FOR_SELF overloads) — forwards straight to the base's reflected @c operator=,
/// the same free-path dispatch every other operator uses.  Delegating to the base (rather than
/// bringing it in with @c using) keeps the wrapper's own operator= declarations in control, so the
/// base's implicit copy/move assignment is not exposed for a same-type source.  A template member,
/// its constraint is checked at call time when the wrapper is complete, so referencing the base
/// operator (whose body downcasts to the wrapper) is well-formed.
#define SCL_WRAPPER_ASSIGNMENT_FOR_OPERAND                                                               \
    template <typename ScLOther>                                                                         \
    constexpr decltype(auto) operator=(ScLOther && scl_other) /**/                                       \
        noexcept(noexcept(static_cast<SCL_WRAPPER_ASSIGNMENT_SELF_BASE &>(::std::declval<self_type &>()) \
                .operator=(::std::declval<ScLOther>())))                                                 \
        requires(!::std::same_as<::std::remove_cvref_t<ScLOther>, self_type> &&                          \
            requires {                                                                                   \
                static_cast<SCL_WRAPPER_ASSIGNMENT_SELF_BASE &>(::std::declval<self_type &>())           \
                    .operator=(::std::declval<ScLOther>());                                              \
            })                                                                                           \
    {                                                                                                    \
        return static_cast<SCL_WRAPPER_ASSIGNMENT_SELF_BASE &>(*this).                                   \
        operator=(::std::forward<ScLOther>(scl_other));                                                  \
    }
