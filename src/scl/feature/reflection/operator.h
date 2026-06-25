#pragma once

/// @file
/// @brief Compile-time operator reflection macros for wrapper types.
/// @ingroup scl_feature_reflection
///
/// Provides macros to automatically generate proxy operators that forward calls
/// from a wrapper class to the held object through an executor, preserving
/// cv-ref qualifiers exactly.
///
/// Every generated overload dispatches through @c Executor::execute so that
/// executor strategies (e.g. locking executors) can intercept every call.
///
/// The executor is located at runtime via @c scl::feature::executor_trait,
/// which must be specialized for each wrapper type.
///
/// @par Requirements on the enclosing class
/// -# Declare the wrapper type with @c SCL_REFLECT_TYPE(Type,Member)
///    (after the executor member declaration).
/// -# Specialize @c scl::feature::executor_trait for the wrapper type.
/// -# The executor type must provide:
///    - a static @c access(exec) method that returns a reference to the
///      wrapped object; and
///    - a static @c execute(exec, callable, args...) method that invokes
///      @c callable(args...) in the executor's context and returns its result.

#include <scl/feature/reflection/access.h>
#include <scl/feature/reflection/type.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/type_traits/has_qualified_method.h>
#include <scl/feature/type_traits/wrapper.h>
#include <scl/feature/wrapper_cast.h>
#include <scl/utility/preprocessor/forward.h>

#include <type_traits>
#include <utility>

// clang-format off

/// @internal
/// @brief Generates two executor-override helper predicates for an operator inside the enclosing class.
/// @ingroup scl_feature_reflection
///
/// Produces per-operator static variable templates that check whether the executor
/// exposes an @c operator_##name override and whether that override is @c noexcept.
/// The executor type is taken as a template parameter @c ScLExec so that the
/// qualified member lookup @c remove_cvref_t\<ScLExec\>::operator_##name is a
/// *dependent* name — lookup is deferred to Phase 2 and caught by @c requires.
///
/// Generated variable templates:
///
/// - @c operator_##name##_scl_has_exec_override\<ScLExec, ScLArgs...\>
///   @c true iff @c remove_cvref_t\<ScLExec\>::operator_##name can be cast to a
///   function pointer with exact first-parameter type @c ScLExec.
///
/// - @c operator_##name##_scl_exec_noexcept\<ScLExec, ScLArgs...\>
///   @c true iff the above check passes and the call is @c noexcept.
///
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
#define SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                         \
    template <typename ScLExec, typename... ScLArgs>                                    \
    static constexpr bool operator_##name##_scl_has_exec_override =                     \
        requires {                                                                      \
            static_cast<                                                                \
                decltype(::std::remove_cvref_t<ScLExec>::operator_##name(               \
                    ::std::declval<ScLExec>(), ::std::declval<ScLArgs>()...))           \
                (*)(ScLExec, ScLArgs...)                                                \
            >(&::std::remove_cvref_t<ScLExec>::operator_##name);                        \
        };                                                                              \
    template <typename ScLExec, typename... ScLArgs>                                    \
    static constexpr bool operator_##name##_scl_exec_noexcept =                         \
        []() constexpr noexcept -> bool {                                               \
            if constexpr (operator_##name##_scl_has_exec_override<ScLExec, ScLArgs...>) \
                return noexcept(::std::remove_cvref_t<ScLExec>::operator_##name(        \
                    ::std::declval<ScLExec>(), ::std::declval<ScLArgs>()...));          \
            return false;                                                               \
        }();

/// @internal
/// @brief Generates the @c operator_##name##_scl_caller helper struct inside the enclosing class.
/// @ingroup scl_feature_reflection
///
/// The struct exposes a single static function template
/// @c call\<P,Ps...\>(obj, args...) that invokes
/// @c obj.template @c operator\ op\<P,Ps...\>(scl::wrapper_cast(args)...).
/// Because @c obj is a dependent type, the @c template keyword is legal there
/// and name lookup is deferred to instantiation — this avoids a hard parse
/// error when @p op is not a template operator on the concrete wrapped type.
///
/// @param op    The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
#define SCL_REFLECT_OPERATOR_CALLER(op, name)                                       \
    struct operator_##name##_scl_caller                                             \
    {                                                                               \
        template <typename ScLParam, typename... ScLParams,                         \
                  typename ScLObj, typename... ScLArgs>                             \
        static constexpr auto call(ScLObj && scl_obj, ScLArgs &&... scl_args)       \
            -> decltype(::std::forward<ScLObj>(scl_obj)                             \
                    .template operator op/**/<ScLParam, ScLParams...>(              \
                        ::scl::wrapper_cast(::std::forward<ScLArgs>(scl_args))...)) \
        {                                                                           \
            return ::std::forward<ScLObj>(scl_obj)                                  \
                .template operator op/**/<ScLParam, ScLParams...>(                  \
                    ::scl::wrapper_cast(::std::forward<ScLArgs>(scl_args))...);     \
        }                                                                           \
    };

