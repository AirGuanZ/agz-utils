#pragma once

#include <cassert>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

#include "../misc/uncopyable.h"

namespace agz::thread
{

/**
 * @brief 异步执行一系列任务
 * 
 * 
 * 分为running态/非running态，转换如下：
 * 
 * running -（join/stop）> 非running
 * 
 * 非running -（run_async）> running
 * 
 * 
 * 注意，在running态下，即使所有任务都被完成，只要没有调用join/stop，就不会进入非running态
 * 
 * running下可调用：finished/error
 * 
 * 非running下可调用：exceptions/error
 * 
 * 
 * 任何时候都可调用running查询处于哪个状态
 */
template<typename Task>
class queue_executer_t : public misc::uncopyable_t
{
    std::queue<Task> tasks_;
    std::vector<std::exception_ptr> exceptions_;
    std::vector<std::thread> workers_;

    std::mutex task_mutex_;
    std::mutex exception_mutex_;

    static void worker(
        std::function<void(const Task&)> f, queue_executer_t<Task> *p_this)
    {
        for(;;)
        {
            Task task;
            {
                std::lock_guard<std::mutex> lk(p_this->task_mutex_);
                if(p_this->tasks_.empty())
                    break;
                task = p_this->tasks_.front();
                p_this->tasks_.pop();
            }

            try
            {
                f(task);
            }
            catch(...)
            {
                std::lock_guard<std::mutex> lk(p_this->exception_mutex_);
                p_this->exceptions_.push_back(std::current_exception());
            }
        }
    }

public:

    ~queue_executer_t()
    {
        if(running())
            stop();
    }

    /**
     * @brief 计算以worker_count_param为参数时实际会开多少线程
     * 
     * 小于1时使用(硬件线程数 + worker_count)
     */
    static int actual_worker_count(int worker_count_param) noexcept
    {
        if(worker_count_param < 1)
            worker_count_param += static_cast<int>(
                std::thread::hardware_concurrency());
        return (std::max)(worker_count_param, 1);
    }

    /**
     * @param worker_count 异步线程数，小于1时使用(硬件线程数 + worker_count)
     * @param tasks 待执行的任务数据
     * @param f 用于执行任务的函数
     * 
     * 执行过程中抛出的异常被存入一个异常指针队列中
     * 
     * 进入running状态
     * 
     * assert(!running());
     */
    void run_async(
        int worker_count, std::queue<Task> &&tasks,
        std::function<void(const Task&)> f)
    {
        assert(!running());

        exceptions_.clear();
        tasks_ = std::move(tasks);

        worker_count = actual_worker_count(worker_count);
        for(int i = 0; i < worker_count; ++i)
            workers_.emplace_back(&queue_executer_t<Task>::worker, f, this);
    }

    /**
     * @brief 等待任务执行完毕
     * 
     * 将结束running状态
     * 
     * assert(running())
     */
    void join()
    {
        assert(running());
        for(auto &w : workers_)
        {
            if(w.joinable())
                w.join();
        }
        workers_.clear();
    }

    /**
     * @brief 终止后续任务的执行
     * 
     * 任务执行是原子的，所以这只是把剩下的任务队列清空而已
     * 
     * 将结束running状态
     * 
     * assert(running())
     */
    void stop()
    {
        assert(running());
        {
            std::queue<Task> empty_tasks;
            std::lock_guard lk(task_mutex_);
            tasks_.swap(empty_tasks);
        }
        return join();
    }

    /**
     * @brief 是否正确完成了所有任务
     * 
     * 在任务队列为空且无异常时返回true
     * 
     * assert(running());
     */
    bool finished() const
    {
        assert(running());

        bool task_empty;
        {
            std::lock_guard lk(task_mutex_);
            task_empty = tasks_.empty();
        }

        if(!task_empty)
            return false;

        return !error();
    }

    /**
     * @brief 是否发生了错误
     * 
     * 在running或非running状态下都可调用
     */
    bool error() const
    {
        std::lock_guard lk(exception_mutex_);
        return !exceptions_.empty();
    }

    /**
     * @brief 是否处于running状态
     */
    bool running() const
    {
        return !workers_.empty();
    }

    /**
     * @brief 取得所有发生的异常
     * 
     * assert(!running())
     * 
     * 会清空内部的异常队列
     */
    std::vector<std::exception_ptr> exceptions()
    {
        assert(!running());
        std::vector<std::exception_ptr> ret;
        ret.swap(exceptions_);
        return ret;
    }
};

} // namespace agz::thread
