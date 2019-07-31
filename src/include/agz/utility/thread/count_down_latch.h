#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "../misc/uncopyable.h"

namespace agz::thread
{
    
/**
 * @brief ���͵�count down latch��װ�������̵߳ȴ�ĳ����������Ϊ0
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
     * @brief ǿ������counterֵ�����ڸ�latch��������ã������������count_down
     */
    void set_counter(int counter)
    {
        std::lock_guard lk(mutex_);
        counter_ = counter;
    }

    /**
     * @brief �ȴ������̰߳�counter��Ϊ0
     */
    void wait()
    {
        std::unique_lock lk(mutex_);
        cond_.wait(lk, [&] { return counter_ <= 0; });
    }

    /**
     * @brief ʹcounter��һ����Ϊ0ʱ֪ͨ����counter�ϵ��߳�
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
