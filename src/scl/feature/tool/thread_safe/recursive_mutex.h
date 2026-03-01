#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_RECURSIVE_MUTEX_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_RECURSIVE_MUTEX_H

#include <scl/feature/tool/thread_safe/lock.h>

#include <mutex>

namespace scl::feature::ThreadSafe
{
    template <typename _Holder>
    struct Locking< ::std::recursive_mutex, _Holder>
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
} // namespace scl::feature::ThreadSafe

namespace scl::feature::ThreadSafe
{
    using RecursiveMutex = ::scl::feature::ThreadSafe::Lock< ::std::recursive_mutex>;
} // namespace scl::feature::ThreadSafe

#endif
