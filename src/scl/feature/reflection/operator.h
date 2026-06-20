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
///    - a static @c value(exec) method that returns a reference to the
///      wrapped object; and
///    - a static @c execute(exec, callable, args...) method that invokes
///      @c callable(args...) in the executor's context and returns its result.

#include <scl/feature/reflection/access.h>
#include <scl/feature/reflection/type.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/type_traits/has_qualified_method.h>
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
            SCL_VALUE_DECLVAL(cv_ref).operator op(                               \
                ::scl::wrapper_cast(::std::declval<ScLArgs>())...);              \
        } && operator_##name##_scl_quals<decltype(SCL_VALUE_DECLVAL(cv_ref)),    \
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
                SCL_VALUE_DECLVAL(cv_ref), ::std::declval<ScLArgs>()...);         \
        } && operator_##name##_scl_template_quals<ScLParam, ScLParams...>         \
                 ::template value<decltype(SCL_VALUE_DECLVAL(cv_ref)),            \
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
#define SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                        \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                            \
    constexpr decltype(auto) operator op() cv_ref                                       \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                              \
    requires                                                                            \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                          \
            && (operator_##name##_scl_has_exec_override<ScLExec>                        \
                || operator_##name##_scl_quals<decltype(SCL_VALUE_DECLVAL(cv_ref))>))   \
    {                                                                                   \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                 \
        {                                                                               \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                     \
                SCL_EXECUTOR_ACCESS(cv_ref));                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            return ::std::remove_cvref_t<ScLExec>::execute(SCL_EXECUTOR_ACCESS(cv_ref), \
                [](auto && scl_v) -> decltype(auto)                                     \
                { return ::std::forward<decltype(scl_v)>(scl_v).operator op(); },       \
                ::std::remove_cvref_t<ScLExec>::value(SCL_EXECUTOR_ACCESS(cv_ref)));    \
        }                                                                               \
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
#define SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                          \
    template <typename ScLExec = s_c_l_executor_type cv_ref>                               \
    constexpr decltype(auto) operator op(int) cv_ref                                       \
    noexcept(operator_##name##_scl_exec_noexcept<ScLExec>)                                 \
    requires                                                                               \
        (::std::is_same_v<ScLExec, s_c_l_executor_type cv_ref>                             \
            && (operator_##name##_scl_has_exec_override<ScLExec>                           \
                || operator_##name##_scl_quals<decltype(SCL_VALUE_DECLVAL(cv_ref)), int>)) \
    {                                                                                      \
        if constexpr (operator_##name##_scl_has_exec_override<ScLExec>)                    \
        {                                                                                  \
            return ::std::remove_cvref_t<ScLExec>::operator_##name(                        \
                SCL_EXECUTOR_ACCESS(cv_ref));                                              \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            return ::std::remove_cvref_t<ScLExec>::execute(SCL_EXECUTOR_ACCESS(cv_ref),    \
                [](auto && scl_v) -> decltype(auto)                                        \
                { return ::std::forward<decltype(scl_v)>(scl_v).operator op(int{}); },     \
                ::std::remove_cvref_t<ScLExec>::value(SCL_EXECUTOR_ACCESS(cv_ref)));       \
        }                                                                                  \
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
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_PREFIX_UNARY_OPERATOR(op, name)                                 \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                               \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                         \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)               \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)          \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)         \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)       \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)      \
    SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &) \
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
/// @param op    The C++ postfix-unary operator token (@c ++ or @c --).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_postinc).
///
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_BINARY_OPERATOR
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_POSTFIX_UNARY_OPERATOR(op, name)                                 \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                          \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &)                \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, &&)               \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &)          \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const &&)         \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &)       \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, volatile &&)      \
    SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(SCL_FORWARD(op), name, const volatile &) \
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
/// @sa SCL_REFLECT_PREFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_POSTFIX_UNARY_OPERATOR
/// @sa SCL_REFLECT_OPERATOR_WITH_ARGUMENTS
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_BINARY_OPERATOR(op, name) SCL_REFLECT_OPERATOR_IMPL(SCL_FORWARD(op), name)

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
#define SCL_REFLECT_OPERATOR_WITH_ARGUMENTS(op, name) SCL_REFLECT_OPERATOR_IMPL(SCL_FORWARD(op), name)

// clang-format on
