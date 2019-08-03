#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include "../misc/uncopyable.h"

namespace agz::thread
{
    
/**
 * @brief �򵥵Ķ��߳��������У��ɷ���ȡ�����ݣ�Ҳ��֪ͨ����
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
     * @brief Ĭ�ϴ�������״̬������ʱ�������ݣ��ɵ���stop()��ʾ����
     */
    blocking_queue_t()
        : stop_(false)
    {
        
    }

    /**
     * @brief ����һ�����ݲ����ѵ����������һ���߳�
     */
    void push(const data_t &data)
    {
        std::lock_guard lk(mut_);
        queue_.push(data);
        cond_.notify_one();
    }

    /**
     * @brief ����һ�����ݲ����ѵ����������һ���߳�
     */
    void push(data_t &&data)
    {
        std::lock_guard lk(mut_);
        queue_.push(std::move(data));
        cond_.notify_one();
    }

    /**
     * @brief ֪ͨ�����̲߳��ٻ����������ݵ������ý�����
     * 
     * ���ú�pop_or_stopһ�ɷ���std::nullopt
     */
    void stop()
    {
        stop_ = true;
        cond_.notify_all();
    }

    /**
     * @brief ������ȡ��һ�����ݣ����֪�����������ݵ���
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
