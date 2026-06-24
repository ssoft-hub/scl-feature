#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::heap
{
    /**
     * @brief Move-only heap executor that owns its value via @c std::unique_ptr.
     *
     * Stores @p Value on the heap with exclusive ownership.  Copy construction
     * and copy assignment are deleted; ownership transfers exclusively via move.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::heap::unique> w{42};
     *    auto w2 = ::std::move(w); // transfers ownership
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
    template <typename Value>
    class unique
    {
        using self_type = unique<Value>;

    public:
        using value_type = Value;
        using pointer = ::std::unique_ptr<value_type>;

    public:
        unique(unique const &) = delete;
        unique & operator=(unique const &) = delete;

        unique(unique &&) noexcept = default;
        unique & operator=(unique &&) noexcept = default;

        template <typename... Args>
        explicit unique(Args &&... args)
            : m_ptr{::std::make_unique<value_type>(::std::forward<Args>(args)...)}
        {}

    public:
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                ::std::invocable<Func, decltype(self_type::value(::std::forward<Self>(self))), Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func),
                self_type::value(::std::forward<Self>(self)), ::std::forward<Args>(args)...);
        }

        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(*self.m_ptr);
        }

    private:
        pointer m_ptr;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)

} // namespace scl::feature::heap