/// @internal
/// @brief Generates the qualifier-discrimination predicates for @p op inside the enclosing class.
/// @ingroup scl_feature_reflection
///
/// Produces two constructs:
///
/// - @c operator_##name##_scl_quals\<V,As...\> — variable template that evaluates
///   @c SCL_HAS_QUALIFIED_METHOD for the non-template (deduced-args) overload.
///
/// - @c operator_##name##_scl_template_quals\<P,Ps...\>::value\<V,As...\> — nested
///   variable template that evaluates @c SCL_HAS_QUALIFIED_METHOD with the token
///   sequence @c template @c operator\ op\<P,Ps...\> (assembled via @c SCL_FORWARD),
///   so the check accounts for explicit template arguments.
///
/// @param op    The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
#define SCL_REFLECT_OPERATOR_QUALS(op, name)                                    \
    template <typename ScLVal, typename... ScLArgs>                             \
    static constexpr bool operator_##name##_scl_quals =                         \
        SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(operator op), ScLVal, ScLArgs...); \
    template <typename ScLParam, typename... ScLParams>                         \
    struct operator_##name##_scl_template_quals                                 \
    {                                                                           \
        template <typename ScLVal, typename... ScLArgs>                         \
        static constexpr bool value =                                           \
            SCL_HAS_QUALIFIED_METHOD(                                           \
                SCL_FORWARD(template operator op/**/<ScLParam, ScLParams...>),  \
                ScLVal, ScLArgs...);                                            \
    };

/// @internal
/// @brief Generates the executor-override operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Introduces a hidden template parameter @c ScLExec defaulting to
/// @c s_c_l_executor_type so that @c ScLExec::operator_##name is a *dependent*
/// name — lookup is deferred to Phase 2.  The @c is_same_v guard prevents
/// users from specifying @c ScLExec explicitly.
///
/// @param op     The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_OPERATOR_EXECUTOR_OVERRIDE_BASE(op, name, cv_ref)             \
    template <typename ScLExec = s_c_l_executor_type cv_ref, typename... ScLArgs> \
    constexpr decltype(auto) operator op(ScLArgs &&... scl_args) cv_ref           \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec, ScLArgs...>)            \
    requires                                                                      \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                    \
            && operator_##name##_scl_has_exec_override<ScLExec, ScLArgs...>)      \
    {                                                                             \
        return ::std::remove_cvref_t<ScLExec>::operator_##name(                   \
            SCL_EXECUTOR_ACCESS(cv_ref),                                          \
            ::std::forward<ScLArgs>(scl_args)...);                                \
    }

/// @internal
/// @brief requires-clause + body for a non-template operator overload (execute path).
/// @ingroup scl_feature_reflection
///
/// Active only when the executor does **not** provide a dedicated @c operator_##name
/// override (i.e. @c operator_##name##_scl_has_exec_override is @c false).
///
/// @param op     The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_OPERATOR_BASE(op, name, cv_ref)                              \
    noexcept (noexcept(SCL_EXECUTE_OVERRIDED(SCL_FORWARD(operator op), cv_ref))) \
    requires                                                                     \
        (!operator_##name##_scl_has_exec_override<s_c_l_executor_type cv_ref,    \
                                                      ScLArgs...> &&             \
        requires                                                                 \
        {                                                                        \
            SCL_ACCESS_DECLVAL(cv_ref).operator op(                              \
                ::scl::wrapper_cast(::std::declval<ScLArgs>())...);              \
        } && operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref)),   \
                 decltype(::scl::wrapper_cast(::std::declval<ScLArgs>()))...>)   \
    {                                                                            \
        return SCL_EXECUTE_OVERRIDED(SCL_FORWARD(operator op), cv_ref);          \
    }

/// @internal
/// @brief requires-clause + body for an explicit-template-args operator overload.
/// @ingroup scl_feature_reflection
///
/// Uses @p caller (a struct whose static @c call\<P,Ps...\>(obj,args...) wraps the
/// @c .template\ operator\ op\<...\> call) so that the @c template keyword appears
/// only in a dependent context, avoiding a hard parse error.
///
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param caller Helper struct generated by @c SCL_REFLECT_OPERATOR_CALLER.
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)                  \
    noexcept(noexcept(SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref)))            \
    requires                                                                      \
        requires                                                                  \
        {                                                                         \
            caller::template call<ScLParam, ScLParams...>(                        \
                SCL_ACCESS_DECLVAL(cv_ref), ::std::declval<ScLArgs>()...);        \
        } && operator_##name##_scl_template_quals<ScLParam, ScLParams...>         \
                 ::template value<decltype(SCL_ACCESS_DECLVAL(cv_ref)),           \
                     decltype(::scl::wrapper_cast(::std::declval<ScLArgs>()))...> \
    {                                                                             \
        return SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref);                    \
    }

