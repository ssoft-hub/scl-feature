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
/// The executor type is taken as a template parameter @c S_c_L_E__ so that the
/// qualified member lookup @c remove_cvref_t\<S_c_L_E__\>::operator_##name is a
/// *dependent* name — lookup is deferred to Phase 2 and caught by @c requires.
///
/// Generated variable templates:
///
/// - @c operator_##name##_S_c_L_has_exec_override_\<S_c_L_E__, S_c_L_A___...\>
///   @c true iff @c remove_cvref_t\<S_c_L_E__\>::operator_##name can be cast to a
///   function pointer with exact first-parameter type @c S_c_L_E__.
///
/// - @c operator_##name##_S_c_L_exec_noexcept_\<S_c_L_E__, S_c_L_A___...\>
///   @c true iff the above check passes and the call is @c noexcept.
///
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
#define SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                 \
    template <typename S_c_L_E__, typename... S_c_L_A___>                                       \
    static constexpr bool operator_##name##_S_c_L_has_exec_override_ =                          \
        requires {                                                                              \
            static_cast<                                                                        \
                decltype(::std::remove_cvref_t<S_c_L_E__>::operator_##name(                     \
                    ::std::declval<S_c_L_E__>(), ::std::declval<S_c_L_A___>()...))              \
                (*)(S_c_L_E__, S_c_L_A___...)                                                   \
            >(&::std::remove_cvref_t<S_c_L_E__>::operator_##name);                              \
        };                                                                                      \
    template <typename S_c_L_E__, typename... S_c_L_A___>                                       \
    static constexpr bool operator_##name##_S_c_L_exec_noexcept_ =                              \
        []() constexpr noexcept -> bool {                                                       \
            if constexpr (operator_##name##_S_c_L_has_exec_override_<S_c_L_E__, S_c_L_A___...>) \
                return noexcept(::std::remove_cvref_t<S_c_L_E__>::operator_##name(              \
                    ::std::declval<S_c_L_E__>(), ::std::declval<S_c_L_A___>()...));             \
            return false;                                                                       \
        }();

/// @internal
/// @brief Generates the @c operator_##name##_S_c_L_caller_ helper struct inside the enclosing class.
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
#define SCL_REFLECT_OPERATOR_CALLER(op, name)                                            \
    struct operator_##name##_S_c_L_caller_                                               \
    {                                                                                    \
        template <typename P_a_r_a_m___, typename... P_a_r_a_m_s___,                     \
                  typename O_b_j___, typename... A_r_g_s___>                             \
        static constexpr auto call(O_b_j___ && o_b_j___, A_r_g_s___ &&... a_r_g_s___)    \
            -> decltype(::std::forward<O_b_j___>(o_b_j___)                               \
                    .template operator op/**/<P_a_r_a_m___, P_a_r_a_m_s___...>(          \
                        ::scl::wrapper_cast(::std::forward<A_r_g_s___>(a_r_g_s___))...)) \
        {                                                                                \
            return ::std::forward<O_b_j___>(o_b_j___)                                    \
                .template operator op/**/<P_a_r_a_m___, P_a_r_a_m_s___...>(              \
                    ::scl::wrapper_cast(::std::forward<A_r_g_s___>(a_r_g_s___))...);     \
        }                                                                                \
    };

/// @internal
/// @brief Generates the qualifier-discrimination predicates for @p op inside the enclosing class.
/// @ingroup scl_feature_reflection
///
/// Produces two constructs:
///
/// - @c operator_##name##_S_c_L_quals_\<V,As...\> — variable template that evaluates
///   @c SCL_HAS_QUALIFIED_METHOD for the non-template (deduced-args) overload.
///
/// - @c operator_##name##_S_c_L_template_quals_\<P,Ps...\>::value\<V,As...\> — nested
///   variable template that evaluates @c SCL_HAS_QUALIFIED_METHOD with the token
///   sequence @c template @c operator\ op\<P,Ps...\> (assembled via @c SCL_FORWARD),
///   so the check accounts for explicit template arguments.
///
/// @param op    The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name  Short unique name identifying the operator (plain identifier, e.g. @c op_add).
#define SCL_REFLECT_OPERATOR_QUALS(op, name)                                           \
    template <typename S_c_L_V___, typename... S_c_L_A___>                             \
    static constexpr bool operator_##name##_S_c_L_quals_ =                             \
        SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(operator op), S_c_L_V___, S_c_L_A___...); \
    template <typename S_c_L_P___, typename... S_c_L_Ps___>                            \
    struct operator_##name##_S_c_L_template_quals_                                     \
    {                                                                                  \
        template <typename S_c_L_V___, typename... S_c_L_A___>                         \
        static constexpr bool value =                                                  \
            SCL_HAS_QUALIFIED_METHOD(                                                  \
                SCL_FORWARD(template operator op/**/<S_c_L_P___, S_c_L_Ps___...>),     \
                S_c_L_V___, S_c_L_A___...);                                            \
    };

