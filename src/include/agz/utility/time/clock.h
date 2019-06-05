#pragma once

#include <chrono>

namespace agz::time
{
   
/**
 * @brief �߾��ȼ�ʱ��
 */
class clock_t
{
    using c = std::chrono::high_resolution_clock;

    c::time_point start_;

public:

    /** ������ʱ����һ������ */
    clock_t() noexcept;

    /** ������ʱ */
    void restart() noexcept;

    /** ��һ���������������� */
    uint64_t s() const noexcept;

    /** ��һ�����������ĺ����� */
    uint64_t ms() const noexcept;

    /** ��һ������������΢���� */
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
