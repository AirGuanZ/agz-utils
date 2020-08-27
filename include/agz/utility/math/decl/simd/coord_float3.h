#pragma once

#ifdef AGZ_UTILS_SSE

#include "./float3.h"

namespace agz::math
{

class _simd_coord_float3_t
{
public:
    
    using axis_t = float3;
    using self_t = _simd_coord_float3_t;
    using vec_t  = float3;

    axis_t x, y, z;

    /** @brief 默认初始化为(1, 0, 0), (0, 1, 0), (0, 0, 1) */
    _simd_coord_float3_t() noexcept;

    /** @brief 会自动对x，y，z进行归一化，不过并不对其施加正交化之类的保障其相互垂直的措施 */
    _simd_coord_float3_t(const axis_t &x, const axis_t &y, const axis_t &z) noexcept;

    /** @brief 保持内部成员为未初始化状态 */
    explicit _simd_coord_float3_t(uninitialized_t) noexcept;

    /** @brief 构造一个以给定向量的方向为x轴正方向的直角坐标系 */
    static self_t from_x(axis_t new_x) noexcept;

    /** @brief 构造一个以给定向量的方向为y轴正方向的直角坐标系 */
    static self_t from_y(axis_t new_y) noexcept;

    /** @brief 构造一个以给定向量的方向为z轴正方向的直角坐标系 */
    static self_t from_z(axis_t new_z) noexcept;

    /** @brief 将world space中的向量坐标转换到该坐标系中来 */
    vec_t global_to_local(const vec_t &global_vector) const noexcept;

    /** @brief 将local space中的向量坐标转换到world space中 */
    vec_t local_to_global(const vec_t &local_vector)  const noexcept;

    /** @brief 旋转该坐标系，使其x轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_x(const axis_t &new_x) const noexcept;

    /** @brief 旋转该坐标系，使其y轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_y(const axis_t &new_y) const noexcept;

    /** @brief 旋转该坐标系，使其z轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_z(const axis_t &new_z) const noexcept;

    /** @brief 给定向量是否位于x轴所在的正半球 */
    bool in_positive_x_hemisphere(const vec_t &v) const noexcept;

    /** @brief 给定向量是否位于y轴所在的正半球 */
    bool in_positive_y_hemisphere(const vec_t &v) const noexcept;

    /** @brief 给定向量是否位于z轴所在的正半球 */
    bool in_positive_z_hemisphere(const vec_t &v) const noexcept;

    // conv with coord

    _simd_coord_float3_t(const tcoord3<float> &c) noexcept
        : x(c.x), y(c.y), z(c.z) { }

    operator tcoord3<float>() const noexcept
    {
        return { x, y, z };
    }
};

_simd_coord_float3_t operator-(const _simd_coord_float3_t &coord) noexcept;

using float3_coord = _simd_coord_float3_t;

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
