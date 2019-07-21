#pragma once

#include <cassert>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

namespace agz::thread
{

/**
 * @brief �첽ִ��һϵ������
 * 
 * 
 * ��Ϊrunning̬/��running̬��ת�����£�
 * 
 * running -��join/stop��> ��running
 * 
 * ��running -��run_async��> running
 * 
 * 
 * ע�⣬��running̬�£���ʹ�������񶼱���ɣ�ֻҪû�е���join/stop���Ͳ�������running̬
 * 
 * running�¿ɵ��ã�finished/error
 * 
 * ��running�¿ɵ��ã�exceptions/error
 * 
 * 
 * �κ�ʱ�򶼿ɵ���running��ѯ�����ĸ�״̬
 */
template<typename Task>
class queue_executer_t
{
    std::queue<Task> tasks_;
    std::vector<std::exception_ptr> exceptions_;
    std::vector<std::thread> workers_;

    std::mutex task_mutex_;
    std::mutex exception_mutex_;

    static void worker(std::function<void(const Task&)> f, queue_executer_t<Task> *p_this)
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
     * @brief ������worker_count_paramΪ����ʱʵ�ʻῪ�����߳�
     * 
     * С��1ʱʹ��(Ӳ���߳��� + worker_count)
     */
    static int actual_worker_count(int worker_count_param) noexcept
    {
        if(worker_count_param < 1)
            worker_count_param += static_cast<int>(std::thread::hardware_concurrency());
        return (std::max)(worker_count_param, 1);
    }

    /**
     * @param worker_count �첽�߳�����С��1ʱʹ��(Ӳ���߳��� + worker_count)
     * @param tasks ��ִ�е���������
     * @param f ����ִ������ĺ���
     * 
     * ִ�й������׳����쳣������һ���쳣ָ�������
     * 
     * ����running״̬
     * 
     * assert(!running());
     */
    void run_async(int worker_count, std::queue<Task> &&tasks, std::function<void(const Task&)> f)
    {
        assert(!running());

        exceptions_.clear();
        tasks_ = std::move(tasks);

        worker_count = actual_worker_count(worker_count);
        for(int i = 0; i < worker_count; ++i)
            workers_.emplace_back(&queue_executer_t<Task>::worker, f, this);
    }

    /**
     * @brief �ȴ�����ִ�����
     * 
     * ������running״̬
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
     * @brief ��ֹ���������ִ��
     * 
     * ����ִ����ԭ�ӵģ�������ֻ�ǰ�ʣ�µ����������ն���
     * 
     * ������running״̬
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
     * @brief �Ƿ���ȷ�������������
     * 
     * ���������Ϊ�������쳣ʱ����true
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
     * @brief �Ƿ����˴���
     * 
     * ��running���running״̬�¶��ɵ���
     */
    bool error() const
    {
        std::lock_guard lk(exception_mutex_);
        return !exceptions_.empty();
    }

    /**
     * @brief �Ƿ���running״̬
     */
    bool running() const
    {
        return !workers_.empty();
    }

    /**
     * @brief ȡ�����з������쳣
     * 
     * assert(!running())
     * 
     * ������ڲ����쳣����
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
