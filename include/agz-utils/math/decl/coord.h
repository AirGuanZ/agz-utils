#pragma once

#include "common.h"
#include "vec3.h"

AGZ_MATH_BEGIN

template<typename T>
class tcoord3
{
public:

    using axis_t = tvec3<T>;
    using self_t = tcoord3<T>;
    using vec_t  = tvec3<T>;

    axis_t x, y, z;

    /** @brief 默认初始化为(1, 0, 0), (0, 1, 0), (0, 0, 1) */
    tcoord3() noexcept;

    /** @brief 会自动对x，y，z进行归一化，不过并不对其施加正交化之类的保障其相互垂直的措施 */
    tcoord3(
        const axis_t &x, const axis_t &y, const axis_t &z) noexcept;

    /** @brief 保持内部成员为未初始化状态 */
    explicit tcoord3(uninitialized_t) noexcept;

    /** @brief 构造一个以给定向量的方向为x轴正方向的直角坐标系 */
    static self_t from_x(axis_t new_x) noexcept;

    /** @brief 构造一个以给定向量的方向为y轴正方向的直角坐标系 */
    static self_t from_y(axis_t new_y) noexcept;

    /** @brief 构造一个以给定向量的方向为z轴正方向的直角坐标系 */
    static self_t from_z(axis_t new_z) noexcept;

    /** @brief 将world space中的向量坐标转换到该坐标系中来 */
    vec_t global_to_local(
        const vec_t &global_vector) const noexcept;

    /** @brief 将local space中的向量坐标转换到world space中 */
    vec_t local_to_global(
        const vec_t &local_vector)  const noexcept;

    /** @brief 旋转该坐标系，使其x轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_x(
        const axis_t &new_x) const noexcept;

    /** @brief 旋转该坐标系，使其y轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_y(
        const axis_t &new_y) const noexcept;

    /** @brief 旋转该坐标系，使其z轴与给定向量为同一方向，返回得到的新坐标系 */
    self_t rotate_to_new_z(const axis_t &new_z) const noexcept;

    /** @brief 给定向量是否位于x轴所在的正半球 */
    bool in_positive_x_hemisphere(const vec_t &v) const noexcept;

    /** @brief 给定向量是否位于y轴所在的正半球 */
    bool in_positive_y_hemisphere(const vec_t &v) const noexcept;

    /** @brief 给定向量是否位于z轴所在的正半球 */
    bool in_positive_z_hemisphere(const vec_t &v) const noexcept;
};

template<typename T>
tcoord3<T> operator-(const tcoord3<T> &coord) noexcept;

AGZ_MATH_END
