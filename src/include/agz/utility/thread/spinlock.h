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
    std::atomic<bool> core_;

public:

    explicit spinlock_t(bool init_flag = false) noexcept;

    void lock()   noexcept;
    void unlock() noexcept;
};

inline spinlock_t::spinlock_t(bool init_flag) noexcept
    : core_(init_flag)
{

}

inline void spinlock_t::lock() noexcept
{
    bool val = false;
    while(!core_.compare_exchange_weak(val, true, std::memory_order_acquire))
        val = false;
}

inline void spinlock_t::unlock() noexcept
{
    core_.store(false, std::memory_order_release);
}

} // namespace agz::thread
