#pragma once

#include <concepts>
#include <functional>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::thread_safe
{
    /**
     * @brief Customisation point for locking a specific lock type.
     *
     * Specialise this struct for each lock type used with @c lock_executor.
     * Each specialisation must provide:
     *   - `static void lock(LockType&)` — acquire the lock
     *   - `static void unlock(LockType&) noexcept` — release the lock
     *
     * @tparam LockType  The lock primitive type (e.g. @c std::mutex).
     */
    template <typename LockType>
    struct locking;

    /**
     * @brief CRTP base that turns any lock type into a thread-safe executor.
     *
     * Parameterised by @p LockType; nest @c lock_executor<LockType>::type<Value>
     * as an executor for @c scl::wrapper.  The lock is never copied or moved —
     * every copy/move of the executor creates a fresh default-constructed lock
     * and transfers only the value.
     *
     * @tparam LockType  A lock primitive that has a @c locking<LockType> specialisation.
     */
    template <typename LockType>
    struct lock_executor
    {
        template <typename Value>
        class type
        {
            using self_type = type<Value>;

        public:
            using value_type = Value;

        public:
            /// Forwarding constructor — default-constructs the lock, forwards args to Value.
            template <typename... Args>
            explicit type(Args &&... args)
                : m_lock{}
                , m_value{::std::forward<Args>(args)...}
            {}

            /// Copy — always creates a fresh lock; copies only the value.
            type(type const & o)
                : m_lock{}
                , m_value{o.m_value}
            {}

            /// Move — always creates a fresh lock; moves only the value.
            type(type && o) noexcept
                : m_lock{}
                , m_value{::std::move(o.m_value)}
            {}

            /// Copy-assignment — copies only the value; lock is untouched.
            type & operator=(type const & o)
            {
                if (this != &o)
                    m_value = o.m_value;
                return *this;
            }

            /// Move-assignment — moves only the value; lock is untouched.
            type & operator=(type && o) noexcept
            {
                if (this != &o)
                    m_value = ::std::move(o.m_value);
                return *this;
            }

            ~type() = default;

        public:
            /// Called by wrapper_guard on construction — acquires the lock.
            template <typename Self>
            static void guard(Self && self)
                requires(::std::same_as<::std::remove_cvref_t<Self>, self_type>)
            {
                locking<LockType>::lock(self.m_lock);
            }

            /// Called by wrapper_guard on destruction — releases the lock.
            template <typename Self>
            static void unguard(Self && self) noexcept
                requires(::std::same_as<::std::remove_cvref_t<Self>, self_type>)
            {
                locking<LockType>::unlock(self.m_lock);
            }

            /// Executes @p func with the supplied arguments (lock already held by wrapper_guard).
            template <typename Self, typename Func, typename... Args>
            static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
                requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
            {
                return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
            }

            /// Returns a reference to the held value, preserving cv-ref qualifiers of Self.
            template <typename Self>
            static constexpr decltype(auto) value(Self && self)
                requires(::std::same_as<::std::remove_cvref_t<Self>, self_type>)
            {
                return ::scl::forward_like<Self>(self.m_value);
            }

        private:
            mutable LockType m_lock{};
            value_type m_value;
        };
    };

} // namespace scl::feature::thread_safe
