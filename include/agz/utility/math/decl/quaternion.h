#pragma once

#include "./vec3.h"

namespace agz::math
{

/**
 * @brief 四元数的简单实现
 */
template<typename T>
class tquaternion_t
{
    static_assert(std::is_floating_point_v<T>);

public:

    using self_t = tquaternion_t<T>;

    T a;
    tvec3<T> b;

    tquaternion_t()                       noexcept;
    tquaternion_t(T a, T b, T c, T d)     noexcept;
    tquaternion_t(T a, const tvec3<T> &b) noexcept;

    /**
     * @brief 根据旋转轴和弧度构造表示该旋转的四元数
     */
    static self_t construct_quaternion(const tvec3<T> &axis, T rad) noexcept;

    /**
     * @brief 将该四元数表示的旋转作用在给定向量上
     */
    tvec3<T> apply_to_vector(const tvec3<T> &vec) noexcept;

    self_t operator*(const self_t &rhs) const noexcept;

    self_t operator*(T rhs) const noexcept;
    self_t operator/(T rhs) const noexcept;

    self_t &operator*=(const self_t &rhs) noexcept;

    bool operator==(const self_t &rhs) const noexcept;
    bool operator!=(const self_t &rhs) const noexcept;

    T length_square() const noexcept;

    T length() const noexcept;

    self_t conjugate() const noexcept;

    self_t inverse() const noexcept;
};

template<typename T>
tquaternion_t<T> operator*(T lhs, const tquaternion_t<T> &rhs) noexcept;

template<typename T>
tquaternion_t<T> slerp(const tquaternion_t<T> &a, const tquaternion_t<T> &b, T interp_factor) noexcept;

} // namespace agz::math