/// @internal
/// @brief Generates three overloads (executor-override, execute-path, explicit template args)
///        for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// @param op     The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param caller Helper struct generated by @c SCL_REFLECT_OPERATOR_CALLER.
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_OPERATOR_HELPER(op, name, caller, cv_ref)                  \
    SCL_REFLECT_OPERATOR_EXECUTOR_OVERRIDE_BASE(SCL_FORWARD(op), name, cv_ref) \
    template <typename... ScLArgs>                                             \
    constexpr decltype(auto) operator op(ScLArgs &&... scl_args) cv_ref        \
        SCL_REFLECT_OPERATOR_BASE(SCL_FORWARD(op), name, cv_ref)               \
    template <typename ScLParam, typename... ScLParams, typename... ScLArgs>   \
    constexpr decltype(auto) operator op(ScLArgs &&... scl_args) cv_ref        \
        SCL_REFLECT_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)

/// @internal
/// @brief Generates all 24 overloads (3 × 8 cv-ref qualifiers) for @p op.
/// @ingroup scl_feature_reflection
///
/// @param op    The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name  Short unique name identifying the operator (plain identifier).
#define SCL_REFLECT_OPERATOR_IMPL(op, name)                                                            \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                                  \
    SCL_REFLECT_OPERATOR_CALLER(SCL_FORWARD(op), name)                                                 \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                            \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, &)                \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, &&)               \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const &)          \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const &&)         \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, volatile &)       \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, volatile &&)      \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const volatile &) \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const volatile &&)

/// @internal
/// @brief Generates the prefix-unary operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Combines executor-override and execute-path dispatch in a single overload
/// using an @c if constexpr branch rather than two separate overloads — prefix
/// unary operators take no arguments so there is no argument-type ambiguity
/// between the two paths.
///
/// @param op     The C++ operator token (e.g. @c -, @c ++, @c *).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                                      \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                                          \
    constexpr decltype(auto) operator op() cv_ref                                                     \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                                            \
    requires                                                                                          \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                                        \
            && (operator_##name##_scl_has_exec_override<ScLExec>                                      \
                || operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref))>))                \
    {                                                                                                 \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                               \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                                   \
                SCL_EXECUTOR_ACCESS(cv_ref));                                                         \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::execute(SCL_EXECUTOR_ACCESS(cv_ref),               \
                [](auto && scl_e) -> decltype(auto) {                                                 \
                return ::std::remove_cvref_t<ScLExec>::access(::std::forward<decltype(scl_e)>(scl_e)) \
                    .operator op();                                                                   \
            }, SCL_EXECUTOR_ACCESS(cv_ref));                                                          \
        }                                                                                             \
    }

/// @internal
/// @brief Generates the postfix-unary operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Analogous to @c SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE but the generated
/// overload takes a dummy @c int parameter (standard C++ postfix-operator
/// signature).
///
/// @param op     The C++ operator token (e.g. @c ++, @c --).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                                     \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                                          \
    constexpr decltype(auto) operator op(int) cv_ref                                                  \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                                            \
    requires                                                                                          \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                                        \
            && (operator_##name##_scl_has_exec_override<ScLExec>                                      \
                || operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref)), int>))           \
    {                                                                                                 \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                               \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                                   \
                SCL_EXECUTOR_ACCESS(cv_ref));                                                         \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::execute(SCL_EXECUTOR_ACCESS(cv_ref),               \
                [](auto && scl_e) -> decltype(auto) {                                                 \
                return ::std::remove_cvref_t<ScLExec>::access(::std::forward<decltype(scl_e)>(scl_e)) \
                    .operator op(int{});                                                              \
            }, SCL_EXECUTOR_ACCESS(cv_ref));                                                          \
        }                                                                                             \
    }

