/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"

namespace uf
{   
    class spinlock
    {
        atomic_flag flag_ = ATOMIC_FLAG_INIT;

    public:
        bool try_lock()
        {
            return !flag_.test_and_set(std::memory_order_acquire);
        }

        template<typename Period = std::micro>
        void lock(i64 interval = 0)
        {
            while (flag_.test_and_set(std::memory_order_acquire))
            {
                if (interval)
                    this_thread::sleep_for(chrono::duration<i64, Period>(interval));
                else
                    this_thread::yield();
            }
        }

        void unlock()
        {
            flag_.clear(std::memory_order_release);
        }
    };
}
// namespace uf
