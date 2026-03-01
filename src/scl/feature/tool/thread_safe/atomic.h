#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_ATOMIC_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_ATOMIC_H

#include <scl/feature/tool/thread_safe/lock.h>

#include <atomic>

namespace scl::feature::ThreadSafe
{
    template <typename _Holder>
    struct Locking< ::std::atomic<bool>, _Holder>
    {
        template <typename _LockRefer>
        static constexpr void lock(_LockRefer && lock)
        {
            using LockRefer = _LockRefer &&;
            bool expected = false;
            while (!::std::forward<LockRefer>(lock).compare_exchange_weak(
                expected, true, ::std::memory_order_acquire))
                expected = false;
        }

        template <typename _LockRefer>
        static constexpr void unlock(_LockRefer && lock)
        {
            using LockRefer = _LockRefer &&;
            ::std::forward<LockRefer>(lock).store(false, ::std::memory_order_release);
        }
    };
} // namespace scl::feature::ThreadSafe

namespace scl::feature::ThreadSafe
{
    using Atomic = ::scl::feature::ThreadSafe::Lock< ::std::atomic<bool> >;
} // namespace scl::feature::ThreadSafe

#endif
