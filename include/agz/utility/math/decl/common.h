#pragma once

#include <algorithm>
#include <type_traits>

#include "../../common/common.h"

namespace agz::math {

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T clamp(T val, T min_v, T max_v) noexcept
{
    return (std::min)((std::max)(val, min_v), max_v);
}


template<typename T>
auto saturate(T val) noexcept
{
    return clamp<T>(val, T(0), T(1));
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T determinant(T m00, T m01,
              T m10, T m11) noexcept
{
    return m00 * m11 - m01 * m10;
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T determinant(T m00, T m01, T m02,
              T m10, T m11, T m12,
              T m20, T m21, T m22) noexcept
{
    return m00 * determinant(m11, m12,
                             m21, m22)
         - m01 * determinant(m10, m12,
                             m20, m22)
         + m02 * determinant(m10, m11,
                             m20, m21);
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T determinant(T m00, T m01, T m02, T m03,
              T m10, T m11, T m12, T m13,
              T m20, T m21, T m22, T m23,
              T m30, T m31, T m32, T m33) noexcept
{
    return m00 * determinant(m11, m12, m13,
                             m21, m22, m23,
                             m31, m32, m33)
         - m01 * determinant(m10, m12, m13,
                             m20, m22, m23,
                             m30, m32, m33)
         + m02 * determinant(m10, m11, m13,
                             m20, m21, m23,
                             m30, m31, m33)
         - m03 * determinant(m10, m11, m12,
                             m20, m21, m22,
                             m30, m31, m32);
}

template<typename T>
constexpr std::enable_if_t<std::is_floating_point_v<T>, T> PI = T(3.141592653589793238462643383);

template<typename T>
constexpr T invPI = 1 / PI<T>;

template<typename T>
constexpr T inv2PI = 1 / (2 * PI<T>);

template<typename T>
constexpr T inv4PI = 1 / (4 * PI<T>);

constexpr float  PI_f = PI<float>;
constexpr double PI_d = PI<double>;

using byte = unsigned char;

template<typename T>
T rad2deg(T rad) noexcept
{
    static_assert(std::is_floating_point_v<T>); return rad / PI<T> * 180;
}

template<typename T>
T deg2rad(T deg) noexcept
{
    static_assert(std::is_floating_point_v<T>); return deg / 180 * PI<T>;
}

template<typename T> class tcolor2;
template<typename T> class tcolor3;
template<typename T> class tcolor4;
template<typename T> class tmat3_c;
template<typename T> class tmat4_c;
template<typename T> class tvec2;
template<typename T> class tvec3;
template<typename T> class tvec4;

template<typename T, int D> class tvec;

} // namespace agz::math
