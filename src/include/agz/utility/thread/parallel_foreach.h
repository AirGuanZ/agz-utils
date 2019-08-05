#pragma once

#include <algorithm>
#include <mutex>
#include <optional>
#include <thread>

namespace agz::thread
{

/**
 * @brief ���ж�һ���ɵ��������е�ÿ��Ԫ��ִ��ָ������
 * 
 * @param iterable Ӧӵ��begin��end�����������ܵõ�forward iterator��iterating�в�Ӧ�׳��쳣
 * @param func Ҫִ�еĲ���
 * @param worker_count �����߳����������������
 *      1. n > 0����ʱ����n���߳�
 *      2. n <= 0����ʱ����max(1, hardware_thread_count - n)���߳�
 * @exception ��һ�������쳣�ᵼ�·������������ִ�У��ҵ�һ���쳣�ᱻԭ���׳�
 */
template<typename Iterable, typename Func>
void parallel_foreach(Iterable &&iterable, const Func &func, int worker_count = 0)
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

    if(worker_count <= 0)
        worker_count += static_cast<int>(std::thread::hardware_concurrency());
    worker_count = (std::max)(1, worker_count);

    auto worker_func = [&]
    {
        for(;;)
        {
            auto item = next_item();
            if(!item)
                break;

            try
            {
                func(*item);
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
        workers.emplace_back(worker_func);
    for(auto &w : workers)
        w.join();

    if(except_ptr)
        std::rethrow_exception(except_ptr);
}

} // namespace agz::thread
