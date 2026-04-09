#pragma once

/// @file
/// @brief Compile-time method reflection macros for wrapper types.
///
/// Provides macros to automatically generate proxy methods that forward calls
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
/// @brief Dispatches a method call through @c Executor::execute.
///
/// Builds the @c execute(exec, callable, args...) call used in every generated
/// overload body.  The callable is a lambda taking @c A_r_g_s__&&... that
/// applies @c scl::wrapper_cast to each argument and forwards the result to
/// @p method on the wrapped value.
///
/// @param method  Unqualified method name (plain identifier).
/// @param cv_ref  cv-ref qualifiers applied to the wrapper.
#define SCL_EXECUTE_OVERRIDED(method, cv_ref)                                       \
    S_c_L_executor_type_::execute(SCL_EXECUTOR_ACCESS(cv_ref),                      \
        [this](A_r_g_s__ &&... values)                                              \
            -> decltype(SCL_VALUE_ACCESS(cv_ref)                                    \
                .method(::scl::wrapper_cast(::std::forward<A_r_g_s__>(values))...)) \
        {                                                                           \
            return SCL_VALUE_ACCESS(cv_ref).method(                                 \
                ::scl::wrapper_cast(std::forward<A_r_g_s__>(values))...);           \
        },  ::std::forward<A_r_g_s__>(a_r_g_s__)...)

/// @internal
/// @brief Generates the @c method##_S_c_L_caller_ helper struct inside the enclosing class.
///
/// The struct exposes a single static function template
/// @c call\<P,Ps...\>(obj, args...) that invokes
/// @c obj.template @c method\<P,Ps...\>(scl::wrapper_cast(args)...).
/// Because @c obj is a dependent type, the @c template keyword is legal there
/// and name lookup is deferred to instantiation — this avoids a hard parse
/// error when @p method is not a template on the concrete wrapped type.
///
/// @param method  Unqualified method name (plain identifier).
#define SCL_REFLECT_METHOD_CALLER(method)                                                                       \
    struct method##_S_c_L_caller_                                                                               \
    {                                                                                                           \
        template <typename P_a_r_a_m___, typename... P_a_r_a_m_s___, typename O_b_j___, typename... A_r_g_s___> \
        static constexpr auto call(O_b_j___ && o_b_j___, A_r_g_s___ &&... a_r_g_s___)                           \
            -> decltype(::std::forward<O_b_j___>(o_b_j___).template method<P_a_r_a_m___, P_a_r_a_m_s___...>(    \
                ::scl::wrapper_cast(::std::forward<A_r_g_s___>(a_r_g_s___))...))                                \
        {                                                                                                       \
            return ::std::forward<O_b_j___>(o_b_j___).template method<P_a_r_a_m___, P_a_r_a_m_s___...>(         \
                ::scl::wrapper_cast(::std::forward<A_r_g_s___>(a_r_g_s___))...);                                \
        }                                                                                                       \
    };                                                                                                          \

/// @internal
/// @brief Generates the qualifier-discrimination predicates for @p method.
///
/// Produces two constructs inside the enclosing class:
///
/// - @c method##_S_c_L_quals_\<V,As...\> — variable template that evaluates
///   @c SCL_HAS_QUALIFIED_METHOD for the non-template (deduced-args) overload.
///   Defined once here; the 16 generated overloads reference it by instantiation
///   rather than re-expanding the macro body each time.
///
/// - @c method##_S_c_L_template_quals_\<P,Ps...\>::value\<V,As...\> — nested
///   variable template that evaluates @c SCL_HAS_QUALIFIED_METHOD with the token
///   sequence @c template @c method\<P,Ps...\> (assembled via @c SCL_FORWARD),
///   so the check accounts for explicit template arguments.
///
/// @param method  Unqualified method name (plain identifier).
#define SCL_REFLECT_METHOD_QUALS(method)                                          \
    template <typename S_c_L_V___, typename... S_c_L_A___>                        \
    static constexpr bool method##_S_c_L_quals_ =                                 \
        SCL_HAS_QUALIFIED_METHOD(SCL_FORWARD(method), S_c_L_V___, S_c_L_A___...); \
    template <typename S_c_L_P___, typename... S_c_L_Ps___>                       \
    struct method##_S_c_L_template_quals_                                         \
    {                                                                             \
        template <typename S_c_L_V___, typename... S_c_L_A___>                    \
        static constexpr bool value =                                             \
            SCL_HAS_QUALIFIED_METHOD(                                             \
                SCL_FORWARD(template method<S_c_L_P___, S_c_L_Ps___...>),         \
                S_c_L_V___, S_c_L_A___...);                                       \
    };

