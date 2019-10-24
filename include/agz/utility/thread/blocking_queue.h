#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <vector>

#include "../misc/uncopyable.h"

namespace agz::thread
{

/**
 * @brief 简单的多线程阻塞队列，可放入取出数据，也可通知结束
 */
template<typename T>
class blocking_queue_t : public misc::uncopyable_t
{
    std::queue<T> queue_;

    std::mutex mut_;
    std::condition_variable cond_;

    std::atomic<bool> stop_;

public:

    using data_t = T;

    /**
     * @brief 默认处于启用状态，即随时接收数据；可调用stop()表示结束
     */
    blocking_queue_t()
        : stop_(false)
    {
        
    }

    /**
     * @brief 放入一个数据并唤醒等在这上面的一个线程
     */
    void push(const data_t &data)
    {
        std::lock_guard lk(mut_);
        queue_.push(data);
        cond_.notify_one();
    }

    /**
     * @brief 放入一个数据并唤醒等在这上面的一个线程
     */
    void push(data_t &&data)
    {
        std::lock_guard lk(mut_);
        queue_.push(std::move(data));
        cond_.notify_one();
    }

    /**
     * @brief 通知所有线程不再会有其他数据到来，该结束了
     */
    void stop()
    {
        stop_ = true;
        cond_.notify_all();
    }

    /**
     * @brief 阻塞地取出一个数据，或得知不会再有数据到来
     */
    std::optional<data_t> pop_or_stop()
    {
        std::unique_lock lk(mut_);

        while(queue_.empty() && !stop_)
            cond_.wait(lk);

        if(queue_.empty())
        {
            assert(stop_);
            return std::nullopt;
        }
        
        auto ret = std::move(queue_.front());
        queue_.pop();
        return std::make_optional(std::move(ret));
    }
};

/**
 * @brief 简单的多线程阻塞优先队列，可放入和取出数据，也可调用stop()表示结束
 */
template<typename T, typename Compare = std::less<T>>
class blocking_priority_queue_t
{
    std::priority_queue<T, std::vector<T>, Compare> queue_;

    std::mutex mut_;
    std::condition_variable cond_;

    std::atomic<bool> stop_;

public:

    using data_t = T;

    blocking_priority_queue_t()
        : stop_(false)
    {

    }

    /**
     * @brief 放入一个数据并通知一个等在该队列上的线程
     */
    void push(const data_t &data)
    {
        std::lock_guard lk(mut_);
        queue_.push(data);
        cond_.notify_one();
    }

    /**
     * @brief 放入一个数据并通知一个等在该队列上的线程
     */
    void push(data_t &&data)
    {
        std::lock_guard lk(mut_);
        queue_.push(std::move(data));
        cond_.notify_one();
    }

    /**
     * @brief 通知所有等在该队列上的线程结束
     */
    void stop()
    {
        stop_ = true;
        cond_.notify_all();
    }
    
    /**
     * @brief 阻塞地取出一个数据，或得知不会再有数据到来
     */
    std::optional<data_t> pop_or_stop()
    {
        std::unique_lock lk(mut_);

        while(queue_.empty() && !stop_)
            cond_.wait(lk);

        if(queue_.empty())
        {
            assert(stop_);
            return std::nullopt;
        }
        
        auto ret = std::move(queue_.front());
        queue_.pop();
        return std::make_optional(std::move(ret));
    }
};

} // namespace agz::thread
