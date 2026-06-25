#pragma once

/// @file
/// @brief Executor that proxies calls directly to the held value with no overhead.
/// @ingroup scl_feature_executors

#include <concepts>
#include <functional>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

#define SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)                        \
    constexpr explicit plain(self_type cv_ref other) /**/                          \
        noexcept(::std::is_nothrow_constructible_v<value_type, value_type cv_ref>) \
        requires(::std::constructible_from<value_type, value_type cv_ref>)         \
        : m_value{::std::forward<value_type cv_ref>(other.m_value)}                \
    {}

/// @internal
/// @brief Expands to constructors for all eight cv-ref qualifications of `self_type`.
///
/// Suppresses the implicitly generated copy and move constructors so that
/// guard()/unguard() are called consistently for every cv-ref qualification.
#define SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF                         \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)                \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &)          \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)               \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const volatile &&)

#define SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(cv_ref)                        \
    constexpr plain & operator=(self_type cv_ref other) /**/                      \
        noexcept(::std::is_nothrow_assignable_v<value_type &, value_type cv_ref>) \
        requires(::std::assignable_from<value_type &, value_type cv_ref>)         \
    {                                                                             \
        m_value = ::scl::forward_like<self_type cv_ref>(other.m_value);           \
        return *this;                                                             \
    }

#define SCL_EXECUTOR_ASSIGNMENT_FOR_SELF                         \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(&)                \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &)          \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &)       \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &) \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(&&)               \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const &&)         \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(volatile &&)      \
    SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE(const volatile &&)

namespace scl::feature::inplace
{
    /**
     * @brief Executor that proxies calls directly to the held value with no overhead.
     * @ingroup scl_feature_executors
     *
     * Stores @p Value in-place without indirection or additional wrapping.
     * Serves as the default executor when none is specified.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::plain> w{42};
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward, cppcoreguidelines-c-copy-assignment-signature)
    template <typename Value>
    class plain
    {
        using self_type = plain<Value>;

    public:
        using value_type = Value;

    public:
        template <typename... Args>
        constexpr explicit plain(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {}

        SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF
        SCL_EXECUTOR_ASSIGNMENT_FOR_SELF

    public:
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) access(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }

    private:
        value_type m_value;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward, cppcoreguidelines-c-copy-assignment-signature)
} // namespace scl::feature::inplace

#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF
#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE
#undef SCL_EXECUTOR_ASSIGNMENT_FOR_SELF
#undef SCL_EXECUTOR_ASSIGNMENT_FOR_SELF_PROTOTYPE
