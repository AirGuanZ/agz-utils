#pragma once

#include <type_traits>
#include <vector>

namespace agz::math::distribution
{
    
/**
 * @brief ��[0, 1]��ĸ�����uת��Ϊ[beg, end)�е�����
 */
template<typename T, typename F, typename = std::enable_if_t<std::is_integral_v<T> && std::is_floating_point_v<F>>>
T uniform_integer(T beg, T end, F u);

/**
 * @brief ����ʱ�䰴��ɢ�ֲ��ɽ��в���
 * 
 * �μ� https://en.wikipedia.org/wiki/Alias_methods
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
     * @param prob ��ֲ��ɳɹ̶�����������
     * @param n �����С
     * 
     * Ӧ����prob[i] >= 0
     */
    void initialize(const F *prob, T n);

    bool available() const noexcept;

    void destroy();

    /**
     * @brief �Թ�һ�����prob����Ϊ�ֲ��ɲ���һ��1��n-1�������
     * @param u [0, 1]��ľ��������
     */
    T sample(F u) const noexcept;

    /**
     * @brief �Թ�һ�����prob����Ϊ�ֲ��ɲ���һ��1��n-1�������
     * @param u1 [0, 1]��ľ��������
     * @param u2 [0, 1]��ľ��������
     */
    T sample(F u1, F u2) const noexcept;
};

} // namespace agz::math::distribution