/// @brief Generates proxy prefix-unary operators that reflect @p op from the wrapped object
///        through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// For each of the 8 cv-ref qualifiers a single overload is generated that uses
/// @c if @c constexpr to dispatch to either the executor override or the execute
/// path (unlike binary operators which generate two separate constrained overloads).
///
/// Only member-function overloads are generated.  Unary operators have a single
/// operand, so there is no reverse-operand case for a hidden friend to cover; use
/// @c SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR explicitly if an ADL-only (member-less)
/// unary operator is required.
///
/// @par Constraint — callability
/// The overload is active only when either:
/// -# The executor provides an @c operator_##name(Executor cv_ref) override, or
/// -# The wrapped object has a dedicated @p op overload for the given qualifier
///    (checked via @c SCL_HAS_QUALIFIED_METHOD).
///
/// @par Executor override
/// If the executor provides a static @c operator_##name(Executor cv_ref) member
/// (detected via a function-pointer cast), the generated operator calls that
/// member directly, bypassing @c Executor::execute.
///
/// @par noexcept propagation
/// When the executor override path is selected, @c noexcept is propagated from
/// the override.  When the execute path is taken, @c noexcept follows
/// @c Executor::execute.
///
/// @param op    The C++ prefix-unary operator token (e.g. @c -, @c ++, @c *).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_neg).
///
/// @sa SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)                                   \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                 \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                           \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                  \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)                 \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)            \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)           \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)         \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)        \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &)   \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates member-only proxy prefix-unary operators (no hidden-friend variants).
/// @ingroup scl_feature_reflection
///
/// @details
/// Use when the operator must be a non-static member function in C++20 (e.g. @c operator->).
/// Generates 8 member overloads (one per cv-ref qualifier) with the same dispatch
/// logic as @c SCL_REFLECT_PREFIX_UNARY_OPERATOR, but without the additional
/// hidden-friend overloads.
///
/// @param op    The C++ prefix-unary operator token.
/// @param name  Short unique name identifying the operator (plain identifier).
///
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_MEMBER_BINARY_OPERATOR
#define SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR(op, name)                           \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                          \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                 \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)                \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)           \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)          \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)        \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)       \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &)  \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates proxy postfix-unary operators that reflect @p op from the wrapped object
///        through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// Identical in structure to @c SCL_REFLECT_PREFIX_UNARY_OPERATOR but generates
/// overloads that take a dummy @c int parameter (standard C++ postfix signature:
/// @c operator op(int)).
///
/// Only member-function overloads are generated.  Use
/// @c SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR explicitly if an ADL-only
/// (member-less) postfix operator is required.
///
/// @param op    The C++ postfix-unary operator token (@c ++ or @c --).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_postinc).
///
/// @sa SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)                                  \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                 \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                           \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                 \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)                \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)           \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)          \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)        \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)       \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &)  \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates member-only proxy postfix-unary operators (no hidden-friend variants).
/// @ingroup scl_feature_reflection
///
/// @details
/// Use when the operator must be a non-static member function.
/// Generates 8 member overloads (one per cv-ref qualifier) without the additional
/// hidden-friend overloads that @c SCL_REFLECT_POSTFIX_UNARY_OPERATOR produces.
///
/// @param op    The C++ postfix-unary operator token (@c ++ or @c --).
/// @param name  Short unique name identifying the operator (plain identifier).
///
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR
#define SCL_REFLECT_MEMBER_POSTFIX_UNARY_OPERATOR(op, name)                           \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                 \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                           \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                 \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)                \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)           \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)          \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)        \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)       \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &)  \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates proxy binary operators that reflect @p op from the wrapped object
///        through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// For each of the 8 cv-ref qualifiers (@c &, @c &&, @c const&, @c const&&,
/// @c volatile&, @c volatile&&, @c const @c volatile&, @c const @c volatile&&)
/// three overloads are generated:
/// - one **executor-override** overload — active when @c Executor::operator_##name exists
/// - one **execute-path** overload — active when no executor override is found
/// - one with **explicit** template arguments
///
/// @par Executor override
/// If the executor provides a static @c operator_##name(Executor cv_ref, args...)
/// member whose first parameter matches the wrapper's cv-ref qualification **exactly**
/// (detected via a function-pointer cast), the reflected operator calls that member
/// directly, bypassing @c Executor::execute.
///
/// @par noexcept propagation
/// When the executor-override path is selected, @c noexcept is propagated from the
/// override.  When the execute path is taken, @c noexcept follows @c Executor::execute.
///
/// @par Reverse-operand symmetry (hidden friends)
/// In addition to the 24 member overloads (wrapper as the **left** operand), 8
/// constrained hidden-friend overloads are generated — one per cv-ref qualifier — with
/// the wrapper as the **right** operand:
/// @code{.cpp}
/// w + x;   // member overload: reflects value.operator+(x)
/// x + w;   // hidden friend: reflects  x + value  (ADL-found, executor-routed)
/// @endcode
/// The reverse friend is constrained out when the left operand is itself a wrapper
/// (via @c is_wrapper_v), so @c w1 @c + @c w2 resolves unambiguously to the member
/// overload.  The reverse path has no executor-override shortcut — the @c operator_##name
/// override convention applies to the wrapper-left (member) overloads only — but it still
/// routes through @c Executor::execute, so cross-cutting behaviour applies on both sides.
///
/// @par Constraint — different return types required
/// Because @c SCL_HAS_QUALIFIED_METHOD relies on return-type discrimination,
/// overloads of the target operator with different cv-ref qualifiers
/// **must** return different types.
///
/// @par Example
/// @code{.cpp}
/// struct Target {
///     short operator+(int rhs) &;
///     int   operator+(int rhs) const &;
/// };
///
/// struct MyWrapper {
///     TargetExecutor m_exec;
///     SCL_REFLECT_TYPE(MyWrapper, TargetExecutor);
///
///     SCL_REFLECT_BINARY_OPERATOR(+, op_add)
/// };
///
/// MyWrapper w{10};
/// w + 5;              // calls Target::operator+(int) &  → short(15)
///
/// MyWrapper const cw{10};
/// cw + 5;             // calls Target::operator+(int) const &  → int(65)
/// @endcode
///
/// @param op    The C++ binary operator token (e.g. @c +, @c -, @c ==).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
///
/// @sa SCL_REFLECT_MEMBER_BINARY_OPERATOR
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_OPERATOR_WITH_ARGUMENTS
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_BINARY_OPERATOR(op, name) \
    SCL_REFLECT_COMBINED_OPERATOR_IMPL(SCL_FORWARD(op), name)

