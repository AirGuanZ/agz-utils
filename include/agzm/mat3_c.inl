#pragma once

#include "mat3_c.h"

AGZM_BEGIN

template<typename T>
tmat3_c<T>::tmat3_c() noexcept
    : tmat3_c(identity())
{
    
}

template<typename T>
tmat3_c<T>::tmat3_c(uninitialized_t) noexcept
{
    
}

template<typename T>
tmat3_c<T>::tmat3_c(T m00, T m01, T m02,
                    T m10, T m11, T m12,
                    T m20, T m21, T m22) noexcept
    : data{ { m00, m10, m20 },
            { m01, m11, m21 },
            { m02, m12, m22 } }
{
    
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::from_rows(const row_t &r0,
                                                  const row_t &r1,
                                                  const row_t &r2) noexcept
{
    return self_t(r0.x, r0.y, r0.z,
                  r1.x, r1.y, r1.z,
                  r2.x, r2.y, r2.z);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::from_cols(const col_t &c0,
                                                  const col_t &c1,
                                                  const col_t &c2) noexcept
{
    self_t ret = { c0, c1, c2 };
    return ret;
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::diag(T val) noexcept
{
    return self_t(val, 0,   0,
                  0,   val, 0,
                  0,   0,   val);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::all(T val) noexcept
{
    return self_t(val, val, val,
                  val, val, val,
                  val, val, val);
}

template<typename T>
const typename tmat3_c<T>::self_t& tmat3_c<T>::zero() noexcept
{
    static const self_t ret = all(0);
    return ret;
}

template<typename T>
const typename tmat3_c<T>::self_t& tmat3_c<T>::one() noexcept
{
    static const self_t ret = all(1);
    return ret;
}

template<typename T>
const typename tmat3_c<T>::self_t& tmat3_c<T>::identity() noexcept
{
    static const self_t ret = diag(1);
    return ret;
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::translate(const tvec2<T> &offset) noexcept
{
    return translate(offset.x, offset.y);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::translate(T x, T y) noexcept
{
    return self_t(1, 0, x,
                  0, 1, y,
                  0, 0, 1);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::rotate(const tvec3<T> &_axis, T rad) noexcept
{
    tvec3<T> axis = _axis.normalize();
    T sinv = std::sin(rad), cosv = std::cos(rad);

    self_t ret(UNINIT);

    ret[0][0] = axis.x * axis.x + (1 - axis.x * axis.x) * cosv;
    ret[0][1] = axis.x * axis.y * (1 - cosv) - axis.z * sinv;
    ret[0][2] = axis.x * axis.z * (1 - cosv) + axis.y * sinv;

    ret[1][0] = axis.x * axis.y * (1 - cosv) + axis.z * sinv;
    ret[1][1] = axis.y * axis.y + (1 - axis.y * axis.y) * cosv;
    ret[1][2] = axis.y * axis.z * (1 - cosv) - axis.x * sinv;

    ret[2][0] = axis.x * axis.z * (1 - cosv) - axis.y * sinv;
    ret[2][1] = axis.y * axis.z * (1 - cosv) + axis.x * sinv;
    ret[2][2] = axis.z * axis.z + (1 - axis.z * axis.z) * cosv;

    return ret;
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::rotate_x(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(1, 0, 0,
                  0, C, -S,
                  0, S, C);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::rotate_y(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(C, 0,  S,
                  0, 1,  0,
                  -S, 0, C);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::rotate_z(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(C, -S, 0,
                  S, C,  0,
                  0, 0,  1);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::scale(const tvec2<T> &ratio) noexcept
{
    return scale(ratio.x, ratio.y);
}

template<typename T>
typename tmat3_c<T>::self_t tmat3_c<T>::scale(T x, T y) noexcept
{
    return self_t(x, 0, 0,
                  0, y, 0,
                  0, 0, 1);
}

template<typename T>
typename tmat3_c<T>::col_t& tmat3_c<T>::operator[](size_t idx) noexcept
{
    return data[idx];
}

template<typename T>
const typename tmat3_c<T>::col_t& tmat3_c<T>::operator[](size_t idx) const noexcept
{
    return data[idx];
}

template<typename T>
T &tmat3_c<T>::operator()(size_t row, size_t col) noexcept
{
    return data[col][row];
}

template<typename T>
const T &tmat3_c<T>::operator()(size_t row, size_t col) const noexcept
{
    return data[col][row];
}

template<typename T>
const tvec3<T>& tmat3_c<T>::get_col(size_t idx) const noexcept
{
    return data[idx];
}

template<typename T>
tvec3<T> tmat3_c<T>::get_row(size_t idx) const noexcept
{
    return tvec3<T>(data[0][idx], data[1][idx], data[2][idx]);
}

template<typename T>
auto tmat3_c<T>::det() const noexcept
{
    return determinant(data[0][0], data[1][0], data[2][0],
                       data[0][1], data[1][1], data[2][1],
                       data[0][2], data[1][2], data[2][2]);
}

template<typename T>
auto tmat3_c<T>::determinant() const noexcept
{
    return det();
}

AGZM_END
