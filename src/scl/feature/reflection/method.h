#pragma once

/// @file
/// @brief Compile-time method reflection macros for wrapper types.
///
/// Provides macros to automatically generate proxy methods that forward calls
/// from a wrapper class to the held object through an executor, preserving
/// cv-ref qualifiers exactly.
///
/// The executor is located at runtime via @c scl::feature::executor_trait,
/// which must be specialized for each wrapper type.
///
/// @par Requirements on the enclosing class
/// -# Declare the wrapper type with @c SCL_REFLECT_TYPE(Type,Member)
///    (after the executor member declaration).
/// -# Specialize @c scl::feature::executor_trait for the wrapper type.
/// -# The executor type must provide a static @c value(exec) method
///    that returns a reference to the wrapped object.
///
/// @par Example
/// @code{.cpp}
/// struct MyWrapper;
/// template <>
/// struct scl::feature::executor_trait<MyWrapper> {
///     template <typename Self>
///     static constexpr decltype(auto) executor(Self && self)
///     { return ::scl::forward_like<Self>(self.m_exec); }
/// };
/// struct MyWrapper {
///     Executor m_exec;
///     SCL_REFLECT_TYPE(MyWrapper, m_exec);
///
///     SCL_REFLECT_METHOD(foo)
///     SCL_REFLECT_METHOD(bar)
/// };
/// @endcode

#include <scl/feature/reflection/type.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/type_traits/has_qualified_method.h>
#include <scl/utility/preprocessor/forward.h>

#include <type_traits>
#include <utility>

/// @internal
/// @brief Obtains the wrapped value in an unevaluated context.
///
/// Uses the executor type directly (not the trait) so that the macro
/// works during class definition when the wrapper type is still incomplete.
///
/// @param cv_ref  cv-ref qualifiers applied to the executor.
#define SCL_VALUE_DECLVAL(cv_ref) \
    S_c_L_executor_type_::value(::std::declval<S_c_L_executor_type_ cv_ref>())

/// @internal
/// @brief Obtains the wrapped value in an evaluated context (function body).
///
/// Routes through @c executor_trait to locate the executor at runtime.
/// Safe here because member function bodies are processed after the class
/// is complete.
///
/// @param cv_ref  cv-ref qualifiers applied to the wrapper.
#define SCL_VALUE_ACCESS(cv_ref)                                                       \
    S_c_L_executor_type_::value(::scl::feature::executor_trait<S_c_L_type_>::executor( \
        ::std::forward<S_c_L_type_ cv_ref>(*this)))

/// @internal
/// @brief requires-clause + body for a non-template method overload.
#define SCL_REFLECT_METHOD_BASE(method, cv_ref)                                                \
    requires requires { SCL_VALUE_DECLVAL(cv_ref).method(::std::declval<A_r_g_s__>()...); } && \
        method##_S_c_L_quals_<decltype(SCL_VALUE_DECLVAL(cv_ref)), A_r_g_s__...>               \
    {                                                                                          \
        return SCL_VALUE_ACCESS(cv_ref).method(::std::forward<A_r_g_s__>(a_r_g_s__)...);       \
    }

/// @internal
/// @brief requires-clause + body for an explicit-template-args overload.
///
/// Uses @p CALLER (a struct whose static @c call\<P_a_r_a_m__,P_a_r_a_m_s__...\>(obj,args...)
/// wraps the @c .template method\<...\> call) so that the @c template keyword
/// appears only in a dependent context, avoiding a hard parse error when
/// @p method is not a template.
///
/// @note @c SCL_HAS_QUALIFIED_METHOD is intentionally omitted here: template
///       methods are not callable without explicit template arguments, so the
///       return-type discrimination check would always fail.  The CALLER's
///       SFINAE (via trailing return type) is sufficient to reject overloads
///       for qualifiers where the method is not available.
#define SCL_REFLECT_TEMPLATE_METHOD_BASE(method, CALLER, cv_ref)                                 \
    requires requires {                                                                          \
                 CALLER::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(SCL_VALUE_DECLVAL(cv_ref), \
                     ::std::declval<A_r_g_s__>()...);                                            \
             } {                                                                                 \
        return CALLER::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(SCL_VALUE_ACCESS(cv_ref),    \
            ::std::forward<A_r_g_s__>(a_r_g_s__)...);                                            \
    }

/// @internal
/// @brief Generates two overloads (deduced and explicit template args)
///        for one cv-ref qualifier.
#define SCL_REFLECT_METHOD_HELPER(method, CALLER, cv_ref)                                                      \
    template <typename... A_r_g_s__>                                                                           \
    constexpr decltype(auto) method(A_r_g_s__ &&... a_r_g_s__) cv_ref SCL_REFLECT_METHOD_BASE(method,          \
        SCL_FORWARD(cv_ref)) template <typename P_a_r_a_m__, typename... P_a_r_a_m_s__, typename... A_r_g_s__> \
    constexpr decltype(auto) method(A_r_g_s__ &&... a_r_g_s__)                                                 \
        cv_ref SCL_REFLECT_TEMPLATE_METHOD_BASE(SCL_FORWARD(method), CALLER, SCL_FORWARD(cv_ref))

