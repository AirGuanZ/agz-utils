#pragma once

namespace agz::math
{

/**
 * @brief helper class for online mean/variance computation
 */
template<typename F, typename I = int>
class variance_computer_t
{
    I n_ = 0;

    F old_m_ = 0;
    F new_m_ = 0;
    F old_s_ = 0;
    F new_s_ = 0;

public:

    /**
     * @brief clear all previous data
     */
    void clear() noexcept
    {
        n_ = 0;
        old_m_ = new_m_ = old_s_ = new_s_ = 0;
    }

    /**
     * @brief add a new sample value
     */
    void add(F value) noexcept
    {
        if(++n_ == 1)
        {
            old_m_ = value;
            new_m_ = value;
        }
        else
        {
            new_m_ = old_m_ + (value - old_m_) / n_;
            new_s_ = old_s_ + (value - old_m_) * (value - new_m_);

            old_m_ = new_m_;
            old_s_ = new_s_;
        }
    }

    /**
     * @brief how many sample values are accumulated since last calling of clear
     */
    I value_count() const noexcept
    {
        return n_;
    }

    /**
     * @brief (x1 + x2 + ... + xn) / n
     */
    F mean() const noexcept
    {
        return new_m_;
    }

    /**
     * @brief unbiased variance estimator
     *
     * let m = mean(), then:
     *
     * variance = ((x1 - m)^2 + (x2 - m)^2 + ... + (xn - m)^2) / (n - 1)
     */
    F variance() const noexcept
    {
        return n_ > 1 ? new_s_ / (n_ - 1) : F(0);
    }
};

} // namespace agz::math
