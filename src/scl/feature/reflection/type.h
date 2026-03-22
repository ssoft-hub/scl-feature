#pragma once

/// @file
/// @brief Macro for declaring the wrapper type used by reflection macros.

#include <type_traits>

// clang-format off

/// @brief Declares the wrapper type for use by @c SCL_REFLECT_METHOD.
///
/// Must appear inside the class body **after** the executor member declaration
/// and **before** any @c SCL_REFLECT_METHOD invocations.
///
/// A specialization of @c scl::feature::executor_trait must also be provided
/// for @p Type.
///
/// @par Example
/// @code{.cpp}
///     SCL_REFLECT_TYPE(MyWrapper, m_executor);
/// @endcode
///
/// @param Type    The unqualified class name.
/// @param Member  Executor data member name.
///
/// @sa scl::feature::executor_trait
#define SCL_REFLECT_TYPE(Type, Member)                                                             \
    using S_c_L_type_ = Type;                                                                      \
    using S_c_L_executor_type_ = ::std::remove_cvref_t<decltype(Member)>

// clang-format on