/// @brief Generates member-only proxy binary operators (no hidden-friend variants).
/// @ingroup scl_feature_reflection
///
/// @details
/// Use when the operator must be a non-static member function in C++20 (e.g. @c operator=,
/// compound assignment operators).  Generates the same 24 member overloads as the
/// member subset of @c SCL_REFLECT_BINARY_OPERATOR without any friend counterparts.
///
/// @par Example operators that require this macro
/// @c operator=, @c operator*=, @c operator/=, @c operator%=, @c operator+=,
/// @c operator-=, @c operator<<=, @c operator>>=, @c operator&=, @c operator|=,
/// @c operator^=.
///
/// @param op    The C++ binary operator token.
/// @param name  Short unique name identifying the operator (plain identifier).
///
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_MEMBER_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_OPERATOR_WITH_ARGUMENTS
#define SCL_REFLECT_MEMBER_BINARY_OPERATOR(op, name) \
    SCL_REFLECT_OPERATOR_IMPL(SCL_FORWARD(op), name)

/// @brief Generates proxy call/subscript operators that reflect @p op from the wrapped object
///        through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// Identical in structure to @c SCL_REFLECT_BINARY_OPERATOR.  Intended for operators
/// whose syntax requires the argument list to be enclosed in brackets — specifically
/// @c operator[] and @c operator() — but works for any operator accepted by
/// @c SCL_REFLECT_OPERATOR_IMPL.
///
/// @param op    The C++ operator token (@c [] or @c ()).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_call).
///
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name) \
    SCL_REFLECT_OPERATOR_IMPL(SCL_FORWARD(op), name)

// ============================================================================
// Hidden-friend operator reflection macros
// ============================================================================
//
// These are the friend-function ("hidden friend") analogues of the member-
// operator macros above.  The generated overloads are free functions defined
// with the `friend` keyword inside the enclosing class body, so they are:
//
//   • only findable via Argument-Dependent Lookup (ADL), not via
//     unqualified or qualified member lookup;
//   • invisible to `w.operator op(...)` call syntax.
//
// The wrapper is passed as an explicit first parameter `scl_self` with the
// same cv-ref qualifier that would be applied to `*this` in the member
// counterpart.  Access to the executor goes through
// `SCL_FRIEND_EXECUTOR_ACCESS` (defined in access.h) rather than through
// `*this`; the wrapped value is resolved via `Executor::value` inside the
// execute dispatch.
//
// The class-level helper constructs (QUALS, CALLER, EXEC_HELPERS) are
// identical to those generated by the member variants — do not invoke both
// the member and friend macro for the same `name` in the same class body,
// as that would produce duplicate static member definitions.
//
// Note: `operator[]` and `operator()` must be non-static member functions
// in C++20, so there is no `SCL_REFLECT_FRIEND_OPERATOR_WITH_ARGUMENTS`.

/// @internal
/// @brief Generates the executor-override hidden-friend operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Analogous to @c SCL_REFLECT_OPERATOR_EXECUTOR_OVERRIDE_BASE but generates a
/// @c friend free function taking @c s_c_l_type @c cv_ref @c scl_self as the
/// first parameter instead of using @c *this.
///
/// @param op     The C++ operator token.
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper parameter.
#define SCL_REFLECT_FRIEND_OPERATOR_EXECUTOR_OVERRIDE_BASE(op, name, cv_ref)      \
    template <typename ScLExec = s_c_l_executor_type cv_ref, typename... ScLArgs> \
    friend constexpr decltype(auto) operator op(                                  \
        s_c_l_type cv_ref scl_self, ScLArgs &&... scl_args)                       \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec, ScLArgs...>)            \
    requires                                                                      \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                    \
            && operator_##name##_scl_has_exec_override<ScLExec, ScLArgs...>)      \
    {                                                                             \
        return ::std::remove_cvref_t<ScLExec>::operator_##name(                   \
            SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref),                         \
            ::std::forward<ScLArgs>(scl_args)...);                                \
    }

