#pragma once

/// @file
/// @brief Internal accessor macros for executor and value access in reflected methods.
/// @ingroup scl_feature_reflection

#include <scl/feature/type_traits/executor.h>

#include <type_traits>

/// @internal
/// @ingroup scl_feature_reflection
/// @brief Obtains the executor in an evaluated context (function body).
///
/// Routes through @c executor_trait so that wrappers that store the executor
/// in a non-default location (not @c m_executor) are handled correctly.
/// Safe here because member function bodies are processed after the class
/// is complete.
///
/// @c static_cast is used rather than @c std::forward because the reflection
/// members are generated inside a mixin base class (@c reflect and its specialisations),
/// where @c *this has the base type, not the derived wrapper type.  A
/// @c static_cast to @c S_c_L_type_ cv_ref correctly re-establishes the
/// wrapper's identity and value category before passing @c *this to
/// @c executor_trait, whereas @c std::forward would forward a reference to
/// the base subobject.
///
/// @param cv_ref  cv-ref qualifiers applied to the wrapper (@c *this).
#define SCL_EXECUTOR_ACCESS(cv_ref) \
    ::scl::feature::executor_trait<S_c_L_type_>::executor(static_cast<S_c_L_type_ cv_ref>(*this))

/// @internal
/// @ingroup scl_feature_reflection
/// @brief Obtains the wrapped value in an unevaluated context.
///
/// Uses the executor type directly (not the trait) so that the macro
/// works during class definition when the wrapper type is still incomplete.
///
/// @param cv_ref  cv-ref qualifiers applied to the executor.
#define SCL_VALUE_DECLVAL(cv_ref) \
    S_c_L_executor_type_::value(::std::declval<S_c_L_executor_type_ cv_ref>())

/// @internal
/// @ingroup scl_feature_reflection
/// @brief Obtains the wrapped value in an evaluated context (function body).
///
/// Calls @c Executor::value on the executor obtained via @c SCL_EXECUTOR_ACCESS.
/// Safe here because member function bodies are processed after the class
/// is complete.
///
/// @param cv_ref  cv-ref qualifiers applied to the wrapper (@c *this).
#define SCL_VALUE_ACCESS(cv_ref) S_c_L_executor_type_::value(SCL_EXECUTOR_ACCESS(cv_ref))
