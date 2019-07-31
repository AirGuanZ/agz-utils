#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "../misc/uncopyable.h"

namespace agz::thread
{
    
/**
 * @brief 典型的count down latch封装，可让线程等待某个计数器降为0
 */
class count_down_latch_t : public misc::uncopyable_t
{
    std::condition_variable cond_;
    std::mutex mutex_;
    int counter_;

public:

    explicit count_down_latch_t(int counter)
        : counter_(counter)
    {
        
    }

    /**
     * @brief 强制重设counter值，用于该latch对象的重用，不能用它替代count_down
     */
    void set_counter(int counter)
    {
        std::lock_guard lk(mutex_);
        counter_ = counter;
    }

    /**
     * @brief 等待其他线程把counter降为0
     */
    void wait()
    {
        std::unique_lock lk(mutex_);
        cond_.wait(lk, [&] { return counter_ <= 0; });
    }

    /**
     * @brief 使counter减一并在为0时通知等在counter上的线程
     */
    void count_down()
    {
        std::lock_guard lk(mutex_);
        assert(counter_);
        --counter_;
        if(counter_ <= 0)
            cond_.notify_all();
    }
};

} // namespace agz::thread
