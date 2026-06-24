#pragma once

/**
 * @defgroup scl_feature_heap_indirect Heap Indirect Executor
 * @ingroup scl_feature_executors
 * @brief Heap-allocating executor with full value semantics.
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
     * @brief Executor that stores @p Value on the heap with full value semantics.
     *
     * Owns a `::std::unique_ptr<Value>` and implements deep-copy on
     * copy-construction and pointer-transfer on move-construction.
     *
     * @ingroup scl_feature_heap_indirect
     * @tparam Value  the wrapped value type
     *
     * @code{.cpp}
     *    wrapper<int, feature::heap::indirect> a{42};
     *    auto b = a;          // deep copy — distinct heap allocation
     *    auto c = std::move(a); // move — b owns the pointer, a.m_ptr is null
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions)
    template <typename Value>
    class indirect
    {
        using self_type = indirect<Value>;

    public:
        using value_type = Value;

    public:
        /// @brief Constructs the held value in-place from @p args.
        /// @tparam Args  argument types forwarded to @c Value
        /// @param  args  arguments forwarded to @c Value constructor
        template <typename... Args>
        explicit indirect(Args &&... args)
            : m_ptr{::std::make_unique<value_type>(::std::forward<Args>(args)...)}
        {}

        // --- copy: deep-copy the pointed-to value ---

        /// @brief Copy-constructs from another indirect; allocates a new object.
        /// @param other  source executor (lvalue)
        indirect(indirect const & other)
            : m_ptr{::std::make_unique<value_type>(*other.m_ptr)}
        {}

        /// @brief Copy-constructs from a mutable lvalue (delegates to const overload).
        /// @param other  source executor (mutable lvalue)
        // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
        indirect(indirect & other)
            : indirect{static_cast<indirect const &>(other)}
        {}

        // --- move: transfer pointer ownership ---

        /// @brief Move-constructs; transfers heap ownership, leaving source empty.
        /// @param other  source executor (rvalue)
        indirect(indirect && other) noexcept
            : m_ptr{::std::move(other.m_ptr)}
        {}

        // --- copy assignment ---

        /// @brief Copy-assigns; deep-copies the pointed-to value.
        /// @param other  source executor
        /// @return reference to @c *this
        indirect & operator=(indirect const & other)
        {
            if (this != &other)
                *m_ptr = *other.m_ptr;
            return *this;
        }

        /// @brief Copy-assigns from mutable lvalue (delegates to const overload).
        /// @param other  source executor
        /// @return reference to @c *this
        indirect & operator=(indirect & other)
        {
            return *this = static_cast<indirect const &>(other);
        }

        // --- move assignment ---

        /// @brief Move-assigns; transfers heap ownership.
        /// @param other  source executor (rvalue)
        /// @return reference to @c *this
        indirect & operator=(indirect && other) noexcept
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
        ::std::unique_ptr<value_type> m_ptr;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions)

} // namespace scl::feature::heap

/** @} */ // end of group scl_feature_heap_indirect
