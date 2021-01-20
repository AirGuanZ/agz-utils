#pragma once

#include "thread_pool.h"

namespace agz::thread
{

/**
 * @brief 使用thread_group_t的parallel_forrange
 *
 * 参见./parallel_foreach.h中的parallel_foreach
 */
template<typename T, typename Func>
void parallel_forrange(
    T               beg,
    T               end,
    Func          &&func,
    thread_group_t &threads,
    int             worker_count = 0)
{
    std::mutex it_mutex;
    T it = beg;
    auto next_item = [&]()->std::optional<T>
    {
        std::lock_guard lk(it_mutex);
        if(it == end)
            return std::nullopt;
        return std::make_optional(it++);
    };

    std::mutex except_mutex;
    std::exception_ptr except_ptr = nullptr;

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

    threads.run(worker_count, worker_func);

    if(except_ptr)
        std::rethrow_exception(except_ptr);
}

} // namespace agz::thread
