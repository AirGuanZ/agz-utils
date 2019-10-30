#pragma once

#include <chrono>

namespace agz::time
{

class fps_counter_t
{
public:

    using clock = std::chrono::high_resolution_clock;
    using duration = clock::duration;
    using time_point = clock::time_point;

    fps_counter_t() noexcept
        : fps_counter_t(
            std::chrono::microseconds(1000000),
            std::chrono::microseconds(16667),
            std::chrono::microseconds(1000000))
    {

    }

    fps_counter_t(
        duration fps_interval,
        duration expected_update_interval,
        duration timeout_interval) noexcept
        : fps_counter_(0), fps_(0), last_update_interval_(expected_update_interval),
          fps_interval_(fps_interval), expected_update_interval_(expected_update_interval),
          update_timeout_interval_(timeout_interval)
    {
        time_point now = clock::now();
        last_update_time_ = now;
        fps_start_time_ = now;
    }

    void frame_end() noexcept
    {
        time_point now = clock::now();

        last_update_interval_ = now - last_update_time_;
        last_update_time_ = now;
        if(last_update_interval_ >= update_timeout_interval_)
            last_update_interval_ = expected_update_interval_;

        ++fps_counter_;
        if(now - fps_start_time_ > fps_interval_)
        {
            fps_ = fps_counter_;
            fps_counter_ = 0;
            fps_start_time_ = now;
        }
    }

    duration elasped_time() const noexcept
    {
        return last_update_interval_;
    }

    float elasped_microseconds() const noexcept
    {
        return float(std::chrono::duration_cast<std::chrono::microseconds>(
                        last_update_interval_).count());
    }

    int fps() const noexcept
    {
        return fps_;
    }

    void restart() noexcept
    {
        time_point now = clock::now();
        last_update_interval_ = expected_update_interval_;
        last_update_time_ = now;
        fps_start_time_ = now;
        fps_counter_ = 0;
        fps_ = 0;
    }

private:

    int fps_counter_;
    int fps_;
    duration last_update_interval_;

    duration fps_interval_;
    duration expected_update_interval_;
    duration update_timeout_interval_;

    time_point last_update_time_;
    time_point fps_start_time_;
};

} // namespace agz::time
