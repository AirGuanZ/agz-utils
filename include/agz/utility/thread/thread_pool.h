#pragma once

#include <condition_variable>
#include <functional>
#include <thread>

#include "../misc/uncopyable.h"
#include "./blocking_queue.h"
#include "./parallel_foreach.h"

namespace agz::thread
{

/**
 * @brief 线程组，弱化版的线程池
 *
 * 内部的线程一旦创建，直到destroy才会销毁
 *
 * 用户可以阻塞或非阻塞地使用n个线程运行一个任务函数
 */
class thread_group_t : public misc::uncopyable_t
{
    using task_t = std::function<void(int)>;

    std::vector<std::thread> threads_;

    blocking_queue_t<task_t> task_queue_;

    std::mutex busy_task_cnt_mut_;
    std::condition_variable busy_task_cnt_cond_;
    int busy_task_cnt_;

    void worker_func(int thread_index);

public:

    explicit thread_group_t(int expected_thread_count = 1);

    ~thread_group_t();

    void reserve(int expected_thread_count);

    /**
     * @brief 阻塞地使用thread_count个线程并行执行func
     *
     * 所有被执行的func都返回后才返回
     */
    void run(int thread_count, std::function<void(int)> func);

    /**
     * @brief 异步地使用thread_count个线程并行执行func
     *
     * 不得在该thread_group正在进行别的同步或异步执行时调用
     */
    void run_async(int thread_count, std::function<void(int)> func);

    /**
     * @brief 等待异步执行完成
     */
    void join_async();
};

inline void thread_group_t::worker_func(int thread_index)
{
    for(;;)
    {
        auto opt_task = task_queue_.pop_or_stop();
        if(!opt_task)
            break;

        (*opt_task)(thread_index);

        std::lock_guard lk(busy_task_cnt_mut_);
        --busy_task_cnt_;
        busy_task_cnt_cond_.notify_all();
    }
}

inline thread_group_t::thread_group_t(int expected_thread_count)
    : busy_task_cnt_(0)
{
    reserve(expected_thread_count);
}

inline thread_group_t::~thread_group_t()
{
    task_queue_.stop();
    for(auto &t : threads_)
        t.join();
}

inline void thread_group_t::reserve(int thread_count)
{
    const int actual_thread_count = actual_worker_count(thread_count);
    const int current_thread_count = static_cast<int>(threads_.size());
    for(int i = current_thread_count; i < actual_thread_count; ++i)
        threads_.emplace_back(&thread_group_t::worker_func, this, i);
}

inline void thread_group_t::run(int thread_count, std::function<void(int)> func)
{
    run_async(thread_count, func);
    join_async();
}

inline void thread_group_t::run_async(
    int thread_count, std::function<void(int)> func)
{
    const int actual_thread_count = actual_worker_count(thread_count);

    {
        std::lock_guard lk(busy_task_cnt_mut_);
        busy_task_cnt_ = actual_thread_count;
    }

    reserve(thread_count);

    for(int i = 0; i < actual_thread_count; ++i)
        task_queue_.push(func);
}

inline void thread_group_t::join_async()
{
    std::unique_lock lk(busy_task_cnt_mut_);
    while(busy_task_cnt_ > 0)
        busy_task_cnt_cond_.wait(lk);
}

} // namespace agz::thread
