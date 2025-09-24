#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_MUTEX_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_MUTEX_H

#include <ScL/Feature/Tool/ThreadSafe/Lock.h>

#include <mutex>

namespace ScL::Feature::ThreadSafe
{
    template <typename _Holder>
    struct Locking< ::std::mutex, _Holder>
    {
        template <typename _LockRefer>
        static constexpr void lock(_LockRefer && lock)
        {
            using LockRefer = _LockRefer &&;
            ::std::forward<LockRefer>(lock).lock();
        }

        template <typename _LockRefer>
        static constexpr void unlock(_LockRefer && lock)
        {
            using LockRefer = _LockRefer &&;
            ::std::forward<LockRefer>(lock).unlock();
        }
    };
} // namespace ScL::Feature::ThreadSafe

namespace ScL::Feature::ThreadSafe
{
    using Mutex = ::ScL::Feature::ThreadSafe::Lock< ::std::mutex>;
} // namespace ScL::Feature::ThreadSafe

#endif