/// @internal
/// @brief requires-clause + body for a hidden-friend non-template operator overload (execute path).
/// @ingroup scl_feature_reflection
///
/// Suffix macro (appended after the function declaration prefix).  Analogous to
/// @c SCL_REFLECT_OPERATOR_BASE but uses @c SCL_FRIEND_EXECUTE_OVERRIDED.
///
/// @param op     The C++ operator token.
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers of the @c scl_self parameter.
#define SCL_REFLECT_FRIEND_OPERATOR_BASE(op, name, cv_ref)                                   \
    noexcept(noexcept(SCL_FRIEND_EXECUTE_OVERRIDED_NX(SCL_FORWARD(operator op), cv_ref)))    \
    requires                                                                                 \
        (!operator_##name##_scl_has_exec_override<s_c_l_executor_type cv_ref, ScLArgs...> && \
        requires                                                                             \
        {                                                                                    \
            SCL_ACCESS_DECLVAL(cv_ref).operator op(                                          \
                ::scl::wrapper_cast(::std::declval<ScLArgs>())...);                          \
        } && operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref)),               \
                 decltype(::scl::wrapper_cast(::std::declval<ScLArgs>()))...>)               \
    {                                                                                        \
        return SCL_FRIEND_EXECUTE_OVERRIDED(SCL_FORWARD(operator op), scl_self, cv_ref);     \
    }

/// @internal
/// @brief requires-clause + body for a hidden-friend explicit-template-args operator overload.
/// @ingroup scl_feature_reflection
///
/// Suffix macro.  Analogous to @c SCL_REFLECT_TEMPLATE_OPERATOR_BASE but uses
/// @c SCL_FRIEND_EXECUTE_TEMPLATE_OVERRIDED.
///
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param caller Helper struct generated by @c SCL_REFLECT_OPERATOR_CALLER.
/// @param cv_ref cv-ref qualifiers of the @c scl_self parameter.
#define SCL_REFLECT_FRIEND_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)           \
    noexcept(noexcept(SCL_FRIEND_EXECUTE_TEMPLATE_OVERRIDED_NX(caller, cv_ref)))  \
    requires                                                                      \
        requires                                                                  \
        {                                                                         \
            caller::template call<ScLParam, ScLParams...>(                        \
                SCL_ACCESS_DECLVAL(cv_ref), ::std::declval<ScLArgs>()...);        \
        } && operator_##name##_scl_template_quals<ScLParam, ScLParams...>         \
                 ::template value<decltype(SCL_ACCESS_DECLVAL(cv_ref)),           \
                     decltype(::scl::wrapper_cast(::std::declval<ScLArgs>()))...> \
    {                                                                             \
        return SCL_FRIEND_EXECUTE_TEMPLATE_OVERRIDED(caller, scl_self, cv_ref);   \
    }

/// @internal
/// @brief Generates three hidden-friend overloads (executor-override, execute-path,
///        explicit template args) for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// @param op     The C++ operator token.
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param caller Helper struct generated by @c SCL_REFLECT_OPERATOR_CALLER.
/// @param cv_ref cv-ref qualifiers applied to the wrapper parameter.
#define SCL_REFLECT_FRIEND_OPERATOR_HELPER(op, name, caller, cv_ref)                  \
    SCL_REFLECT_FRIEND_OPERATOR_EXECUTOR_OVERRIDE_BASE(SCL_FORWARD(op), name, cv_ref) \
    template <typename... ScLArgs>                                                    \
    friend constexpr decltype(auto) operator op(                                      \
        s_c_l_type cv_ref scl_self, ScLArgs &&... scl_args)                           \
        SCL_REFLECT_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, cv_ref)               \
    template <typename ScLParam, typename... ScLParams, typename... ScLArgs>          \
    friend constexpr decltype(auto) operator op(                                      \
        s_c_l_type cv_ref scl_self, ScLArgs &&... scl_args)                           \
        SCL_REFLECT_FRIEND_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)

/// @internal
/// @brief Generates all 24 hidden-friend overloads (3 × 8 cv-ref qualifiers) for @p op.
/// @ingroup scl_feature_reflection
///
/// @param op    The C++ operator token.
/// @param name  Short unique name identifying the operator (plain identifier).
#define SCL_REFLECT_FRIEND_OPERATOR_IMPL(op, name)                                                            \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                                         \
    SCL_REFLECT_OPERATOR_CALLER(SCL_FORWARD(op), name)                                                        \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                                   \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, &)                \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, &&)               \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const &)          \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const &&)         \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, volatile &)       \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, volatile &&)      \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const volatile &) \
    SCL_REFLECT_FRIEND_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_scl_caller, const volatile &&)

