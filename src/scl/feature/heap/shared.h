#pragma once

/**
 * @defgroup scl_feature_heap_shared Heap Shared Executor
 * @ingroup scl_feature_executors
 * @brief Heap-allocating executor with shared ownership semantics.
 * @{
 */

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::heap
{
    /**
     * @brief Executor that stores @p Value on the heap with shared ownership semantics.
     *
     * Owns a `::std::shared_ptr<Value>` and implements shallow copy on
     * copy-construction: copies share the same heap allocation.
     * Move-construction transfers pointer ownership, leaving the source empty.
     *
     * @ingroup scl_feature_heap_shared
     * @tparam Value  the wrapped value type
     *
     * @code{.cpp}
     *    wrapper<int, feature::heap::shared> a{42};
     *    auto b = a;           // shallow copy — same heap allocation
     *    // mutation through b is visible through a
     *    auto c = std::move(a); // move — c owns the pointer, a.m_ptr is null
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions)
    template <typename Value>
    class shared
    {
        using self_type = shared<Value>;

    public:
        using value_type = Value;

    public:
        /// @brief Constructs the held value in-place from @p args.
        /// @tparam Args  argument types forwarded to @c Value
        /// @param  args  arguments forwarded to @c Value constructor
        template <typename... Args>
        explicit shared(Args &&... args)
            : m_ptr{::std::make_shared<value_type>(::std::forward<Args>(args)...)}
        {}

        // --- copy: shallow — share the same allocation ---

        /// @brief Copy-constructs from another shared; shares the same heap object.
        /// @param other  source executor (const lvalue)
        shared(shared const & other) noexcept
            : m_ptr{other.m_ptr}
        {}

        /// @brief Copy-constructs from a mutable lvalue (delegates to const overload).
        /// @param other  source executor (mutable lvalue)
        // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
        shared(shared & other) noexcept
            : shared{static_cast<shared const &>(other)}
        {}

        // --- move ---

        /// @brief Move-constructs; transfers shared ownership, leaving source empty.
        /// @param other  source executor (rvalue)
        shared(shared && other) noexcept
            : m_ptr{::std::move(other.m_ptr)}
        {}

        /// @brief Move-constructs from a const rvalue (delegates to const copy).
        /// @param other  source executor (const rvalue)
        // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
        shared(shared const && other) noexcept
            : shared{static_cast<shared const &>(other)}
        {}

        // --- copy assignment: shallow ---

        /// @brief Copy-assigns; shares the same heap object.
        /// @param other  source executor
        /// @return reference to @c *this
        shared & operator=(shared const & other) noexcept
        {
            m_ptr = other.m_ptr;
            return *this;
        }

        /// @brief Copy-assigns from mutable lvalue (delegates to const overload).
        /// @param other  source executor
        /// @return reference to @c *this
        shared & operator=(shared & other) noexcept
        {
            return *this = static_cast<shared const &>(other);
        }

        // --- move assignment ---

        /// @brief Move-assigns; transfers shared ownership.
        /// @param other  source executor (rvalue)
        /// @return reference to @c *this
        shared & operator=(shared && other) noexcept
        {
            m_ptr = ::std::move(other.m_ptr);
            return *this;
        }

    public:
        /// @brief Accesses the held value with cv-ref qualifiers propagated from @p Self.
        /// @tparam Self  deduced executor type (carries the desired cv-ref)
        /// @param  self  the executor instance
        /// @return reference to the held value, qualified like @p Self
        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(*self.m_ptr);
        }

        /// @brief Invokes @p func with the held value and @p args.
        /// @tparam Self  deduced executor type
        /// @tparam Func  callable type
        /// @tparam Args  additional argument types
        /// @param  self  the executor instance
        /// @param  func  callable to invoke
        /// @param  args  additional arguments forwarded to @p func
        /// @return result of @c ::std::invoke(func, value(self), args...)
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                ::std::invocable<Func, decltype(value(::std::forward<Self>(self))), Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), value(::std::forward<Self>(self)),
                ::std::forward<Args>(args)...);
        }

    private:
        ::std::shared_ptr<value_type> m_ptr;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions)

} // namespace scl::feature::heap

/** @} */ // end of group scl_feature_heap_shared
