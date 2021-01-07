#pragma once

#include <atomic>

#include "../misc/uncopyable.h"

namespace agz::thread
{
    
/**
 * @brief 标准自旋锁，没啥好说的，可用于std::lock_guard
 */
class spinlock_t : public misc::uncopyable_t
{
    std::atomic_flag mutex_;

public:

    spinlock_t()
    {
        mutex_.clear(std::memory_order_release);
    }

    void lock()
    {
        while(mutex_.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock()
    {
        mutex_.clear(std::memory_order_release);
    }
};

} // namespace agz::thread