/// @internal
/// @brief Generates the hidden-friend prefix-unary operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Combines executor-override and execute-path dispatch in a single overload using
/// @c if constexpr.  The wrapper is passed as an explicit @c scl_self parameter.
///
/// @param op     The C++ prefix-unary operator token.
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers of the @c scl_self parameter.
#define SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                               \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                                          \
    friend constexpr decltype(auto) operator op(s_c_l_type cv_ref scl_self)                           \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                                            \
    requires                                                                                          \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                                        \
            && (operator_##name##_scl_has_exec_override<ScLExec>                                      \
                || operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref))>))                \
    {                                                                                                 \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                               \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                                   \
                SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref));                                        \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::execute(                                           \
                SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref),                                         \
                [](auto && scl_e) -> decltype(auto) {                                                 \
                return ::std::remove_cvref_t<ScLExec>::access(::std::forward<decltype(scl_e)>(scl_e)) \
                    .operator op();                                                                   \
            }, SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref));                                         \
        }                                                                                             \
    }

/// @internal
/// @brief Generates the hidden-friend postfix-unary operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Analogous to @c SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE but the generated
/// overload takes a dummy @c int parameter (standard C++ postfix-operator signature).
///
/// @param op     The C++ postfix-unary operator token (@c ++ or @c --).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers of the @c scl_self parameter.
#define SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                              \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                                          \
    friend constexpr decltype(auto) operator op(s_c_l_type cv_ref scl_self, int)                      \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                                            \
    requires                                                                                          \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                                        \
            && (operator_##name##_scl_has_exec_override<ScLExec>                                      \
                || operator_##name##_scl_quals<decltype(SCL_ACCESS_DECLVAL(cv_ref)), int>))           \
    {                                                                                                 \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                               \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                                   \
                SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref));                                        \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            return ::std::remove_cvref_t<ScLExec>::execute(                                           \
                SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref),                                         \
                [](auto && scl_e) -> decltype(auto) {                                                 \
                return ::std::remove_cvref_t<ScLExec>::access(::std::forward<decltype(scl_e)>(scl_e)) \
                    .operator op(int{});                                                              \
            }, SCL_FRIEND_EXECUTOR_ACCESS(scl_self, cv_ref));                                         \
        }                                                                                             \
    }

/// @internal
/// @brief Emits the @c operator_##name##_scl_reverse_viable helper used by the reverse-operand
///        friend overloads in @c SCL_REFLECT_COMBINED_OPERATOR_IMPL.
/// @ingroup scl_feature_reflection
///
/// The viability check (@c lhs @c op @c value) is wrapped in a class-member function rather
/// than an inline requires-expression on the friend.  An inline requires would make the
/// friend's constraint atomic-constraint graph reference the very @c operator @p op being
/// declared (it is a candidate for the @p op inside its own constraint), which GCC rejects
/// as "atomic constraint depends on itself".  Routing through a function call breaks that
/// self-reference: the inner requires is evaluated lazily as a constant expression at
/// instantiation, in a separate phase from constraint normalisation.
///
/// @param op    The C++ binary operator token.
/// @param name  Short unique name identifying the operator (plain identifier).
#define SCL_REFLECT_REVERSE_COMBINED_HELPERS(op, name)                         \
    template <typename ScLLhs, typename ScLValue>                              \
    static constexpr bool operator_##name##_scl_reverse_viable() noexcept      \
    {                                                                          \
        return requires(ScLLhs && scl_l, ScLValue && scl_v)                    \
        { static_cast<ScLLhs &&>(scl_l) op static_cast<ScLValue &&>(scl_v); }; \
    }

/// @internal
/// @brief Generates one reverse-operand hidden-friend binary operator overload for one
///        cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// The wrapper is the **right** operand (@c s_c_l_type @c cv_ref @c scl_self); the left
/// operand @p ScLLhs is an arbitrary external type.  The overload reflects the expression
/// @c lhs @c op @c value through @c Executor::execute.
///
/// @par Anti-ambiguity
/// Constrained out when @p ScLLhs is itself a wrapper (@c is_wrapper_v), so that
/// @c w1 @c op @c w2 resolves to the member overload (wrapper-left) without ambiguity.
/// For @c x @c op @c w (non-wrapper @c x) only this friend is viable; for @c w @c op @c x
/// only the member overload is viable (a non-wrapper @c x cannot bind the wrapper
/// parameter).  Viability of @c lhs @c op @c value is checked through the
/// @c operator_##name##_scl_reverse_viable helper (see that macro for why an inline
/// requires would be self-referential).
///
/// There is no reverse executor-override path: the @c operator_##name override convention
/// applies to the wrapper-left member overloads only.  The reflected call still routes
/// through @c Executor::execute.
///
/// @param op     The C++ binary operator token (e.g. @c +, @c ==).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper (right-operand) parameter.
#define SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(op, name, cv_ref)                     \
    template <typename ScLLhs>                                                         \
    friend constexpr decltype(auto) operator op(                                       \
        ScLLhs && scl_lhs, s_c_l_type cv_ref scl_self)                                 \
    noexcept(noexcept(SCL_FRIEND_REVERSE_EXECUTE_NX(SCL_FORWARD(op), cv_ref)))         \
    requires                                                                           \
        (!::scl::feature::is_wrapper_v<::std::remove_cvref_t<ScLLhs>>                  \
            && !::std::is_same_v<::std::remove_cvref_t<ScLLhs>,                        \
                   ::std::remove_cvref_t<s_c_l_type>>                                  \
            && operator_##name##_scl_reverse_viable<ScLLhs,                            \
                   decltype(SCL_ACCESS_DECLVAL(cv_ref))>())                            \
    {                                                                                  \
        return SCL_FRIEND_REVERSE_EXECUTE(SCL_FORWARD(op), scl_self, scl_lhs, cv_ref); \
    }

