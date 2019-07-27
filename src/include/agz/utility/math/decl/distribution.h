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
 * @brief ���ڶ��������ڳ���ʱ���ڰ���ɢ�ֲ��ɽ��в���
 *
 * �μ� https://en.wikipedia.org/wiki/Alias_methods
 */
template<typename F, typename T = int>
class bsearch_sampler_t
{
    static_assert(std::is_floating_point_v<F> && std::is_integral_v<T>);

    std::vector<F> partial_sum_;

public:

    bsearch_sampler_t() = default;

    bsearch_sampler_t(const F *prob, T n);

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
};

/**
 * @brief ����Alias Method�ڳ���ʱ���ڰ���ɢ�ֲ��ɽ��в���
 * 
 * FIXME
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

    using self_t = alias_sampler_t<F, T>;

    alias_sampler_t() = default;

    alias_sampler_t(const F *prob, T n);

    alias_sampler_t(self_t &&move_from) noexcept = default;

    alias_sampler_t(const self_t &copy_from) = default;

    self_t &operator=(self_t &&move_from) noexcept = default;

    self_t &operator=(const self_t &copy_from) = default;

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

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��λ�����ϵľ��ȷֲ���w.r.t. solid angle��
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_sphere(F u1, F u2) noexcept;

/**
 * @brief ��λ�����ϵľ��ȷֲ���Ӧ��pdf��w.r.t. solid angle��
 */
template<typename F>
constexpr F uniform_on_sphere_pdf = 1 / (4 * PI<F>);

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��λ�������ϵľ��ȷֲ���w.r.t. solid angle��
 * 
 * ��+z��һ��ĵ�λ���棩
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_hemisphere(F u1, F u2) noexcept;

/**
 * @brief ��λ�������ϵľ��ȷֲ���Ӧ��pdf��w.r.t. solid angle��
 */
template<typename F>
constexpr F uniform_on_hemisphere_pdf = 1 / (2 * PI<F>);

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��maxCosThetaΪ���ǵĵ�λ�����ϵ�׶���ڵķ����ϵľ��ȷֲ���w.r.t. solid angle��
 * 
 * @param max_cos_theta ��+zΪ׶�����ģ�theta�Ƿ���������+z�ļн�
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_cone(F max_cos_theta, F u1, F u2) noexcept;

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��maxCosThetaΪ���ǵĵ�λ�����ϵ�׶���ڵķ����ϵľ��ȷֲ���Ӧ��pdf��w.r.t. solid angle��
 */
template<typename F>
F uniform_on_cone_pdf(F max_cos_theta) noexcept;

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��λ����������zΪȨ�صķֲ���w.r.t. solid angle��
 * 
 * ��+z��һ��ĵ�λ���棩
 */
template<typename F>
std::pair<tvec3<F>, F> zweighted_on_hemisphere(F u1, F u2) noexcept;

/**
 * @brief ��λ����������zΪȨ�صķֲ���Ӧ��pdf��w.r.t. solid angle��
 */
template<typename F>
F zweighted_on_hemisphere_pdf(F z) noexcept;

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��������Ƭ�ϵľ��ȷֲ�������barycentric coordinate
 */
template<typename F>
tvec2<F> uniform_on_triangle(F u1, F u2) noexcept;

/**
 * @brief ��[0, 1]^2�ϵľ��ȷֲ�ת��Ϊ��λԲ���ϵľ��ȷֲ�
 */
template<typename F>
tvec2<F> uniform_on_unit_disk(F u1, F u2) noexcept;

/**
 * @brief �ڵ�λԲ���Ͼ��Ȳ�����Ӧ��pdf
 */
template<typename F>
constexpr F uniform_on_unit_disk_pdf = 1;

} // namespace agz::math::distribution
