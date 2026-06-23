#pragma once

#include <concepts>
#include <type_traits>

/// @internal
/// @brief Generates one constructor for `self_type cv_ref` via wrapper_constructor_resolver.
///
/// Covers all eight cv-ref combinations when expanded by SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
/// Using a macro eliminates eight near-identical definitions and eliminates the risk of
/// copy-paste mistakes (wrong cv-qualifier, wrong forward cast, wrong resolver argument).
// cppcheck-suppress noExplicitConstructor
#define SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)                               \
    constexpr wrapper(self_type cv_ref other) /**/                                       \
        noexcept(::std::is_nothrow_constructible_v<executor_type, executor_type cv_ref>) \
        requires(::std::constructible_from<executor_type, executor_type cv_ref>)         \
        : m_executor{::scl::forward_like<self_type cv_ref>(other.m_executor)}            \
    {}

/// @internal
/// @brief Expands to constructors for all eight cv-ref qualifications of `self_type`.
///
/// Suppresses the implicitly generated copy and move constructors so that
/// guard()/unguard() are called consistently for every cv-ref qualification.
#define SCL_WRAPPER_CONSTRUCTOR_FOR_SELF                         \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)                \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &)          \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)               \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_WRAPPER_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &&)

/// @internal
/// @brief Single forwarding-reference constructor for any other wrapper type.
///
/// A forwarding reference deduces all cv-ref combinations in one definition.
/// The requires clause restricts this to wrapper types other than self_type,
/// preventing it from competing with SCL_WRAPPER_CONSTRUCTOR_FOR_SELF.
// cppcheck-suppress noExplicitConstructor
#define SCL_WRAPPER_CONSTRUCTOR_FOR_OTHER                                                                           \
    template <typename Other>                                                                                       \
    constexpr wrapper(Other && other) /**/                                                                          \
        noexcept(noexcept(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve()))           \
        requires(::scl::feature::is_wrapper_v<Other> && !::std::same_as<::std::remove_cvref_t<Other>, self_type> && \
            ::std::constructible_from<executor_type,                                                                \
                decltype(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve())>)           \
        : m_executor{wrapper_constructor_resolver<self_type, Other &&>{::std::forward<Other>(other)}.resolve()}     \
    {}

/// @internal
/// @brief Generates one assignment operator for `self_type cv_ref` source.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)                               \
    constexpr wrapper & operator=(self_type cv_ref other) /**/                          \
        noexcept(::std::is_nothrow_assignable_v<executor_type &, executor_type cv_ref>) \
        requires(::std::assignable_from<executor_type &, executor_type cv_ref>)         \
    {                                                                                   \
        m_executor = ::scl::forward_like<self_type cv_ref>(other.m_executor);           \
        return *this;                                                                   \
    }

/// @internal
/// @brief Expands to assignment operators for all eight cv-ref qualifications of `self_type`.
#define SCL_WRAPPER_ASSIGNMENT_FOR_SELF                         \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(&)                \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &)          \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(&&)               \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_WRAPPER_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &&)

/// @internal
/// @brief Single forwarding-reference assignment for any other wrapper type.
#define SCL_WRAPPER_ASSIGNMENT_FOR_OTHER                                                                            \
    template <typename Other>                                                                                       \
    constexpr wrapper & operator=(Other && other) /**/                                                              \
        noexcept(noexcept(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve()))           \
        requires(::scl::feature::is_wrapper_v<Other> && !::std::same_as<::std::remove_cvref_t<Other>, self_type> && \
            ::std::assignable_from<executor_type &,                                                                 \
                decltype(::std::declval<wrapper_constructor_resolver<self_type, Other &&>>().resolve())>)           \
    {                                                                                                               \
        m_executor = wrapper_constructor_resolver<self_type, Other &&>{::std::forward<Other>(other)}.resolve();     \
        return *this;                                                                                               \
    }