/// @brief Generates proxy methods that reflect @p method from the wrapped object
///        through the executor, for all 8 cv-ref qualifier combinations.
///
/// @details
/// For each of the 8 cv-ref qualifiers (@c &, @c &&, @c const&, @c const&&,
/// @c volatile&, @c volatile&&, @c const @c volatile&, @c const @c volatile&&)
/// two overloads are generated:
/// - one with **deduced** template arguments —
///   @c template\<typename...\ A_r_g_s__\>
/// - one with **explicit** template arguments —
///   @c template\<typename\ P_a_r_a_m__,\ typename...\ P_a_r_a_m_s__,\ typename...\ A_r_g_s__\>
///
/// This gives 16 overloads in total (2 × 8).
///
/// @par Executor access
/// The executor is obtained at runtime via
/// @c scl::feature::executor_trait\<WrapperType\>::executor(self).
/// The default trait returns @c self.m_executor.
/// Specialize the trait for types that store the executor differently.
///
/// The wrapped value is then obtained by calling
/// @c Executor::value(executor_ref).
///
/// @par Constraint — callability
/// Each overload is constrained by two @c requires clauses:
/// -# The wrapped object's @p method must be callable with the given
///    arguments for the particular cv-ref qualification.
/// -# @c SCL_HAS_QUALIFIED_METHOD must confirm that the wrapped object has
///    a **dedicated** overload for that qualifier (not an implicit
///    cv-widening fallback).  This prevents, for example, a @c const&
///    proxy from being generated when only a mutable @c & overload
///    exists on the target.
///
/// @par Constraint — different return types required
/// Because @c SCL_HAS_QUALIFIED_METHOD relies on return-type discrimination,
/// overloads of the target method with different cv-ref qualifiers
/// **must** return different types.  If two overloads return the same
/// type, the macro will produce a false negative.
/// @code{.cpp}
/// struct Good {
///     short get() &;       // short ≠ int → distinguishable
///     int   get() const &;
/// };
/// struct Bad {
///     int get() &;         // int == int → macro cannot distinguish
///     int get() const &;
/// };
/// @endcode
///
/// @par Template methods
/// If @p method is a template on the wrapped object
/// (e.g. @c template\<typename\ T\>\ T\ convert()), users can call the
/// reflected method with explicit template arguments:
/// @c wrapper.convert\<double\>().
///
/// Internally this is handled by a helper struct
/// (@c METHOD_S_c_L_caller_) generated inside the enclosing class.
/// The struct wraps the @c .template @c method\<...\> call inside a
/// static function template whose object parameter (@c O_b_j___) is
/// dependent.  This ensures that the @c template keyword appears only
/// in a **dependent** context, so the compiler defers name lookup to
/// instantiation time.  Without this indirection,
/// @c .template @c foo\<...\> on a **non-dependent** type where @c foo
/// is not a template would be a hard parse error (not SFINAE), even
/// inside a @c requires expression.
///
/// When @p method is **not** a template, the helper struct's @c call
/// function fails to instantiate (SFINAE via trailing return type),
/// the @c requires clause evaluates to @c false, and the
/// explicit-template-args overload is silently discarded.
///
/// @par Example
/// @code{.cpp}
/// struct Target {
///     short get() &;
///     int   get() const &;
///     float get() &&;
///
///     template <typename T>
///     T convert() const &;
/// };
///
/// struct MyWrapper;
/// template <>
/// struct scl::feature::executor_trait<MyWrapper> {
///     template <typename Self>
///     static constexpr decltype(auto) executor(Self && self)
///     { return ::scl::forward_like<Self>(self.m_executor); }
/// };
/// struct MyWrapper {
///     ::scl::feature::inplace::plain<Target> m_executor;
///     SCL_REFLECT_TYPE(MyWrapper, m_executor);
///
///     explicit MyWrapper(int v) : m_executor{v} {}
///
///     SCL_REFLECT_METHOD(get)      // 16 overloads, constrained to &, const&, &&
///     SCL_REFLECT_METHOD(convert)  // 16 overloads, only const& survives constraints
/// };
///
/// MyWrapper w{42};
/// w.get();                // calls Target::get() &  → short(42)
///
/// MyWrapper const cw{42};
/// cw.get();               // calls Target::get() const &  → int(92)
///
/// MyWrapper{42}.get();    // calls Target::get() &&  → float(142)
///
/// cw.convert<double>();   // calls Target::convert<double>() const &  → 42.0
/// @endcode
///
/// @param method  Unqualified method name (plain identifier).
///
/// @sa SCL_REFLECT_TYPE
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_METHOD(method)                                                                              \
    struct method##_S_c_L_caller_                                                                               \
    {                                                                                                           \
        template <typename P_a_r_a_m___, typename... P_a_r_a_m_s___, typename O_b_j___, typename... A_r_g_s___> \
        static constexpr auto call(O_b_j___ && o_b_j___, A_r_g_s___ &&... a_r_g_s___)                           \
            -> decltype(::std::forward<O_b_j___>(o_b_j___).template method<P_a_r_a_m___, P_a_r_a_m_s___...>(    \
                ::std::forward<A_r_g_s___>(a_r_g_s___)...))                                                     \
        {                                                                                                       \
            return ::std::forward<O_b_j___>(o_b_j___).template method<P_a_r_a_m___, P_a_r_a_m_s___...>(         \
                ::std::forward<A_r_g_s___>(a_r_g_s___)...);                                                     \
        }                                                                                                       \
    };                                                                                                          \
    template <typename S_c_L_V___, typename... S_c_L_A___>                                                      \
    static constexpr bool method##_S_c_L_quals_ = SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(method),                 \
        S_c_L_V___, S_c_L_A___...);                                                                             \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, &)                                   \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, &&)                                  \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const &)                             \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const &&)                            \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, volatile &)                          \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, volatile &&)                         \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const volatile &)                    \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const volatile &&)
