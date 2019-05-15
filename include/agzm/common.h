#pragma once

#include <algorithm>
#include <type_traits>

#define AGZM_BEGIN namespace agzm {
#define AGZM_END   }

AGZM_BEGIN

struct uninitialized_t { };
inline uninitialized_t UNINIT;

template<typename T>
using rm_rcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T clamp(T val, T min_v, T max_v) noexcept
{
    return std::min(std::max(val, min_v), max_v);
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T determinant(T m00, T m01,
              T m10, T m11)
{
    return m00 * m11 - m01 * m10;
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T determinant(T m00, T m01, T m02,
              T m10, T m11, T m12,
              T m20, T m21, T m22)
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
              T m30, T m31, T m32, T m33)
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

AGZM_END
