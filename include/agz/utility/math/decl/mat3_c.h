#pragma once

#include "common.h"
#include "vec3.h"

namespace agz::math {

/**
 * @brief column-major 3x3 matrix
 */
template<typename T>
class tmat3_c
{
    tmat3_c(const tvec3<T> &c0, const tvec3<T> &c1, const tvec3<T> &c2) noexcept;

public:

    using row_t  = tvec3<T>;
    using col_t  = tvec3<T>;
    using data_t = col_t[3];

    using self_t = tmat3_c<T>;
    using elem_t = T;

    data_t data;

    tmat3_c() noexcept;

    explicit tmat3_c(uninitialized_t) noexcept;

    tmat3_c(T m00, T m01, T m02,
            T m10, T m11, T m12,
            T m20, T m21, T m22) noexcept;

    static self_t from_rows(const row_t &r0,
                            const row_t &r1,
                            const row_t &r2) noexcept;

    static self_t from_cols(const col_t &c0,
                            const col_t &c1,
                            const col_t &c2) noexcept;

    static self_t diag(T val) noexcept;

    static self_t all(T val) noexcept;

    static const self_t &zero()     noexcept;
    static const self_t &one()      noexcept;
    static const self_t &identity() noexcept;

    static self_t translate(const tvec2<T> &offset) noexcept;
    static self_t translate(T x, T y)               noexcept;

    static self_t rotate(const tvec3<T> &_axis, T rad) noexcept;

    static self_t rotate_x(T rad) noexcept;
    static self_t rotate_y(T rad) noexcept;
    static self_t rotate_z(T rad) noexcept;
    
    static self_t scale(const tvec2<T> &ratio) noexcept;
    static self_t scale(T x, T y)              noexcept;

    self_t inv_from_adj(const self_t &adj) const noexcept;

          col_t &operator[](size_t idx)       noexcept;
    const col_t &operator[](size_t idx) const noexcept;

          T &operator()(size_t row, size_t col)       noexcept;
    const T &operator()(size_t row, size_t col) const noexcept;

    const tvec3<T> &get_col(size_t idx) const noexcept;
    tvec3<T>        get_row(size_t idx) const noexcept;

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

template<typename T> tmat3_c<T> operator+(
    const tmat3_c<T> &lhs, const tmat3_c<T> &rhs) noexcept;
template<typename T> tmat3_c<T> operator-(
    const tmat3_c<T> &lhs, const tmat3_c<T> &rhs) noexcept;

template<typename T> tmat3_c<T> operator*(
    const tmat3_c<T> &lhs, const tmat3_c<T> &rhs) noexcept;
template<typename T> tvec3<T>   operator*(
    const tmat3_c<T> &lhs, const tvec3<T>   &rhs) noexcept;
template<typename T> tvec3<T>   operator*(
    const tvec3<T>   &lhs, const tmat3_c<T> &rhs) noexcept;

template<typename T> tmat3_c<T> operator*(
    const tmat3_c<T> &lhs, T rhs) noexcept;
template<typename T> tmat3_c<T> operator/(
    const tmat3_c<T> &lhs, T rhs) noexcept;
template<typename T> tmat3_c<T> operator*(
    T lhs, const tmat3_c<T> &rhs) noexcept;

using mat3f_c = tmat3_c<float>;
using mat3d_c = tmat3_c<double>;

} // namespace agz::math