/// @internal
/// @brief Generates the executor-override operator overload for one cv-ref qualifier.
/// @ingroup scl_feature_reflection
///
/// Introduces a hidden template parameter @c S_c_L_E__ defaulting to
/// @c S_c_L_executor_type_ so that @c S_c_L_E__::operator_##name is a *dependent*
/// name — lookup is deferred to Phase 2.  The @c is_same_v guard prevents
/// users from specifying @c S_c_L_E__ explicitly.
///
/// @param op     The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_OPERATOR_EXECUTOR_OVERRIDE_BASE(op, name, cv_ref)                  \
    template <typename S_c_L_E__ = S_c_L_executor_type_ cv_ref, typename... A_r_g_s__> \
    constexpr decltype(auto) operator op(A_r_g_s__ &&... a_r_g_s__) cv_ref             \
    noexcept(operator_##name##_S_c_L_exec_noexcept_<S_c_L_E__, A_r_g_s__...>)          \
    requires                                                                           \
        (::std::is_same_v<S_c_L_E__, S_c_L_executor_type_ cv_ref>                      \
            && operator_##name##_S_c_L_has_exec_override_<S_c_L_E__, A_r_g_s__...>)    \
    {                                                                                  \
        return ::std::remove_cvref_t<S_c_L_E__>::operator_##name(                      \
            SCL_EXECUTOR_ACCESS(cv_ref),                                               \
            ::std::forward<A_r_g_s__>(a_r_g_s__)...);                                  \
    }

/// @internal
/// @brief requires-clause + body for a non-template operator overload (execute path).
/// @ingroup scl_feature_reflection
///
/// Active only when the executor does **not** provide a dedicated @c operator_##name
/// override (i.e. @c operator_##name##_S_c_L_has_exec_override_ is @c false).
///
/// @param op     The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name   Short unique name identifying the operator (plain identifier).
/// @param cv_ref cv-ref qualifiers applied to the wrapper.
#define SCL_REFLECT_OPERATOR_BASE(op, name, cv_ref)                               \
    noexcept (noexcept(SCL_EXECUTE_OVERRIDED(SCL_FORWARD(operator op), cv_ref)))  \
    requires                                                                      \
        (!operator_##name##_S_c_L_has_exec_override_<S_c_L_executor_type_ cv_ref, \
                                                      A_r_g_s__...> &&            \
        requires                                                                  \
        {                                                                         \
            SCL_VALUE_DECLVAL(cv_ref).operator op(                                \
                ::scl::wrapper_cast(::std::declval<A_r_g_s__>())...);             \
        } && operator_##name##_S_c_L_quals_<decltype(SCL_VALUE_DECLVAL(cv_ref)),  \
                 decltype(::scl::wrapper_cast(::std::declval<A_r_g_s__>()))...>)  \
    {                                                                             \
        return SCL_EXECUTE_OVERRIDED(SCL_FORWARD(operator op), cv_ref);           \
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
#define SCL_REFLECT_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)                    \
    noexcept(noexcept(SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref)))              \
    requires                                                                        \
        requires                                                                    \
        {                                                                           \
            caller::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(                   \
                SCL_VALUE_DECLVAL(cv_ref), ::std::declval<A_r_g_s__>()...);         \
        } && operator_##name##_S_c_L_template_quals_<P_a_r_a_m__, P_a_r_a_m_s__...> \
                 ::template value<decltype(SCL_VALUE_DECLVAL(cv_ref)),              \
                     decltype(::scl::wrapper_cast(::std::declval<A_r_g_s__>()))...> \
    {                                                                               \
        return SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref);                      \
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
#define SCL_REFLECT_OPERATOR_HELPER(op, name, caller, cv_ref)                         \
    SCL_REFLECT_OPERATOR_EXECUTOR_OVERRIDE_BASE(SCL_FORWARD(op), name, cv_ref)        \
    template <typename... A_r_g_s__>                                                  \
    constexpr decltype(auto) operator op(A_r_g_s__ &&... a_r_g_s__) cv_ref            \
        SCL_REFLECT_OPERATOR_BASE(SCL_FORWARD(op), name, cv_ref)                      \
    template <typename P_a_r_a_m__, typename... P_a_r_a_m_s__, typename... A_r_g_s__> \
    constexpr decltype(auto) operator op(A_r_g_s__ &&... a_r_g_s__) cv_ref            \
        SCL_REFLECT_TEMPLATE_OPERATOR_BASE(name, caller, cv_ref)

/// @internal
/// @brief Generates all 24 overloads (3 × 8 cv-ref qualifiers) for @p op.
/// @ingroup scl_feature_reflection
///
/// @param op    The C++ operator token (e.g. @c +, @c [], @c ()).
/// @param name  Short unique name identifying the operator (plain identifier).
#define SCL_REFLECT_OPERATOR_IMPL(op, name)                                                               \
    SCL_REFLECT_OPERATOR_QUALS(SCL_FORWARD(op), name)                                                     \
    SCL_REFLECT_OPERATOR_CALLER(SCL_FORWARD(op), name)                                                    \
    SCL_REFLECT_OPERATOR_EXEC_HELPERS(name)                                                               \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, &)                \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, &&)               \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, const &)          \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, const &&)         \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, volatile &)       \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, volatile &&)      \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, const volatile &) \
    SCL_REFLECT_OPERATOR_HELPER(SCL_FORWARD(op), name, operator_##name##_S_c_L_caller_, const volatile &&)

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
#define SCL_REFLECT_PREFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                          \
    template <typename S_c_L_E__ = S_c_L_executor_type_ cv_ref>                           \
    constexpr decltype(auto) operator op() cv_ref                                         \
    noexcept(operator_##name##_S_c_L_exec_noexcept_<S_c_L_E__>)                           \
    requires                                                                              \
        (::std::is_same_v<S_c_L_E__, S_c_L_executor_type_ cv_ref>                         \
            && (operator_##name##_S_c_L_has_exec_override_<S_c_L_E__>                     \
                || operator_##name##_S_c_L_quals_<decltype(SCL_VALUE_DECLVAL(cv_ref))>))  \
    {                                                                                     \
        if constexpr (operator_##name##_S_c_L_has_exec_override_<S_c_L_E__>)              \
        {                                                                                  \
            return ::std::remove_cvref_t<S_c_L_E__>::operator_##name(                     \
                SCL_EXECUTOR_ACCESS(cv_ref));                                             \
        }                                                                                  \
        else                                                                              \
        {                                                                                  \
            return ::std::remove_cvref_t<S_c_L_E__>::execute(SCL_EXECUTOR_ACCESS(cv_ref), \
                [](auto && S_c_L_v__) -> decltype(auto)                                   \
                { return ::std::forward<decltype(S_c_L_v__)>(S_c_L_v__).operator op(); }, \
                ::std::remove_cvref_t<S_c_L_E__>::value(SCL_EXECUTOR_ACCESS(cv_ref)));    \
        }                                                                                  \
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
#define SCL_REFLECT_POSTFIX_UNARY_OPERATOR_BASE(op, name, cv_ref)                              \
    template <typename S_c_L_E__ = S_c_L_executor_type_ cv_ref>                                \
    constexpr decltype(auto) operator op(int) cv_ref                                           \
    noexcept(operator_##name##_S_c_L_exec_noexcept_<S_c_L_E__>)                                \
    requires                                                                                   \
        (::std::is_same_v<S_c_L_E__, S_c_L_executor_type_ cv_ref>                              \
            && (operator_##name##_S_c_L_has_exec_override_<S_c_L_E__>                          \
                || operator_##name##_S_c_L_quals_<decltype(SCL_VALUE_DECLVAL(cv_ref)), int>))  \
    {                                                                                          \
        if constexpr (operator_##name##_S_c_L_has_exec_override_<S_c_L_E__>)                   \
        {                                                                                       \
            return ::std::remove_cvref_t<S_c_L_E__>::operator_##name(                          \
                SCL_EXECUTOR_ACCESS(cv_ref));                                                  \
        }                                                                                       \
        else                                                                                   \
        {                                                                                       \
            return ::std::remove_cvref_t<S_c_L_E__>::execute(SCL_EXECUTOR_ACCESS(cv_ref),      \
                [](auto && S_c_L_v__) -> decltype(auto)                                        \
                { return ::std::forward<decltype(S_c_L_v__)>(S_c_L_v__).operator op(int{}); }, \
                ::std::remove_cvref_t<S_c_L_E__>::value(SCL_EXECUTOR_ACCESS(cv_ref)));         \
        }                                                                                       \
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
