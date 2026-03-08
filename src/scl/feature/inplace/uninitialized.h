#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::inplace
{
    /**
     * @brief Executor that holds @p Value in uninitialized aligned storage.
     *
     * Provides correctly sized and aligned storage without constructing or
     * destroying the object automatically, enabling deferred initialization.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::uninitialized> w{};
     * @endcode
     */
    template <typename Value>
    class uninitialized
    {
        using self_type = uninitialized<Value>;

    public:
        using value_type = Value;

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
            // reinterpret_cast<value_type*> for raw storage access;
            // const correctness is restored by forward_like<Self>.
            return ::scl::forward_like<Self>(*reinterpret_cast<value_type *>(self.m_storage)); // NOLINT(*-reinterpret-cast)
        }

    private:
        alignas(value_type)::std::byte m_storage[sizeof(value_type)]; // NOLINT(*-avoid-c-arrays)
    };
} // namespace scl::feature::inplace
