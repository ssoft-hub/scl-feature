#pragma once
#ifndef SCL_TOOL_PRIVATE_SHATED_MUTEX_H
#define SCL_TOOL_PRIVATE_SHATED_MUTEX_H

#include <condition_variable>
#include <mutex>

namespace ScL::Feature::Detail
{
    /*
     * Copied implementation from Boost library
     */
    class SharedMutex
    {
    private:
        struct StateData
        {
            unsigned m_shared_count;
            bool m_exclusive;
            bool m_upgrade;
            bool m_exclusive_waiting_blocked;

            StateData()
                : m_shared_count()
                , m_exclusive()
                , m_upgrade()
                , m_exclusive_waiting_blocked()
            {}
        };

        StateData m_state;
        ::std::mutex m_mutex;
        ::std::condition_variable m_shared_condition;
        ::std::condition_variable m_exclusive_condition;

        void release_waiters()
        {
            m_exclusive_condition.notify_one();
            m_shared_condition.notify_all();
        }

    public:
        SharedMutex()
            : m_state()
        {}

        ~SharedMutex() {}

        void lock_shared()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            while (m_state.m_exclusive || m_state.m_exclusive_waiting_blocked)
                m_shared_condition.wait(locker);
            ++m_state.m_shared_count;
        }

        bool try_lock_shared()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            if (m_state.m_exclusive || m_state.m_exclusive_waiting_blocked)
                return false;
            ++m_state.m_shared_count;
            return true;
        }

        void unlock_shared()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            if (--m_state.m_shared_count == 0)
            {
                if (m_state.m_upgrade)
                {
                    m_state.m_upgrade = false;
                    m_state.m_exclusive = true;
                }
                else
                {
                    m_state.m_exclusive_waiting_blocked = false;
                }
                release_waiters();
            }
        }

        void lock()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            while (m_state.m_shared_count || m_state.m_exclusive)
            {
                m_state.m_exclusive_waiting_blocked = true;
                m_exclusive_condition.wait(locker);
            }
            m_state.m_exclusive = true;
        }

        bool try_lock()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            if (m_state.m_shared_count || m_state.m_exclusive)
            {
                return false;
            }
            else
            {
                m_state.m_exclusive = true;
                return true;
            }
        }

        void unlock()
        {
            ::std::unique_lock< ::std::mutex> locker(m_mutex);
            m_state.m_exclusive = false;
            m_state.m_exclusive_waiting_blocked = false;
            release_waiters();
        }
    };
} // namespace ScL::Feature::Detail

#endif
