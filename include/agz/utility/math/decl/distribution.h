#pragma once

#include <type_traits>
#include <vector>

namespace agz::math::distribution
{
    
/**
 * @brief 将[0, 1]间的浮点数u转换为[beg, end)中的整数
 */
template<typename T, typename F,
         typename = std::enable_if_t<
            std::is_integral_v<T> && std::is_floating_point_v<F>>>
T uniform_integer(T beg, T end, F u);

/**
 * @brief 基于二分搜索在常数时间内按离散分布律进行采样
 *
 * 参见 https://en.wikipedia.org/wiki/Alias_methods
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
};

/**
 * @brief 基于Alias Method在常数时间内按离散分布律进行采样
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

    using self_t = alias_sampler_t<F, T>;

    alias_sampler_t() = default;

    alias_sampler_t(const F *prob, T n);

    alias_sampler_t(self_t &&move_from) noexcept = default;

    alias_sampler_t(const self_t &copy_from) = default;

    self_t &operator=(self_t &&move_from) noexcept = default;

    self_t &operator=(const self_t &copy_from) = default;

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

/**
 * @brief 将[0, 1]^2上的均匀分布转换为单位球面上的均匀分布（w.r.t. solid angle）
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_sphere(F u1, F u2) noexcept;

/**
 * @brief 单位球面上的均匀分布对应的pdf（w.r.t. solid angle）
 */
template<typename F>
constexpr F uniform_on_sphere_pdf = 1 / (4 * PI<F>);

/**
 * @brief 将[0, 1]^2上的均匀分布转换为单位半球面上的均匀分布（w.r.t. solid angle）
 * 
 * （+z那一侧的单位球面）
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_hemisphere(F u1, F u2) noexcept;

/**
 * @brief 单位半球面上的均匀分布对应的pdf（w.r.t. solid angle）
 */
template<typename F>
constexpr F uniform_on_hemisphere_pdf = 1 / (2 * PI<F>);

/**
 * @brief 把[0, 1]^2上的均匀分布转换为以maxCosTheta为顶角的单位球面上的锥体内的方向上
 *  的均匀分布（w.r.t. solid angle）
 * 
 * @param max_cos_theta 以+z为锥体中心，theta是方向向量与+z的夹角
 */
template<typename F>
std::pair<tvec3<F>, F> uniform_on_cone(F max_cos_theta, F u1, F u2) noexcept;

/**
 * @brief 把[0, 1]^2上的均匀分布转换为以maxCosTheta为顶角的单位球面上的锥体内的方向上
 *  的均匀分布对应的pdf（w.r.t. solid angle）
 */
template<typename F>
F uniform_on_cone_pdf(F max_cos_theta) noexcept;

/**
 * @brief 将[0, 1]^2上的均匀分布转换为单位半球面上以z为权重的分布（w.r.t. solid angle）
 * 
 * （+z那一侧的单位球面）
 */
template<typename F>
std::pair<tvec3<F>, F> zweighted_on_hemisphere(F u1, F u2) noexcept;

/**
 * @brief 单位半球面上以z为权重的分布对应的pdf（w.r.t. solid angle）
 */
template<typename F>
F zweighted_on_hemisphere_pdf(F z) noexcept;

/**
 * @brief 将[0, 1]^2上的均匀分布转换为三角形面片上的均匀分布，返回barycentric coordinate
 */
template<typename F>
tvec2<F> uniform_on_triangle(F u1, F u2) noexcept;

/**
 * @brief 将[0, 1]^2上的均匀分布转换为单位圆盘上的均匀分布
 */
template<typename F>
tvec2<F> uniform_on_unit_disk(F u1, F u2) noexcept;

/**
 * @brief 在单位圆盘上均匀采样对应的pdf
 */
template<typename F>
constexpr F uniform_on_unit_disk_pdf = 1;

/**
 * @brief 将[0, 1]上的均匀分布转换为
 *  { begin, begin + 1, ..., end - 1 } \times [0, 1]上的均匀分布
 */
template<typename F, typename I>
std::pair<I, F> extract_uniform_int(F u, I begin, I end);

/**
 * @brief 将[0, 1]上的均匀分布转换为服从inv_cdf描述的cdf的分布
 * 
 * 记inverse CDF表格为A，大小为N，则A[0]对应CDF^{-1}(0)，A[N-1]对应
 *  CDF^{-1}(1)，中间的值用最近的表项作线性插值得到
 */
template<typename F>
F sample_inv_cdf_table(F u, const F *inv_cdf, size_t tab_size) noexcept;

} // namespace agz::math::distribution
