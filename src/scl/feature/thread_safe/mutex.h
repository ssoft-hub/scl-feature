#pragma once

#include <mutex>

#include <scl/feature/thread_safe/lock.h>

namespace scl::feature::thread_safe
{
    /**
     * @brief Locking policy for @c std::mutex.
     */
    template <>
    struct locking<::std::mutex>
    {
        static void lock(::std::mutex & m) { m.lock(); }
        static void unlock(::std::mutex & m) noexcept { m.unlock(); }
    };

    /**
     * @brief Thread-safe executor that guards every access with a @c std::mutex.
     *
     * Use as an executor for @c scl::wrapper to make all guarded accesses
     * mutually exclusive.  The mutex is never copied or moved — copy/move
     * of a @c mutex<Value> always creates a fresh mutex and transfers only
     * the value.
     *
     * @tparam Value  The wrapped value type.
     *
     * @code{.cpp}
     *    scl::wrapper<int, scl::feature::thread_safe::mutex> w{42};
     *    {
     *        auto g = scl::feature::wrapper_guard{w};
     *        g.value() = 100;
     *    }
     * @endcode
     */
    template <typename Value>
    using mutex = typename lock_executor<::std::mutex>::template type<Value>;

} // namespace scl::feature::thread_safe
