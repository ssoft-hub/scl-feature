#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_SHARED_MUTEX_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_SHARED_MUTEX_H

#include <scl/feature/tool/thread_safe/lock.h>

#if __cplusplus > 201700L
#   include <shared_mutex>
#else
#include "private/shared_mutex.h"
#endif

namespace scl
{
    using SharedMutex =
#if __cplusplus > 201700L
        ::std::shared_mutex
#else
    ::scl::feature::detail::SharedMutex
#endif
    ;
}

namespace scl::feature::ThreadSafe
{
    template < typename _Holder >
    struct Locking< ::scl::SharedMutex, _Holder >
    {
        template < typename _LockRefer >
        static constexpr void lock ( _LockRefer && lock )
        {
            using LockRefer = _LockRefer &&;
            ::std::forward< LockRefer >( lock ).lock();
        }

        template < typename _LockRefer >
        static constexpr void unlock ( _LockRefer && lock )
        {
            using LockRefer = _LockRefer &&;
            ::std::forward< LockRefer >( lock ).unlock();
        }
    };

    template < typename _Holder >
    struct Locking< ::scl::SharedMutex, const _Holder >
    {
        template < typename _LockRefer >
        static constexpr void lock ( _LockRefer && lock )
        {
            using LockRefer = _LockRefer &&;
            ::std::forward< LockRefer >( lock ).lock_shared();
        }

        template < typename _LockRefer >
        static constexpr void unlock ( _LockRefer && lock )
        {
            using LockRefer = _LockRefer &&;
            ::std::forward< LockRefer >( lock ).unlock_shared();
        }
    };
}

namespace scl::feature::ThreadSafe
{
    using SharedMutex = ::scl::feature::ThreadSafe::Lock< ::scl::SharedMutex >;
}

#endif
