#pragma once

#include <cmath>

#include "../decl/func.h"

AGZ_MATH_BEGIN

template<typename T>
AGZ_MATH_API tquaternion_t<T>::tquaternion_t() noexcept
    : w(1), x(0), y(0), z(0)
{
    
}

template<typename T>
AGZ_MATH_API tquaternion_t<T>::tquaternion_t(T w, T x, T y, T z) noexcept
    : w(w), x(x), y(y), z(z)
{
    
}

template<typename T>
AGZ_MATH_API tquaternion_t<T>::tquaternion_t(
    const tvec3<T> &axis, T rad) noexcept
{
    const tvec3<T> normalized_axis = axis.normalize();
    const T half_theta = T(0.5) * rad;
    const T sin_angle = std::sin(half_theta);
    const T cos_angle = std::cos(half_theta);
    x = sin_angle * axis.x;
    y = sin_angle * axis.y;
    z = sin_angle * axis.z;
    w = cos_angle;
}

template<typename T>
AGZ_MATH_API typename tquaternion_t<T>::self_t
tquaternion_t<T>::normalize() const noexcept
{
    const T len = std::sqrt(x * x + y * y + z * z + w * w);
    if(len)
    {
        const T inv_len = 1 / len;
        return self_t(w * inv_len, x * inv_len, y * inv_len, z * inv_len);
    }
    return *this;
}

template<typename T>
AGZ_MATH_API typename tquaternion_t<T>::self_t
tquaternion_t<T>::conjugate() const noexcept
{
    return self_t(w, -x, -y, -z);
}

template<typename T>
AGZ_MATH_API tvec3<T> tquaternion_t<T>::apply_to_vector(
    const tvec3<T> &rhs) const noexcept
{
    const self_t rhs_q(0, rhs.x, rhs.y, rhs.z);
    const self_t inv_this = conjugate();
    const self_t ret = *this * rhs_q * conjugate();
    return tvec3<T>(ret.x, ret.y, ret.z);
}

template<typename T>
AGZ_MATH_API typename tquaternion_t<T>::self_t tquaternion_t<T>::operator*(
    const self_t &rhs) const noexcept
{
    return self_t(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
                  w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
                  w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
                  w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
}

template<typename T>
AGZ_MATH_API tquaternion_t<T> slerp(
    const tquaternion_t<T> &lhs, const tquaternion_t<T> &rhs, T interp_factor)
{
    T cos_theta = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    tquaternion_t<T> real_rhs = rhs;
    if(cos_theta < 0)
    {
        cos_theta = -cos_theta;
        real_rhs.x = -real_rhs.x;
        real_rhs.y = -real_rhs.y;
        real_rhs.z = -real_rhs.z;
        real_rhs.w = -real_rhs.w;
    }

    T lhs_w, rhs_w;
    if(1 - cos_theta > T(1e-4))
    {
        const T theta     = std::acos(cos_theta);
        const T sin_theta = std::sin(theta);
        lhs_w = std::sin((1 - interp_factor) * theta) / sin_theta;
        rhs_w = std::sin(interp_factor       * theta) / sin_theta;
    }
    else
    {
        lhs_w = 1 - interp_factor;
        rhs_w = interp_factor;
    }

    return tquaternion_t<T>(lhs_w * lhs.w + rhs_w * real_rhs.w,
                            lhs_w * lhs.x + rhs_w * real_rhs.x,
                            lhs_w * lhs.y + rhs_w * real_rhs.y,
                            lhs_w * lhs.z + rhs_w * real_rhs.z);
}

AGZ_MATH_END
