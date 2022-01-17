#pragma once
#ifndef SCL_WRAPPER_TOOL_THREAD_SAFE_SHARED_MUTEX_H
#define SCL_WRAPPER_TOOL_THREAD_SAFE_SHARED_MUTEX_H

#include <ScL/Feature/Tool/ThreadSafe/Lock.h>

#if __cplusplus > 201700L
#   include <shared_mutex>
#else
#   include "private/SharedMutex.h"
#endif

namespace ScL
{
    using SharedMutex =
#if __cplusplus > 201700L
        ::std::shared_mutex
#else
    ::ScL::Feature::Detail::SharedMutex
#endif
    ;
}

namespace ScL { namespace Feature { namespace ThreadSafe
{
    template < typename _Holder >
    struct Locking< ::ScL::SharedMutex, _Holder >
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
    struct Locking< ::ScL::SharedMutex, const _Holder >
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
}}}

namespace ScL { namespace Feature { namespace ThreadSafe
{
    using SharedMutex = ::ScL::Feature::ThreadSafe::Lock< ::ScL::SharedMutex >;
}}}

#endif
