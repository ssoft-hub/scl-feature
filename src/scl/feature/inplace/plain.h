#pragma once

#include <concepts>
#include <functional>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::inplace
{
    /**
     * @brief Executor that proxies calls directly to the held value with no overhead.
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

    public:
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(self.m_value);
        }

    private:
        value_type m_value;
    };
} // namespace scl::feature::inplace
