#pragma once

#include <chrono>

namespace agz::time
{
   
/**
 * @brief 高精度计时器
 */
class clock_t
{
    using c = std::chrono::high_resolution_clock;

    c::time_point start_;

public:

    /** 被构造时算作一次重启 */
    clock_t() noexcept;

    /** 重启计时 */
    void restart() noexcept;

    /** 上一次重启以来的秒数 */
    uint64_t s() const noexcept;

    /** 上一次重启以来的毫秒数 */
    uint64_t ms() const noexcept;

    /** 上一次重启以来的微秒数 */
    uint64_t us() const noexcept;
};

inline clock_t::clock_t() noexcept
{
    restart();
}

inline void clock_t::restart() noexcept
{
    start_ = c::now();
}

inline uint64_t clock_t::s() const noexcept
{
    return std::chrono::duration_cast<std::chrono::seconds>(c::now() - start_).count();
}

inline uint64_t clock_t::ms() const noexcept
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(c::now() - start_).count();
}

inline uint64_t clock_t::us() const noexcept
{
    return std::chrono::duration_cast<std::chrono::microseconds>(c::now() - start_).count();
}

} // namespace agz::time
