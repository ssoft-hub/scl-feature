#pragma once

/// @file
/// @brief Compile-time predicate macro for cv-ref qualifier detection.
/// @ingroup scl_feature_type_traits

#include <scl/utility/preprocessor/forward.h>

#include <type_traits>

/// @internal
/// @ingroup scl_feature_type_traits
/// @brief Call expression for @p method on @c Obj with qualifier @p Q.
///
/// Must appear only inside @c SCL_HAS_QUALIFIED_METHOD where @c Obj and
/// @c ScLCallArgs are in scope.
#define SCL_HQM_CALL(method, quals) \
    ::std::declval<Obj quals>().method(::std::declval<ScLCallArgs>()...)

/// @internal
/// @ingroup scl_feature_type_traits
/// @brief True when calls through qualifiers @p Q1 and @p Q2 return the same type.
///
/// Must appear only inside an @c if @c constexpr guard that ensures both
/// calls are well-formed.
#define SCL_HQM_SAME(method, left_quals, right_quals)                         \
    ::std::is_same_v<decltype(SCL_HQM_CALL(SCL_FORWARD(method), left_quals)), \
        decltype(SCL_HQM_CALL(SCL_FORWARD(method), right_quals))>

/// @brief Compile-time predicate: checks whether @p Type has a **dedicated**
/// @ingroup scl_feature_type_traits
///        overload of @p method whose cv-ref qualifiers exactly match those
///        of @p Type.
///
/// @details
/// In C++ a method qualified as @c const& is implicitly callable on a
/// mutable lvalue, because a mutable reference converts to a const one.
/// A simple @c requires expression therefore cannot distinguish "the
/// class has a @c const& overload" from "the class has a mutable @c &
/// overload that is also callable on @c const&".
///
/// This macro solves the problem by comparing **return types** of calls
/// with different cv-ref qualifiers.  If the return types differ, the
/// class must have separate overloads; if they are the same, the call is
/// assumed to go through implicit cv-widening and the predicate returns
/// @c false.
///
/// @par Algorithm
/// For each value category (lvalue / rvalue) the macro:
/// -# Checks callability for all 4 cv combinations
///    (@c mutable, @c const, @c volatile, @c const @c volatile).
/// -# For each pair where the "narrower" qualifier is callable through
///    a "wider" one, compares @c decltype of the two calls.
///    If both return the same type, the narrower call is considered
///    implicit — the overload does not exist for that qualifier.
/// -# For the rvalue @c const case an additional **cross-category**
///    check is performed (see MSVC note below).
///
/// @par Implementation note — @c if @c constexpr guards
/// Return-type comparisons (@c ::std::is_same_v) are placed inside
/// @c if @c constexpr blocks that first verify callability, ensuring
/// @c decltype is never evaluated on an ill-formed call expression.
/// This avoids the Clang limitation where @c requires @c { @c requires
/// @c EXPR; @c } inside a constexpr-evaluated lambda body is rejected.
///
/// @par Constraints — different return types required
/// Overloads of the same method name with different cv-ref qualifiers
/// **must** have different return types.  If two overloads return the
/// same type, the macro cannot tell them apart and will produce a
/// false negative (report no dedicated overload where one exists).
/// @code{.cpp}
/// struct Good {
///     bool  foo() &;        // bool  ≠ int  → distinguishable
///     int   foo() const &;
/// };
/// struct Bad {
///     int foo() &;          // int == int → indistinguishable!
///     int foo() const &;
/// };
/// @endcode
///
/// @par Constraints — volatile qualifiers
/// @c volatile overloads follow the same return-type discrimination
/// rules.  If the class has both @c volatile& and @c const @c volatile&
/// overloads, their return types must also differ.
///
/// @par Constraints — non-ref-qualified methods
/// A method without any ref-qualifier (e.g. @c void foo() const)
/// is callable on both lvalues and rvalues.  The macro treats such
/// methods as having dedicated overloads for **both** categories,
/// provided the return-type discrimination passes.
///
/// @par MSVC note — const& callable on rvalues
/// MSVC (as a non-standard extension, enabled by default) allows
/// @c const& ref-qualified member functions to be called on rvalues.
/// This means @c can_rv_c will be @c true even when no @c const&&
/// overload exists.  To compensate, the macro performs a
/// **cross-category** return-type comparison: it checks whether the
/// return type of calling through @c const&& differs from calling
/// through @c const&.  If they match and no @c mutable/const rvalue
/// return-type difference exists, the @c const&& call is deemed
/// implicit and the predicate returns @c false.
///
/// @par Example
/// @code{.cpp}
/// struct Target {
///     bool  get() &;
///     int   get() const &;
///     float get() &&;
/// };
///
/// // true  — dedicated & overload exists (bool ≠ int)
/// static_assert( SCL_HAS_QUALIFIED_METHOD(get, Target &));
/// // true  — dedicated const& overload exists
/// static_assert( SCL_HAS_QUALIFIED_METHOD(get, Target const &));
/// // true  — dedicated && overload exists
/// static_assert( SCL_HAS_QUALIFIED_METHOD(get, Target &&));
/// // false — no const&& overload; const& is called implicitly
/// static_assert(!SCL_HAS_QUALIFIED_METHOD(get, Target const &&));
/// @endcode
///
/// @param method   Unqualified method name (may contain @c template keyword for
///                 explicit template arguments).
/// @param Type     A cv-ref qualified type, e.g. @c const MyClass &.
/// @param ...      Parameter types for the method call (may be empty).

