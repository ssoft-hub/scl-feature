#pragma once

/// @file
/// @brief Macro for declaring the wrapper type used by reflection macros.
/// @ingroup scl_feature_reflection

#include <type_traits>

/// @brief Declares the wrapper and executor types for use by @c SCL_REFLECT_METHOD.
/// @ingroup scl_feature_reflection
///
/// Must appear inside the class body **before** any @c SCL_REFLECT_METHOD
/// invocations.
///
/// A specialization of @c scl::feature::executor_trait must also be provided
/// for @p type.
///
/// @par Example
/// @code{.cpp}
///     SCL_REFLECT_TYPE(MyWrapper, MyExecutor);
/// @endcode
///
/// @param type      The unqualified wrapper class name.
/// @param executor  The unqualified executor type name.
///
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_TYPE(type, executor) \
    using S_c_L_type_ = type;            \
    using S_c_L_executor_type_ = executor;
