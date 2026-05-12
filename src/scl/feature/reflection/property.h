#pragma once

/// @file
/// @brief Compile-time property reflection macro for wrapper types.
/// @ingroup scl_feature_reflection
///
/// Provides @c SCL_REFLECT_PROPERTY to generate a data-member proxy that
/// forwards field access from the enclosing wrapper class to the held object
/// through the executor, preserving cv-ref qualifiers exactly.
///
/// @par Requirements on the enclosing class
/// The same requirements as @c SCL_REFLECT_METHOD apply:
/// -# Declare the executor data member.  It must precede every
///    @c SCL_REFLECT_PROPERTY: the generated member initializer reads the
///    executor through @c executor_trait, so the executor subobject has to be
///    constructed first (member initialization follows declaration order).
/// -# Place @c SCL_REFLECT_TYPE(Type, Member) after the executor declaration.
/// -# Specialize @c scl::feature::executor_trait for the wrapper type.
/// -# Use @c SCL_REFLECT_PROPERTY inside the class body after @c SCL_REFLECT_TYPE.
///
/// @par Difference from SCL_REFLECT_METHOD
/// @c SCL_REFLECT_PROPERTY exposes a **data field** (@c wrapper.prop) rather
/// than a **method** (@c wrapper.prop()).  Read access uses implicit conversion
/// via @c wrapper_cast(w.prop); write access uses
/// @c wrapper_cast(w.prop).to<FieldType&>() for fundamental types, or
/// @c operator= for class types that provide it.
///
/// @par ScLExec override
/// If the executor provides a static @c property_\<name\>(exec cv_ref) member
/// whose first parameter matches a specific cv-ref qualification **exactly**
/// (detected via a function-pointer cast, the same technique as @c has_execute_v),
/// property access for that qualification calls the override directly, bypassing
/// @c execute().  This is analogous to @c method_\<name\> overrides in
/// @c SCL_REFLECT_METHOD.
///
/// @par Copy / move safety
/// The generated member stores a byte offset instead of a raw pointer, so
/// memberwise copy and move work correctly without custom constructors.
///
/// @par Limitations
/// The offset is computed with @c reinterpret_cast (the @c container_of idiom),
/// which is not a constant expression.  Consequently a reflected property
/// member cannot be used in a constant expression, and the enclosing wrapper is
/// not @c constexpr-constructible while it carries @c SCL_REFLECT_PROPERTY
/// members.
///
/// @par Example
/// @code{.cpp}
/// struct PairWrapper;
/// template <>
/// struct scl::feature::executor_trait<PairWrapper> {
///     template <typename Self>
///     static constexpr decltype(auto) executor(Self && self)
///     { return ::scl::forward_like<Self>(self.m_impl); }
/// };
/// struct PairWrapper {
///     ::scl::feature::inplace::plain<std::pair<int,int>> m_impl;
///     SCL_REFLECT_TYPE(PairWrapper, m_impl);
///
///     SCL_REFLECT_PROPERTY(first)
///     SCL_REFLECT_PROPERTY(second)
/// };
///
/// PairWrapper w{std::pair{1, 2}};
/// wrapper_cast(w.first).to<int &>() = 42; // writes through executor
/// int x = wrapper_cast(w.first);          // reads through executor → 42
/// @endcode
///
/// @param prop  Unqualified data member name on the wrapped type.
///
/// @sa SCL_REFLECT_TYPE
/// @sa SCL_REFLECT_METHOD
/// @sa scl::feature::detail::property_execution

#include <scl/feature/detail/property_execution.h>
#include <scl/feature/reflection/type.h>
#include <scl/feature/type_traits/executor.h>
#include <scl/feature/wrapper.h>

#include <type_traits>
#include <utility>

// clang-format off

/// @internal
/// @brief Helper: the (non-reference) type returned by @c s_c_l_executor_type::value(exec&).
///
/// Does not require @c value_type to be declared on the executor.
#define SCL_EXECUTOR_VALUE_TYPE \
    ::std::remove_reference_t<decltype(s_c_l_executor_type::value(::std::declval<s_c_l_executor_type &>()))>

