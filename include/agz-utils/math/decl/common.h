#pragma once

#include <algorithm>
#include <type_traits>

#include "../../common/common.h"

/*
    agz::math中的代码为了cuda兼容在对cpp特性的使用上做了一些妥协
*/

#ifdef __CUDACC__

#define AGZ_MATH_BEGIN namespace agz { namespace math {
#define AGZ_MATH_END   }}

#define AGZ_MATH_API // __host__ __device__

#define AGZ_MATH_MIN (::min)
#define AGZ_MATH_MAX (::max)

#else

#define AGZ_MATH_BEGIN namespace agz::math {
#define AGZ_MATH_END   }

#define AGZ_MATH_API

#define AGZ_MATH_MIN (std::min)
#define AGZ_MATH_MAX (std::max)

#endif

AGZ_MATH_BEGIN

template<typename T> class tcolor2;
template<typename T> class tcolor3;
template<typename T> class tcolor4;
template<typename T> class tmat3_c;
template<typename T> class tmat4_c;
template<typename T> class tvec2;
template<typename T> class tvec3;
template<typename T> class tvec4;

template<typename T, int D> class tvec;

using byte = unsigned char;

/**
 * @brief 限定给定标量的取值范围
 */
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
AGZ_MATH_API T clamp(T val, T min_v, T max_v) noexcept;

/**
 * @brief 将给定标量的取值范围限定至[0, 1]
 */
template<typename T>
AGZ_MATH_API auto saturate(T val) noexcept;

/**
 * @brief 求二阶行列式
 */
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
AGZ_MATH_API T determinant(
    T m00, T m01,
    T m10, T m11) noexcept;

/**
 * @brief 求三阶行列式
 */
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
AGZ_MATH_API T determinant(
    T m00, T m01, T m02,
    T m10, T m11, T m12,
    T m20, T m21, T m22) noexcept;

/**
 * @brief 求四阶行列式
 */
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
AGZ_MATH_API T determinant(
    T m00, T m01, T m02, T m03,
    T m10, T m11, T m12, T m13,
    T m20, T m21, T m22, T m23,
    T m30, T m31, T m32, T m33) noexcept;

/**
 * PI相关
 */

template<typename T>
constexpr std::enable_if_t<std::is_floating_point_v<T>, T>
    PI = T(3.141592653589793238462643383);

template<typename T>
constexpr T invPI = 1 / PI<T>;

template<typename T>
constexpr T inv2PI = 1 / (2 * PI<T>);

template<typename T>
constexpr T inv4PI = 1 / (4 * PI<T>);

constexpr float  PI_f = PI<float>;
constexpr double PI_d = PI<double>;

/**
 * @brief 弧度转角度
 */
template<typename T>
AGZ_MATH_API constexpr T rad2deg(T rad) noexcept;

/**
 * @brief 角度转弧度
 */
template<typename T>
AGZ_MATH_API constexpr T deg2rad(T deg) noexcept;

template<typename T, typename>
AGZ_MATH_API T clamp(T val, T min_v, T max_v) noexcept
{
    return (std::min)((std::max)(val, min_v), max_v);
}

template<typename T>
AGZ_MATH_API auto saturate(T val) noexcept
{
    return clamp<T>(val, T(0), T(1));
}

template<typename T, typename>
AGZ_MATH_API T determinant(
    T m00, T m01,
    T m10, T m11) noexcept
{
    return m00 * m11 - m01 * m10;
}

template<typename T, typename>
AGZ_MATH_API T determinant(
    T m00, T m01, T m02,
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

template<typename T, typename>
AGZ_MATH_API T determinant(
    T m00, T m01, T m02, T m03,
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
AGZ_MATH_API constexpr T rad2deg(T rad) noexcept
{
    static_assert(std::is_floating_point_v<T>, ""); return rad / PI<T> * 180;
}

template<typename T>
AGZ_MATH_API constexpr T deg2rad(T deg) noexcept
{
    static_assert(std::is_floating_point_v<T>, ""); return deg / 180 * PI<T>;
}

AGZ_MATH_END
