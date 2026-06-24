#pragma once

#include <concepts>
#include <functional>
#include <iostream>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

#if defined(_MSC_VER)
#define SCL_DEBUG_FUNC_INFO __FUNCSIG__
#else
#define SCL_DEBUG_FUNC_INFO __PRETTY_FUNCTION__
#endif

#define SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)                  \
    explicit debug(self_type cv_ref other) noexcept(                         \
        ::std::is_nothrow_constructible_v<value_type, value_type cv_ref>)    \
        requires(::std::constructible_from<value_type, value_type cv_ref>)   \
        : m_value{::std::forward<value_type cv_ref>(other.m_value)}          \
    {                                                                        \
        ::std::cout << "[scl::debug] ctor: " << SCL_DEBUG_FUNC_INFO << '\n'; \
    }

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

namespace scl::feature::inplace
{
    /**
     * @ingroup scl_feature_wrapper
     * @brief Diagnostic executor that logs every operation to `std::cout`.
     *
     * Stores @p Value in-place identically to `plain`, but prints a
     * one-line message before each operation (constructor, value(), execute(),
     * guard(), unguard()). Intended for development and debugging only.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::debug> w{42};
     *    // prints: [scl::debug] ctor: ...
     *    int v = w->*[](int x){ return x; };
     *    // prints: [scl::debug] execute: ...
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
    template <typename Value>
    class debug
    {
        using self_type = debug<Value>;

    public:
        using value_type = Value;

    public:
        template <typename... Args>
        explicit debug(Args &&... args)
            : m_value{::std::forward<Args>(args)...}
        {
            ::std::cout << "[scl::debug] ctor: " << SCL_DEBUG_FUNC_INFO << '\n';
        }

        SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF

    public:
        template <typename Self, typename Func, typename... Args>
        static decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            ::std::cout << "[scl::debug] execute: " << SCL_DEBUG_FUNC_INFO << '\n';
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            ::std::cout << "[scl::debug] value: " << SCL_DEBUG_FUNC_INFO << '\n';
            return ::scl::forward_like<Self>(self.m_value);
        }

        template <typename Self>
        static void guard(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            ::std::cout << "[scl::debug] guard: " << SCL_DEBUG_FUNC_INFO << '\n';
        }

        template <typename Self>
        static void unguard(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            ::std::cout << "[scl::debug] unguard: " << SCL_DEBUG_FUNC_INFO << '\n';
        }

    private:
        value_type m_value;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
} // namespace scl::feature::inplace

#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF
#undef SCL_EXECUTOR_CONSTRUCTOR_FOR_SELF_PROTOTYPE
#undef SCL_DEBUG_FUNC_INFO
