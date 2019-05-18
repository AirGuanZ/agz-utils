#pragma once

#include "common.h"
#include "vec4.h"

AGZM_BEGIN

/**
 * @brief column-major 4x4 matrix
 */
template<typename T>
class tmat4_c
{
    tmat4_c(const tvec4<T> &c0, const tvec4<T> &c1, const tvec4<T> &c2, const tvec4<T> &c3) noexcept;

public:

    using row_t  = tvec4<T>;
    using col_t  = tvec4<T>;
    using data_t = col_t[4];

    using self_t = tmat4_c<T>;
    using elem_t = T;

    data_t data;

    tmat4_c() noexcept;

    explicit tmat4_c(uninitialized_t) noexcept;

    tmat4_c(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33) noexcept;

    static self_t from_rows(const row_t &r0,
                            const row_t &r1,
                            const row_t &r2,
                            const row_t &r3) noexcept;

    static self_t from_cols(const col_t &c0,
                            const col_t &c1,
                            const col_t &c2,
                            const col_t &c3) noexcept;

    static self_t diag(T val) noexcept;

    static self_t all(T val) noexcept;

    static const self_t &zero()     noexcept;
    static const self_t &one()      noexcept;
    static const self_t &identity() noexcept;

    static self_t translate(const tvec3<T> &offset) noexcept;
    static self_t translate(T x, T y, T z) noexcept;

    static self_t rotate(const tvec3<T> &_axis, T rad) noexcept;
    static self_t rotate_x(T rad) noexcept;
    static self_t rotate_y(T rad) noexcept;
    static self_t rotate_z(T rad) noexcept;

    static self_t scale(const tvec3<T> &ratio) noexcept;
    static self_t scale(T x, T y, T z)         noexcept;

    static self_t perspective(T fov_y_rad, T w_over_h, T near_plane, T far_plane)       noexcept;
    static self_t look_at(const tvec3<T> &eye, const tvec3<T> &dst, const tvec3<T> &up) noexcept;

    self_t inv_from_adj(const self_t &adj) const noexcept;

          col_t &operator[](size_t idx)       noexcept;
    const col_t &operator[](size_t idx) const noexcept;

          T &operator()(size_t row, size_t col)       noexcept;
    const T &operator()(size_t row, size_t col) const noexcept;

    const tvec4<T> &get_col(size_t idx) const noexcept;
    tvec4<T>        get_row(size_t idx) const noexcept;

    auto det()         const noexcept;
    auto determinant() const noexcept;

    self_t inv()     const noexcept;
    self_t inverse() const noexcept;

    self_t t()         const noexcept;
    self_t transpose() const noexcept;

    self_t adj()     const noexcept;
    self_t adjoint() const noexcept;

    self_t &operator+=(const self_t &rhs) noexcept;
    self_t &operator-=(const self_t &rhs) noexcept;
    self_t &operator*=(const self_t &rhs) noexcept;

    self_t &operator*=(T rhs) noexcept;
    self_t &operator/=(T rhs) noexcept;
};

template<typename T> tmat4_c<T> operator+(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept;
template<typename T> tmat4_c<T> operator-(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept;

template<typename T> tmat4_c<T> operator*(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept;
template<typename T> tvec4<T>   operator*(const tmat4_c<T> &lhs, const tvec4<T>   &rhs) noexcept;
template<typename T> tvec4<T>   operator*(const tvec4<T>   &lhs, const tmat4_c<T> &rhs) noexcept;

template<typename T> tmat4_c<T> operator*(const tmat4_c<T> &lhs, T rhs) noexcept;
template<typename T> tmat4_c<T> operator/(const tmat4_c<T> &lhs, T rhs) noexcept;
template<typename T> tmat4_c<T> operator*(T lhs, const tmat4_c<T> &rhs) noexcept;

using mat4f_c = tmat4_c<float>;
using mat4d_c = tmat4_c<double>;

AGZM_END
