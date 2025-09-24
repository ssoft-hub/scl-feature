#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_ATOMIC_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_ATOMIC_H

#include <ScL/Feature/Tool/ThreadSafe/Lock.h>

#include <atomic>

namespace ScL::Feature::ThreadSafe
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
} // namespace ScL::Feature::ThreadSafe

namespace ScL::Feature::ThreadSafe
{
    using Atomic = ::ScL::Feature::ThreadSafe::Lock< ::std::atomic<bool> >;
} // namespace ScL::Feature::ThreadSafe

#endif