/// @internal
/// @brief Generates all 24 member overloads (wrapper-left) plus 8 reverse-operand
///        hidden-friend overloads (wrapper-right, one per cv-ref qualifier) for @p op.
/// @ingroup scl_feature_reflection
///
/// The reverse friends make the reflected binary operator symmetric: @c w @c op @c x
/// dispatches through the member overload, @c x @c op @c w through ADL.  See
/// @c SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE for the anti-ambiguity rule.
///
/// @param op    The C++ operator token.
/// @param name  Short unique name identifying the operator.
#define SCL_REFLECT_COMBINED_OPERATOR_IMPL(op, name)                                  \
    SCL_REFLECT_OPERATOR_IMPL(SCL_FORWARD(op), name)                                  \
    SCL_REFLECT_REVERSE_COMBINED_HELPERS(SCL_FORWARD(op), name)                       \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, &)                \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, &&)               \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, const &)          \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)         \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)       \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)      \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &) \
    SCL_REFLECT_REVERSE_FRIEND_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates hidden-friend proxy binary operators that reflect @p op from the wrapped
///        object through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// Identical in structure to @c SCL_REFLECT_BINARY_OPERATOR but generates @c friend
/// free functions instead of member functions.  Each generated overload takes the
/// wrapper itself as its first (cv-ref-qualified) parameter and is only
/// findable via Argument-Dependent Lookup (ADL).
///
/// @par Key difference from @c SCL_REFLECT_BINARY_OPERATOR
/// - @c w.operator op(args) — NOT valid (no member function generated).
/// - @c w @c op @c args    — valid via ADL.
///
/// @par Constraint — different return types required
/// Same as @c SCL_REFLECT_BINARY_OPERATOR: overloads with different cv-ref
/// qualifiers must return distinct types so that @c SCL_HAS_QUALIFIED_METHOD
/// can distinguish them.
///
/// @par Do not combine with @c SCL_REFLECT_BINARY_OPERATOR for the same @p name
/// Both macros emit the same class-level helper constructs (QUALS, CALLER,
/// EXEC_HELPERS).  Using both for identical @p name in the same class body
/// produces duplicate static member definitions.
///
/// @param op    The C++ binary operator token (e.g. @c +, @c -, @c ==).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
///
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_FRIEND_BINARY_OPERATOR(op, name) \
    SCL_REFLECT_FRIEND_OPERATOR_IMPL(SCL_FORWARD(op), name)

/// @brief Generates hidden-friend proxy prefix-unary operators that reflect @p op from the
///        wrapped object through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// Identical in structure to @c SCL_REFLECT_PREFIX_UNARY_OPERATOR but generates
/// @c friend free functions.  The wrapper itself is passed as the cv-ref-qualified
/// operand; the operator is only ADL-findable.
///
/// @par Do not combine with @c SCL_REFLECT_PREFIX_UNARY_OPERATOR for the same @p name
/// Both macros emit the same class-level helper constructs.
///
/// @param op    The C++ prefix-unary operator token (e.g. @c -, @c ++, @c *).
/// @param name  Short unique name identifying the operator (plain identifier).
///
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_BINARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR
#define SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR(op, name)                                 \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                      \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)               \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)          \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)         \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)       \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)      \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &) \
    SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

/// @brief Generates hidden-friend proxy postfix-unary operators that reflect @p op from the
///        wrapped object through the executor, for all 8 cv-ref qualifier combinations.
/// @ingroup scl_feature_reflection
///
/// @details
/// Identical in structure to @c SCL_REFLECT_POSTFIX_UNARY_OPERATOR but generates
/// @c friend free functions.  Each overload takes a dummy @c int parameter
/// (standard C++ postfix-operator signature).
///
/// @par Do not combine with @c SCL_REFLECT_POSTFIX_UNARY_OPERATOR for the same @p name
/// Both macros emit the same class-level helper constructs.
///
/// @param op    The C++ postfix-unary operator token (@c ++ or @c --).
/// @param name  Short unique name identifying the operator (plain identifier).
///
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_BINARY_OPERATOR
/// @sa SCL_REFLECT_FRIEND_PREFIX_UNARY_OPERATOR
#define SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR(op, name)                                 \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                       \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                 \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)               \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)          \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)         \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)       \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)      \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &) \
    SCL_REFLECT_FRIEND_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &&)

// clang-format on