/// @internal
/// @brief Dispatches an explicit-template-args method call through @c Executor::execute.
///
/// Counterpart of @c SCL_EXECUTE_OVERRIDED for template methods.  The callable
/// uses @p caller to invoke @c .template method\<P_a_r_a_m__,...\> in a
/// type-dependent context.
///
/// @param caller  Helper struct generated by @c SCL_REFLECT_METHOD (the
///               @c method##_S_c_L_caller_ type).
/// @param cv_ref  cv-ref qualifiers applied to the wrapper.
#define SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref)                           \
    S_c_L_executor_type_::execute(SCL_EXECUTOR_ACCESS(cv_ref),                   \
        [this](A_r_g_s__ &&... values)                                           \
            -> decltype(caller::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(    \
                SCL_VALUE_ACCESS(cv_ref), ::std::forward<A_r_g_s__>(values)...)) \
        {                                                                        \
            return caller::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(         \
                SCL_VALUE_ACCESS(cv_ref), ::std::forward<A_r_g_s__>(values)...); \
        }, ::std::forward<A_r_g_s__>(a_r_g_s__)...)

/// @internal
/// @brief requires-clause + body for a non-template method overload.
///
/// The body dispatches through @c Executor::execute so that executor strategies
/// (e.g. locking executors) can intercept every call.
///
/// Both the callability check and the qualifier-discrimination check apply
/// @c scl::wrapper_cast to every argument before passing it to the wrapped
/// method.  This allows a @c wrapper<T> to be passed wherever a raw @c T is
/// expected — the same unwrapping that the call body performs at runtime.
#define SCL_REFLECT_METHOD_BASE(method, cv_ref)                             \
    noexcept (noexcept(SCL_EXECUTE_OVERRIDED(SCL_FORWARD(method), cv_ref))) \
    requires                                                                \
        requires                                                            \
        {                                                                   \
            SCL_VALUE_DECLVAL(cv_ref).method(                               \
                ::scl::wrapper_cast(::std::declval<A_r_g_s__>())...);       \
        } && method##_S_c_L_quals_<decltype(SCL_VALUE_DECLVAL(cv_ref)),     \
            decltype(::scl::wrapper_cast(::std::declval<A_r_g_s__>()))...>  \
    {                                                                       \
        return SCL_EXECUTE_OVERRIDED(SCL_FORWARD(method), cv_ref);          \
    }

/// @internal
/// @brief requires-clause + body for an explicit-template-args overload.
///
/// Uses @p caller (a struct whose static @c call\<P_a_r_a_m__,P_a_r_a_m_s__...\>(obj,args...)
/// wraps the @c .template method\<...\> call) so that the @c template keyword
/// appears only in a dependent context, avoiding a hard parse error when
/// @p method is not a template.
///
/// The body dispatches through @c Executor::execute so that executor strategies
/// (e.g. locking executors) can intercept every call.
///
/// @note Unlike @c SCL_REFLECT_METHOD_BASE, qualifier discrimination here uses
///       @c method##_S_c_L_template_quals_ rather than @c method##_S_c_L_quals_.
///       Internally @c method##_S_c_L_template_quals_\<P,Ps...\>::value\<V,As...\>
///       delegates to @c SCL_HAS_QUALIFIED_METHOD with the token sequence
///       @c template @c method\<P,Ps...\> (assembled via @c SCL_FORWARD) so that
///       the unevaluated call expressions inside the predicate include explicit
///       template arguments.  This is necessary for purely-template methods where
///       argument deduction without explicit arguments would fail and the check
///       would always return @c false.
#define SCL_REFLECT_TEMPLATE_METHOD_BASE(method, caller, cv_ref)                          \
    noexcept(noexcept(SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref)))                    \
    requires                                                                              \
        requires                                                                          \
        {                                                                                 \
            caller::template call<P_a_r_a_m__, P_a_r_a_m_s__...>(                        \
                SCL_VALUE_DECLVAL(cv_ref), ::std::declval<A_r_g_s__>()...);               \
        } && method##_S_c_L_template_quals_<P_a_r_a_m__, P_a_r_a_m_s__...>               \
                 ::template value<decltype(SCL_VALUE_DECLVAL(cv_ref)),                    \
                     decltype(::scl::wrapper_cast(::std::declval<A_r_g_s__>()))...>       \
    {                                                                                     \
        return SCL_EXECUTE_TEMPLATE_OVERRIDED(caller, cv_ref);                            \
    }

