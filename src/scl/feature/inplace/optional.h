#pragma once

#include <scl/feature/detail/wrapper.h>
#include <scl/utility/type_traits/forward_like.h>

#include <concepts>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace scl::feature::inplace
{
    /**
     * @brief Executor that holds @p Value in engaged/empty state, equivalent to @c std::optional.
     *
     * Stores @p Value in a union to avoid default-initialisation when empty.
     * Placement-new is used to engage the value; the destructor is called
     * explicitly on @c reset().
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    wrapper<int, feature::inplace::optional> w{};     // empty
     *    wrapper<int, feature::inplace::optional> w{42};   // engaged with 42
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
    template <typename Value>
    class optional
    {
        using self_type = optional<Value>;

    public:
        using value_type = Value;

    public:
        /// @brief Default constructor: empty state.
        constexpr optional() noexcept
            : m_empty{}
            , m_is_engaged{false}
        {}

        /// @brief Value constructor: engages the value in-place.
        template <typename... Args>
            requires(::std::constructible_from<value_type, Args...> &&
                        !(sizeof...(Args) == 1 && (::std::same_as<::std::remove_cvref_t<Args>, self_type> || ...)))
        constexpr explicit optional(Args &&... args)
            noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
            : m_empty{}
            , m_is_engaged{false}
        {
            ::new (static_cast<void *>(::std::addressof(m_value))) value_type{::std::forward<Args>(args)...};
            m_is_engaged = true;
        }

        /// @brief Copy constructor from lvalue.
        constexpr optional(self_type const & other) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
            requires(::std::copy_constructible<value_type>)
            : m_empty{}
            , m_is_engaged{false}
        {
            if (other.m_is_engaged)
            {
                ::new (static_cast<void *>(::std::addressof(m_value))) value_type{other.m_value};
                m_is_engaged = true;
            }
        }

        /// @brief Move constructor from rvalue.
        constexpr optional(self_type && other) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
            requires(::std::move_constructible<value_type>)
            : m_empty{}
            , m_is_engaged{false}
        {
            if (other.m_is_engaged)
            {
                ::new (static_cast<void *>(::std::addressof(m_value))) value_type{::std::move(other.m_value)};
                m_is_engaged = true;
            }
        }

        /// @brief Destructor: destroys the held value if engaged.
        constexpr ~optional() noexcept(::std::is_nothrow_destructible_v<value_type>) { reset(); }

    public:
        /// @brief Executor interface: returns the held value with cv-ref of @p Self.
        ///
        /// @throws std::bad_optional_access if not engaged.
        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            if (!self.m_is_engaged)
                throw ::std::bad_optional_access{};
            return ::scl::forward_like<Self>(self.m_value);
        }

        /// @brief Executor interface: invokes @p func with the held value.
        ///
        /// @throws std::bad_optional_access if not engaged.
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

    public:
        /// @brief Returns @c true if a value is held.
        [[nodiscard]]
        constexpr bool has_value() const noexcept
        {
            return m_is_engaged;
        }

        /// @brief Returns @c true if a value is held.
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept
        {
            return m_is_engaged;
        }

        /// @brief Destroys the held value, leaving the executor empty.
        constexpr void reset() noexcept(::std::is_nothrow_destructible_v<value_type>)
        {
            if (m_is_engaged)
            {
                m_value.~value_type(); // NOLINT(clang-diagnostic-trivially-destructible)
                m_is_engaged = false;
            }
        }

        /// @brief Destroys any held value and constructs a new one in-place.
        template <typename... Args>
            requires ::std::constructible_from<value_type, Args...>
        constexpr void
        emplace(Args &&... args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
        {
            reset();
            ::new (static_cast<void *>(::std::addressof(m_value))) value_type{::std::forward<Args>(args)...};
            m_is_engaged = true;
        }

        /// @brief Returns the held value or @p default_value if empty.
        template <typename Self, typename Default>
        static constexpr decltype(auto) value_or(Self && self, Default && default_value)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            if (self.m_is_engaged)
                return static_cast<::std::remove_reference_t<decltype(::scl::forward_like<Self>(
                    self.m_value))>>(::scl::forward_like<Self>(self.m_value));
            return static_cast<::std::remove_reference_t<decltype(::scl::forward_like<Self>(self.m_value))>>(
                ::std::forward<Default>(default_value));
        }

    private:
        union
        {
            struct
            {
            } m_empty; // NOLINT(clang-diagnostic-nested-anon-types)
            value_type m_value;
        };
        bool m_is_engaged;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)

} // namespace scl::feature::inplace

// ---- Partial specialisation of detail::wrapper to expose mixin methods ----

namespace scl::feature::detail
{
    /**
     * @brief Specialisation of @c wrapper for the @c inplace::optional executor.
     *
     * Inherits all constructor and storage behaviour from the primary template
     * and additionally exposes the optional-specific mixin interface:
     * @c has_value(), @c operator bool(), @c reset(), @c emplace(), @c value_or().
     */
    template <typename Value>
    class wrapper<Value, ::scl::feature::inplace::optional>
    {
        using self_type = wrapper<Value, ::scl::feature::inplace::optional>;
        using executor_type = ::scl::feature::inplace::optional<Value>;

        executor_type m_executor;

        friend struct executor_access;

    public:
        using value_type = Value;

        // --- Constructors ----------------------------------------------------

        /// @brief Default constructor: creates an empty optional wrapper.
        constexpr wrapper() noexcept
            : m_executor{}
        {}

        /// @brief Value constructor: engages the value in-place.
        template <typename... Args>
            requires(::std::constructible_from<Value, Args...> &&
                !(sizeof...(Args) == 1 && (::std::same_as<::std::remove_cvref_t<Args>, self_type> || ...)) &&
                !(sizeof...(Args) == 1 && (::std::same_as<::std::remove_cvref_t<Args>, executor_type> || ...)))
        constexpr explicit wrapper(Args &&... args)
            noexcept(::std::is_nothrow_constructible_v<executor_type, Args...>)
            : m_executor{::std::forward<Args>(args)...}
        {}

        /// @brief Construct from executor (copy/move of the executor directly).
        constexpr wrapper(executor_type const & exec)
            noexcept(::std::is_nothrow_copy_constructible_v<executor_type>)
            requires(::std::copy_constructible<executor_type>)
            : m_executor{exec}
        {}

        constexpr wrapper(executor_type && exec) noexcept(::std::is_nothrow_move_constructible_v<executor_type>)
            requires(::std::move_constructible<executor_type>)
            : m_executor{::std::move(exec)}
        {}

        constexpr wrapper(self_type const & other) noexcept(::std::is_nothrow_copy_constructible_v<executor_type>)
            requires(::std::copy_constructible<executor_type>)
            : m_executor{other.m_executor}
        {}

        constexpr wrapper(self_type && other) noexcept(::std::is_nothrow_move_constructible_v<executor_type>)
            requires(::std::move_constructible<executor_type>)
            : m_executor{::std::move(other.m_executor)}
        {}

        // --- Optional mixin interface ----------------------------------------

        /// @brief Returns @c true if a value is held.
        [[nodiscard]]
        constexpr bool has_value() const noexcept
        {
            return m_executor.has_value();
        }

        /// @brief Returns @c true if a value is held.
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept
        {
            return static_cast<bool>(m_executor);
        }

        /// @brief Destroys the held value, leaving the wrapper empty.
        constexpr void reset() noexcept(noexcept(m_executor.reset())) { m_executor.reset(); }

        /// @brief Destroys any held value and constructs a new one in-place.
        template <typename... Args>
            requires ::std::constructible_from<Value, Args...>
        constexpr void
        emplace(Args &&... args) noexcept(::std::is_nothrow_constructible_v<Value, Args...>)
        {
            m_executor.emplace(::std::forward<Args>(args)...);
        }

        /// @brief Returns the held value or @p default_value if empty.
        template <typename Default>
        constexpr decltype(auto) value_or(Default && default_value) &
        {
            return executor_type::value_or(m_executor, ::std::forward<Default>(default_value));
        }

        template <typename Default>
        constexpr decltype(auto) value_or(Default && default_value) const &
        {
            return executor_type::value_or(m_executor, ::std::forward<Default>(default_value));
        }

        template <typename Default>
        constexpr decltype(auto) value_or(Default && default_value) &&
        {
            return executor_type::value_or(::std::move(m_executor), ::std::forward<Default>(default_value));
        }

        template <typename Default>
        constexpr decltype(auto) value_or(Default && default_value) const &&
        {
            return executor_type::value_or(::std::move(m_executor), ::std::forward<Default>(default_value));
        }
    };

} // namespace scl::feature::detail
