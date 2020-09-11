#pragma once

#include <utility>

#include "common.h"
#include "vec.h"

namespace agz::math
{

namespace spherical_harmonics
{

    /**
     * @brief 支持的最大L值
     */
    constexpr int L_MAX = 4;

    /**
     * @brief 计算球谐函数值
     * @tparam L 阶，满足0 <= L <= L_MAX
     * @tparam M 阶内编号，满足-L <= M <= L
     */
    template<int L, int M, typename T>
    auto eval(const tvec3<T> &v) noexcept;

    /**
     * @brief 将L和M转换为一个线性下标
     */
    constexpr int to_linear_index(int L, int M) noexcept;

    /**
     * @brief 将线性下标转回L和M
     */
    inline std::pair<int, int> from_linear_index(int index) noexcept;

    /**
     * @brief 球谐函数指针
     */
    template<typename T>
    using sh_func_t = T(*)(const tvec3<T>&) noexcept;

    /**
     * @brief 以线性下标进行索引的球谐函数指针表
     */
    template<typename T>
    sh_func_t<T> *linear_table() noexcept;

    /**
     * @brief 将向量投影到L阶球谐函数上，得到该阶的2L+1个球谐系数
     */
    template<int L, typename T>
    tvec<T, 2 * L + 1> project_to_sh(const tvec3<T> &v) noexcept;

    /**
     * @brief 将L阶球谐系数按指定旋转进行变换
     * @param rot 旋转矩阵
     * @param coefs 待变换系数，应包含2L+1个数据
     */
    template<int L, typename T>
    void rotate_sh_coefs(const tmat3_c<T> &rot, T *coefs) noexcept;

} // namespace spherical_harmonics

} // namespace agz::math