/// @internal
/// @brief Generates two overloads (deduced and explicit template args)
///        for one cv-ref qualifier.
#define SCL_REFLECT_METHOD_HELPER(method, caller, cv_ref)                             \
    template <typename... A_r_g_s__>                                                  \
    constexpr decltype(auto) method(A_r_g_s__ &&... a_r_g_s__) cv_ref                 \
        SCL_REFLECT_METHOD_BASE(method, cv_ref)                                       \
    template <typename P_a_r_a_m__, typename... P_a_r_a_m_s__, typename... A_r_g_s__> \
    constexpr decltype(auto) method(A_r_g_s__ &&... a_r_g_s__) cv_ref                 \
        SCL_REFLECT_TEMPLATE_METHOD_BASE(method, caller, cv_ref)

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
/// @par Executor dispatch
/// Every call is routed through @c Executor::execute(exec, callable, args...)
/// so that executor strategies (e.g. locking, tracing) can intercept it.
/// The callable receives the same @p args... and invokes the method on the
/// wrapped value.
///
/// The executor is obtained at runtime via
/// @c scl::feature::executor_trait\<WrapperType\>::executor(self).
/// There is no default implementation of @c executor_trait — every wrapper
/// type must provide an explicit specialization.
///
/// The wrapped value is then obtained by calling
/// @c Executor::value(executor_ref).
///
/// @par Constraint — callability
/// Each non-template overload has one @c requires clause with two conditions:
/// -# The wrapped object's @p method must be callable with the given
///    arguments (after @c scl::wrapper_cast) for the particular cv-ref
///    qualification.
/// -# @c SCL_HAS_QUALIFIED_METHOD must confirm that the wrapped object has
///    a **dedicated** overload for that qualifier (not an implicit
///    cv-widening fallback).  This prevents, for example, a @c const&
///    proxy from being generated when only a mutable @c & overload
///    exists on the target.
///
/// Explicit-template overloads use @c method##_S_c_L_template_quals_ for
/// qualifier discrimination instead of @c method##_S_c_L_quals_.  The struct
/// template forwards to @c SCL_HAS_QUALIFIED_METHOD with the token sequence
/// @c template @c method\<P,Ps...\> (assembled via @c SCL_FORWARD) so it works
/// even for purely-template methods where deduction without explicit arguments
/// would fail — see @c SCL_REFLECT_TEMPLATE_METHOD_BASE for details.
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
/// Internally this is handled by two helper constructs generated inside
/// the enclosing class by @c SCL_REFLECT_METHOD_CALLER and
/// @c SCL_REFLECT_METHOD_QUALS:
///
/// - @c method##_S_c_L_caller_ — a struct whose static
///   @c call\<P,Ps...\>(obj, args...) wraps the
///   @c .template @c method\<P,Ps...\>(args...) call.  The object
///   parameter (@c O_b_j___) is dependent, so the @c template keyword
///   appears only in a **dependent** context and name lookup is deferred
///   to instantiation.  Without this indirection,
///   @c .template @c foo\<...\> on a non-dependent type would be a hard
///   parse error (not SFINAE) when @c foo is not a template.
///
/// - @c method##_S_c_L_quals_ — a variable template that computes the
///   qualifier-discrimination predicate for the deduced-args overload.
///
/// - @c method##_S_c_L_template_quals_ — a struct template parameterised
///   over @c \<P,Ps...\> with an inner @c value\<V,As...\> variable template.
///   Delegates to @c SCL_HAS_QUALIFIED_METHOD with the token sequence
///   @c template @c method\<P,Ps...\> so that the predicate evaluates the
///   method call with concrete template arguments, handling purely-template
///   methods that do not have any deduction-friendly non-template overload.
///
/// When @p method is **not** a template, the @c call function fails to
/// instantiate (SFINAE via trailing return type), the @c requires clause
/// evaluates to @c false, and the explicit-template-args overload is
/// silently discarded.
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
#define SCL_REFLECT_METHOD(method)                                                           \
    SCL_REFLECT_METHOD_QUALS(method)                                                         \
    SCL_REFLECT_METHOD_CALLER(method)                                                        \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, &)                \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, &&)               \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const &)          \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const &&)         \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, volatile &)       \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, volatile &&)      \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const volatile &) \
    SCL_REFLECT_METHOD_HELPER(SCL_FORWARD(method), method##_S_c_L_caller_, const volatile &&)

// clang-format on
