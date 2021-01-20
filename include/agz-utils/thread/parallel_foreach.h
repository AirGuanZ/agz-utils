#pragma once

#include <algorithm>
#include <mutex>
#include <optional>
#include <thread>

namespace agz::thread
{

/**
 * @brief 计算实际使用的并行线程数
 * @param worker_count 并行线程数，分两种情况：
 *      1. n > 0，此时创建n个线程
 *      2. n <= 0，此时创建max(1, hardware_thread_count - n)个线程
 */
inline int actual_worker_count(int worker_count) noexcept
{
    if(worker_count <= 0)
        worker_count += static_cast<int>(std::thread::hardware_concurrency());
    return (std::max)(1, worker_count);
}

/**
 * @brief 并行对一个可迭代对象中的每个元素执行指定操作
 * 
 * @param iterable 应拥有begin和end方法，
 *  至少能得到forward iterator；iterating中不应抛出异常
 * @param func 要执行的操作。func(int thread_index, Iterable it)
 * @param worker_count 并行线程数，分两种情况：
 *      1. n > 0，此时创建n个线程
 *      2. n <= 0，此时创建max(1, hardware_thread_count - n)个线程
 * @exception 任一任务发生异常会导致放弃后续任务的执行，且第一个异常会被原样抛出
 */
template<typename Iterable, typename Func>
void parallel_foreach(
    Iterable &&iterable, const Func &func, int worker_count = 0)
{
    std::mutex it_mutex;
    auto it = iterable.begin();
    auto end = iterable.end();
    auto next_item = [&]() -> decltype(std::make_optional(*it))
    {
        std::lock_guard lk(it_mutex);
        if(it == end)
            return std::nullopt;
        return std::make_optional(*it++);
    };

    std::mutex except_mutex;
    std::exception_ptr except_ptr = nullptr;

    worker_count = actual_worker_count(worker_count);

    auto worker_func = [&](int thread_index)
    {
        for(;;)
        {
            auto item = next_item();
            if(!item)
                break;

            try
            {
                func(thread_index, *item);
            }
            catch(...)
            {
                std::lock_guard lk(except_mutex);
                if(!except_ptr)
                    except_ptr = std::current_exception();
            }

            std::lock_guard lk(except_mutex);
            if(except_ptr)
                break;
        }
    };

    std::vector<std::thread> workers;
    for(int i = 0; i < worker_count; ++i)
        workers.emplace_back(worker_func, i);

    for(auto &w : workers)
        w.join();

    if(except_ptr)
        std::rethrow_exception(except_ptr);
}

// 和parallal_foreach相似，只是暂时这么写，以后改成parallal_foreach + range
// IMPROVE: use parallel_foreach + range
template<typename T, typename Func>
void parallel_forrange(T beg, T end, Func &&func, int worker_count = 0)
{
    std::mutex it_mutex;
    T it = beg;
    auto next_item = [&]() -> std::optional<T>
    {
        std::lock_guard lk(it_mutex);
        if(it == end)
            return std::nullopt;
        return std::make_optional(it++);
    };

    std::mutex except_mutex;
    std::exception_ptr except_ptr = nullptr;

    worker_count = actual_worker_count(worker_count);

    auto worker_func = [&](int thread_index)
    {
        for(;;)
        {
            auto item = next_item();
            if(!item)
                break;

            try
            {
                func(thread_index, *item);
            }
            catch(...)
            {
                std::lock_guard lk(except_mutex);
                if(!except_ptr)
                    except_ptr = std::current_exception();
            }

            std::lock_guard lk(except_mutex);
            if(except_ptr)
                break;
        }
    };

    std::vector<std::thread> workers;
    for(int i = 0; i < worker_count; ++i)
        workers.emplace_back(worker_func, i);

    for(auto &w : workers)
        w.join();

    if(except_ptr)
        std::rethrow_exception(except_ptr);
}

} // namespace agz::thread