/// @internal
/// @brief Generates the executor-override tag struct for property @p prop.
/// @ingroup scl_feature_reflection
///
/// Produces @c property_##prop##_scl_tag inside the enclosing class.
/// The struct provides three members consulted by
/// @c scl::feature::detail::property_execution::holder:
///
/// - @c has_override\<ScLExec\> — @c true iff @c remove_cvref_t\<ScLExec\>::property_##prop
///   can be cast to an exact function pointer whose first parameter is @c ScLExec
///   (same function-pointer cast technique as @c has_execute_v and
///   @c method_##name##_scl_has_exec_override).
///
/// - @c override_noexcept\<ScLExec\> — @c true iff the above check passes and the
///   call is @c noexcept.
///
/// - @c call_override(ScLExec&&) — calls @c remove_cvref_t\<ScLExec\>::property_##prop
///   with the forwarded executor reference and returns its result.
///
/// @param prop  Unqualified data member name on the wrapped type.
#define SCL_REFLECT_PROPERTY_EXEC_HELPERS(prop)                                      \
    struct property_##prop##_scl_tag                                                 \
    {                                                                                \
        template <typename ScLExec>                                                  \
        static constexpr bool has_override =                                         \
            requires {                                                               \
                static_cast<                                                         \
                    decltype(::std::remove_cvref_t<ScLExec>::property_##prop(        \
                        ::std::declval<ScLExec>()))                                  \
                    (*)(ScLExec)                                                     \
                >(&::std::remove_cvref_t<ScLExec>::property_##prop);                 \
            };                                                                       \
        template <typename ScLExec>                                                  \
        static constexpr bool override_noexcept =                                    \
            []() constexpr noexcept -> bool {                                        \
                if constexpr (has_override<ScLExec>)                                 \
                    return noexcept(::std::remove_cvref_t<ScLExec>::property_##prop( \
                        ::std::declval<ScLExec>()));                                 \
                return false;                                                        \
            }();                                                                     \
        template <typename ScLExec>                                                  \
        static auto call_override(ScLExec && exec)                                   \
            -> decltype(::std::remove_cvref_t<ScLExec>::property_##prop(             \
                ::std::forward<ScLExec>(exec)))                                      \
        {                                                                            \
            return ::std::remove_cvref_t<ScLExec>::property_##prop(                  \
                ::std::forward<ScLExec>(exec));                                      \
        }                                                                            \
    };

/// @internal
/// @brief Deduces the @c wrapper type used for a reflected field @p prop.
///
/// Expands to @c scl::wrapper<FieldType, property_execution<ExecutorType, Tag>::holder>
/// where @c FieldType is @c remove_reference_t of the field's declared type,
/// @c ExecutorType is @c s_c_l_executor_type (set by @c SCL_REFLECT_TYPE), and
/// @c Tag is the tag struct generated by @c SCL_REFLECT_PROPERTY_EXEC_HELPERS.
///
/// Must appear after @c SCL_REFLECT_PROPERTY_EXEC_HELPERS(prop) in the same class body.
#define SCL_DECLTYPE_PROPERTY(prop)                                                          \
    ::scl::wrapper<                                                                          \
        ::std::remove_reference_t<decltype(::std::declval<SCL_EXECUTOR_VALUE_TYPE>().prop)>, \
        ::scl::feature::detail::property_execution<s_c_l_executor_type,                      \
            property_##prop##_scl_tag>::template holder                                      \
    >

/// @brief Generates a @c wrapper data member that reflects field @p prop of the
///        wrapped object through the executor.
///
/// @details
/// Expands to a tag struct followed by one data member inside the enclosing class:
///
/// @code
///   // 1. Tag struct for override detection (SCL_REFLECT_PROPERTY_EXEC_HELPERS):
///   struct property_prop_scl_tag { ... };
///
///   // 2. Data member (SCL_DECLTYPE_PROPERTY + initializer):
///   ::scl::wrapper<FieldType,
///       property_execution<ExecutorType, Tag>::holder> prop{
///       &executor_trait<s_c_l_type>::executor(*static_cast<s_c_l_type*>(this)),
///       &OuterValueType::prop
///   };
/// @endcode
///
/// @par ScLExec override
/// If the executor provides
/// @code
///   static R property_##prop(ScLExec cv_ref exec);
/// @endcode
/// then property access for that cv-ref qualification calls the override directly,
/// bypassing @c execute().  Other qualifications fall through to the execute path.
/// The override must return a type compatible with @c PropertyType (typically @c PropertyType&).
///
/// The inner wrapper's executor (@c property_execution<ExecutorType, Tag>::holder)
/// stores a signed byte offset from its own address to the outer executor, so
/// memberwise copy and move of the enclosing wrapper work correctly without
/// custom constructors.
///
/// @param prop  Unqualified data member name on the wrapped type.
///
/// @sa SCL_REFLECT_TYPE
/// @sa SCL_REFLECT_PROPERTY_EXEC_HELPERS
/// @sa scl::feature::detail::property_execution
#define SCL_REFLECT_PROPERTY(prop)                               \
    SCL_REFLECT_PROPERTY_EXEC_HELPERS(prop)                      \
    SCL_DECLTYPE_PROPERTY(prop)                                  \
    prop{&::scl::feature::executor_trait<s_c_l_type>::executor(  \
             *static_cast<s_c_l_type  *>(this)),                 \
         &SCL_EXECUTOR_VALUE_TYPE::prop};

// clang-format on
