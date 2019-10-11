#pragma once

#include <chrono>
#include <iostream>

namespace agz::console
{

/**
 * @brief 适用于终端字符界面的进度条
 * 
 * 基本使用流程：构造时确定总任务数、进度条长度以及用于显示的字符
 * 使用display来刷新进度条
 * 使用done来关闭进度条
 * 使用operator++使已完成的任务数+1
 */
class progress_bar_t
{
    int finished_;
    int total_;

    int width_;
    char complete_;
    char incomplete_;

    std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();

public:

    progress_bar_t(int total, int width, char complete = '#', char incomplete = ' ')
        : finished_(0), total_(total), width_(width), complete_(complete), incomplete_(incomplete)
    {

    }

    progress_bar_t &operator++()
    {
        ++finished_;
        return *this;
    }

    void reset_time()
    {
        start_ = std::chrono::steady_clock::now();
    }

    void display() const
    {
        const float progress = float(finished_) / total_;
        const int pos = int(width_ * progress);

        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

        std::cout << "[";

        for(int i = 0; i < width_; ++i)
        {
            if(i < pos)
                std::cout << complete_;
            else if(i == pos)
                std::cout << ">";
            else
                std::cout << incomplete_;
        }
        std::cout << "] " << int(progress * 100.0) << "% "
            << float(time_elapsed) / 1000.0 << "s   \r";
        std::cout.flush();
    }

    void done()
    {
        display();
        if(finished_ <= total_)
        {
            std::cout << std::endl;
            ++finished_;
        }
    }
};

/**
 * @brief 基本同progress_bar_t，只不过直接以已完成的百分比为参数
 */
class progress_bar_f_t
{
    float percent_;

    int width_;
    char complete_;
    char incomplete_;

    std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();

public:

    explicit progress_bar_f_t(int width, char complete = '#', char incomplete = ' ')
        : percent_(0), width_(width), complete_(complete), incomplete_(incomplete)
    {

    }

    void set_percent(float percent)
    {
        percent_ = percent;
    }

    void reset_time()
    {
        start_ = std::chrono::steady_clock::now();
    }

    void display() const
    {
        const int pos = int(width_ * percent_ / 100);

        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

        std::cout << "[";

        for(int i = 0; i < width_; ++i)
        {
            if(i < pos)
                std::cout << complete_;
            else if(i == pos)
                std::cout << ">";
            else
                std::cout << incomplete_;
        }
        std::cout << "] " << int(percent_) << "% "
                  << float(time_elapsed) / 1000.0 << "s   \r";
        std::cout.flush();
    }

    void done()
    {
        set_percent(100);
        display();
        std::cout << std::endl;
    }
};

} // namespace AGZ