#define SCL_HAS_QUALIFIED_METHOD(method, Type, ...)                                                           \
    ([]<typename ScLObjType, typename... ScLCallArgs>() -> bool {                                             \
        using Obj = ::std::remove_cvref_t<ScLObjType>;                                                        \
        using Base = ::std::remove_reference_t<ScLObjType>;                                                   \
                                                                                                              \
        constexpr bool is_obj_rvalue = ::std::is_rvalue_reference_v<ScLObjType>;                              \
        constexpr bool is_obj_lvalue = ::std::is_lvalue_reference_v<ScLObjType>;                              \
        constexpr bool is_obj_const = ::std::is_const_v<Base>;                                                \
        constexpr bool is_obj_volatile = ::std::is_volatile_v<Base>;                                          \
        constexpr bool is_obj_cv = is_obj_const && is_obj_volatile;                                           \
                                                                                                              \
        /* --- lvalue callability --- */                                                                      \
        constexpr bool can_lv_m = requires { SCL_HQM_CALL(SCL_FORWARD(method), &); };                         \
        constexpr bool can_lv_c = requires { SCL_HQM_CALL(SCL_FORWARD(method), const &); };                   \
        constexpr bool can_lv_v = requires { SCL_HQM_CALL(SCL_FORWARD(method), volatile &); };                \
        constexpr bool can_lv_cv = requires { SCL_HQM_CALL(SCL_FORWARD(method), const volatile &); };         \
                                                                                                              \
        /* --- rvalue callability --- */                                                                      \
        constexpr bool can_rv_m = requires { SCL_HQM_CALL(SCL_FORWARD(method), &&); };                        \
        constexpr bool can_rv_c = requires { SCL_HQM_CALL(SCL_FORWARD(method), const &&); };                  \
        constexpr bool can_rv_v = requires { SCL_HQM_CALL(SCL_FORWARD(method), volatile &&); };               \
        constexpr bool can_rv_cv = requires { SCL_HQM_CALL(SCL_FORWARD(method), const volatile &&); };        \
                                                                                                              \
        /* --- lvalue qualifier detection --- */                                                              \
        /* Return-type comparisons (SCL_HQM_SAME) are guarded by if-constexpr */                              \
        /* callability checks so that decltype is never evaluated on an ill-formed call. */                   \
        if constexpr (is_obj_lvalue)                                                                          \
        {                                                                                                     \
            if constexpr (is_obj_cv)                                                                          \
            {                                                                                                 \
                return can_lv_cv;                                                                             \
            }                                                                                                 \
            else if constexpr (is_obj_volatile || is_obj_const)                                               \
            {                                                                                                 \
                constexpr bool can_narrow = is_obj_volatile ? can_lv_v : can_lv_c;                            \
                constexpr bool can_wider = can_lv_cv;                                                         \
                if constexpr (!can_narrow)                                                                    \
                    return false;                                                                             \
                else if constexpr (!can_wider)                                                                \
                    return true;                                                                              \
                else                                                                                          \
                    return !(is_obj_volatile                                                                  \
                            ? SCL_HQM_SAME(SCL_FORWARD(method), volatile &, const volatile &)                 \
                            : SCL_HQM_SAME(SCL_FORWARD(method), const &, const volatile &));                  \
            }                                                                                                 \
            else                                                                                              \
            {                                                                                                 \
                /* mutable lvalue: must differ from every wider qualifier that is callable. */                \
                if constexpr (!can_lv_m)                                                                      \
                    return false;                                                                             \
                else                                                                                          \
                {                                                                                             \
                    if constexpr (can_lv_c)                                                                   \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &, const &))                          \
                            return false;                                                                     \
                    }                                                                                         \
                    if constexpr (can_lv_v)                                                                   \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &, volatile &))                       \
                            return false;                                                                     \
                    }                                                                                         \
                    if constexpr (can_lv_cv)                                                                  \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &, const volatile &))                 \
                            return false;                                                                     \
                    }                                                                                         \
                    return true;                                                                              \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
        /* --- rvalue qualifier detection --- */                                                              \
        else if constexpr (is_obj_rvalue)                                                                     \
        {                                                                                                     \
            if constexpr (is_obj_cv)                                                                          \
            {                                                                                                 \
                return can_rv_cv;                                                                             \
            }                                                                                                 \
            else if constexpr (is_obj_volatile)                                                               \
            {                                                                                                 \
                if constexpr (!can_rv_v)                                                                      \
                    return false;                                                                             \
                else if constexpr (!can_rv_cv)                                                                \
                    return true;                                                                              \
                else                                                                                          \
                    return !SCL_HQM_SAME(SCL_FORWARD(method), volatile &&, const volatile &&);                \
            }                                                                                                 \
            else if constexpr (is_obj_const)                                                                  \
            {                                                                                                 \
                /* MSVC cross-category check: const& is callable on rvalues (non-standard). */                \
                /* Detect implicit binding by comparing return types across categories.     */                \
                if constexpr (!can_rv_c)                                                                      \
                    return false;                                                                             \
                else                                                                                          \
                {                                                                                             \
                    if constexpr (can_rv_cv)                                                                  \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), const &&, const volatile &&))         \
                            return false;                                                                     \
                    }                                                                                         \
                    if constexpr (can_lv_c)                                                                   \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), const &&, const &))                   \
                        {                                                                                     \
                            if constexpr (!can_rv_m)                                                          \
                                return false;                                                                 \
                            else                                                                              \
                            {                                                                                 \
                                constexpr bool scl_same_rv = SCL_HQM_SAME(SCL_FORWARD(method), &&, const &&); \
                                if constexpr (scl_same_rv)                                                    \
                                    return false;                                                             \
                            }                                                                                 \
                        }                                                                                     \
                    }                                                                                         \
                    return true;                                                                              \
                }                                                                                             \
            }                                                                                                 \
            else                                                                                              \
            {                                                                                                 \
                /* mutable rvalue: must differ from every wider qualifier that is callable. */                \
                if constexpr (!can_rv_m)                                                                      \
                    return false;                                                                             \
                else                                                                                          \
                {                                                                                             \
                    if constexpr (can_rv_c)                                                                   \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &&, const &&))                        \
                            return false;                                                                     \
                    }                                                                                         \
                    if constexpr (can_rv_v)                                                                   \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &&, volatile &&))                     \
                            return false;                                                                     \
                    }                                                                                         \
                    if constexpr (can_rv_cv)                                                                  \
                    {                                                                                         \
                        if constexpr (SCL_HQM_SAME(SCL_FORWARD(method), &&, const volatile &&))               \
                            return false;                                                                     \
                    }                                                                                         \
                    return true;                                                                              \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
        else                                                                                                  \
        {                                                                                                     \
            return false;                                                                                     \
        }                                                                                                     \
    }).template operator()<SCL_FORWARD(Type), ##__VA_ARGS__>()
