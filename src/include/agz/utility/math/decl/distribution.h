#pragma once

#include <type_traits>
#include <vector>

namespace agz::math::distribution
{
    
/**
 * @brief 将[0, 1]间的浮点数u转换为[beg, end)中的整数
 */
template<typename T, typename F, typename = std::enable_if_t<std::is_integral_v<T> && std::is_floating_point_v<F>>>
T uniform_integer(T beg, T end, F u);

/**
 * @brief 常数时间按离散分布律进行采样
 * 
 * 参见 https://en.wikipedia.org/wiki/Alias_methods
 */
template<typename F, typename T = int>
class alias_sampler_t
{
    static_assert(std::is_floating_point_v<F> && std::is_integral_v<T>);

    struct table_unit
    {
        F accept_prob;
        T another_idx;
    };

    std::vector<table_unit> table_;

public:

    alias_sampler_t() = default;

    alias_sampler_t(const F *prob, T n);

    /**
     * @param prob 与分布律成固定比例的数组
     * @param n 数组大小
     * 
     * 应满足prob[i] >= 0
     */
    void initialize(const F *prob, T n);

    bool available() const noexcept;

    void destroy();

    /**
     * @brief 以归一化后的prob数组为分布律采样一个1到n-1间的整数
     * @param u [0, 1]间的均匀随机数
     */
    T sample(F u) const noexcept;

    /**
     * @brief 以归一化后的prob数组为分布律采样一个1到n-1间的整数
     * @param u1 [0, 1]间的均匀随机数
     * @param u2 [0, 1]间的均匀随机数
     */
    T sample(F u1, F u2) const noexcept;
};

} // namespace agz::math::distribution
