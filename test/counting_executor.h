#pragma once

/// @file counting_executor.h
/// @brief Shared test executor that counts guard()/unguard() calls.

#include <scl/utility/type_traits/forward_like.h>

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

struct guard_counters
{
    int guard_count = 0;
    int unguard_count = 0;
};

template <typename T>
struct counting_executor
{
    using value_type = T;

    explicit constexpr counting_executor(T v, guard_counters & counters)
        : m_value{v}
        , m_counters{counters}
    {}

    template <typename Self, typename Func, typename... Args>
    static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
    {
        return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
    }

    template <typename Self>
    static constexpr decltype(auto) access(Self && self)
        requires ::std::same_as<::std::remove_cvref_t<Self>, counting_executor>
    {
        return ::scl::forward_like<Self>(self.m_value);
    }

    template <typename Self>
    static constexpr void guard(Self & self)
        requires ::std::same_as<::std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.guard_count;
    }

    template <typename Self>
    static constexpr void unguard(Self & self)
        requires ::std::same_as<::std::remove_cvref_t<Self>, counting_executor>
    {
        ++self.m_counters.unguard_count;
    }

    T m_value;
    guard_counters & m_counters;
};
