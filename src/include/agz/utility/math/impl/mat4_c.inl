#pragma once

namespace agz::math {

template<typename T>
tmat4_c<T>::tmat4_c(const tvec4<T> &c0, const tvec4<T> &c1, const tvec4<T> &c2, const tvec4<T> &c3) noexcept
    : data{ c0, c1, c2, c3 }
{

}

template<typename T>
tmat4_c<T>::tmat4_c() noexcept
    : tmat4_c(identity())
{
    
}

template <typename T>
tmat4_c<T>::tmat4_c(uninitialized_t) noexcept
    : data{ col_t(UNINIT), col_t(UNINIT), col_t(UNINIT), col_t(UNINIT) }
{
    
}

template<typename T>
tmat4_c<T>::tmat4_c(T m00, T m01, T m02, T m03,
                    T m10, T m11, T m12, T m13,
                    T m20, T m21, T m22, T m23,
                    T m30, T m31, T m32, T m33) noexcept
    : data{ { m00, m10, m20, m30 },
            { m01, m11, m21, m31 },
            { m02, m12, m22, m32 },
            { m03, m13, m23, m33 } }
{
    
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::from_rows(const row_t &r0,
                                                  const row_t &r1,
                                                  const row_t &r2,
                                                  const row_t &r3) noexcept
{
    return self_t(r0.x, r0.y, r0.z, r0.w,
                  r1.x, r1.y, r1.z, r1.w,
                  r2.x, r2.y, r2.z, r2.w,
                  r3.x, r3.y, r3.z, r3.w);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::from_cols(const col_t &c0,
                                                  const col_t &c1,
                                                  const col_t &c2,
                                                  const col_t &c3) noexcept
{
    return self_t(c0, c1, c2, c3);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::diag(T val) noexcept
{
    return self_t(val, 0,   0,   0,
                  0,   val, 0,   0,
                  0,   0,   val, 0,
                  0,   0,   0,   val);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::all(T val) noexcept
{
    return self_t(val, val, val, val,
                  val, val, val, val,
                  val, val, val, val,
                  val, val, val, val);
}

template<typename T>
const typename tmat4_c<T>::self_t& tmat4_c<T>::zero() noexcept
{
    static const self_t ret = self_t::all(0);
    return ret;
}

template<typename T>
const typename tmat4_c<T>::self_t& tmat4_c<T>::one() noexcept
{
    static const self_t ret = self_t::all(1);
    return ret;
}

template<typename T>
const typename tmat4_c<T>::self_t& tmat4_c<T>::identity() noexcept
{
    static const self_t ret = self_t::diag(1);
    return ret;
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::translate(const tvec3<T> &offset) noexcept
{
    return translate(offset.x, offset.y, offset.z);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::translate(T x, T y, T z) noexcept
{
    return self_t(1, 0, 0, x,
                  0, 1, 0, y,
                  0, 0, 1, z,
                  0, 0, 0, 1);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::rotate(const tvec3<T> &_axis, T rad) noexcept
{
    const auto axis = _axis.normalize();
    const T sinv = std::sin(rad), cosv = std::cos(rad);

    self_t ret(UNINIT);

    ret[0][0] = axis.x * axis.x + (1 - axis.x * axis.x) * cosv;
    ret[0][1] = axis.x * axis.y * (1 - cosv) - axis.z * sinv;
    ret[0][2] = axis.x * axis.z * (1 - cosv) + axis.y * sinv;
    ret[0][3] = 0;

    ret[1][0] = axis.x * axis.y * (1 - cosv) + axis.z * sinv;
    ret[1][1] = axis.y * axis.y + (1 - axis.y * axis.y) * cosv;
    ret[1][2] = axis.y * axis.z * (1 - cosv) - axis.x * sinv;
    ret[1][3] = 0;

    ret[2][0] = axis.x * axis.z * (1 - cosv) - axis.y * sinv;
    ret[2][1] = axis.y * axis.z * (1 - cosv) + axis.x * sinv;
    ret[2][2] = axis.z * axis.z + (1 - axis.z * axis.z) * cosv;
    ret[2][3] = 0;

    ret[3][0] = 0;
    ret[3][1] = 0;
    ret[3][2] = 0;
    ret[3][3] = 1;

    return ret;
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::rotate_x(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(1, 0, 0,  0,
                  0, C, -S, 0,
                  0, S, C,  0,
                  0, 0, 0,  1);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::rotate_y(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(C,  0, S, 0,
                  0,  1, 0, 0,
                  -S, 0, C, 0,
                  0,  0, 0, 1);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::rotate_z(T rad) noexcept
{
    const auto S = std::sin(rad), C = std::cos(rad);
    return self_t(C, -S, 0, 0,
                  S, C,  0, 0,
                  0, 0,  1, 0,
                  0, 0,  0, 1);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::scale(const tvec3<T> &ratio) noexcept
{
    return scale(ratio.x, ratio.y, ratio.z);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::scale(T x, T y, T z) noexcept
{
    return self_t(x, 0, 0, 0,
                  0, y, 0, 0,
                  0, 0, z, 0,
                  0, 0, 0, 1);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::perspective(T fov_y_rad, T w_over_h, T near_plane, T far_plane) noexcept
{
    T inv_dis = T(1) / (far_plane - near_plane);
    auto y_rad = T(0.5) * fov_y_rad;
    auto cot = std::cos(y_rad) / std::sin(y_rad);

    return self_t(cot / w_over_h, 0,   0,                   0,
                  0,              cot, 0,                   0,
                  0,              0,   far_plane * inv_dis, -far_plane * near_plane * inv_dis,
                  0,              0,   1,                   0);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::look_at(const tvec3<T> &eye, const tvec3<T> &dst, const tvec3<T> &up) noexcept
{
    auto D = (dst - eye).normalize();
    auto R = cross(up, D).normalize();
    auto U = cross(D, R);
    return self_t(R.x, U.x, D.x, eye.x,
                  R.y, U.y, D.y, eye.y,
                  R.z, U.z, D.z, eye.z,
                  0,   0,   0,   1).inverse();
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::inv_from_adj(const self_t &adj) const noexcept
{
    return adj / dot(data[0], adj.get_row(0));
}

template<typename T>
typename tmat4_c<T>::col_t& tmat4_c<T>::operator[](size_t idx) noexcept
{
    return data[idx];
}

template<typename T>
const typename tmat4_c<T>::col_t& tmat4_c<T>::operator[](size_t idx) const noexcept
{
    return data[idx];
}

template<typename T>
T &tmat4_c<T>::operator()(size_t row, size_t col) noexcept
{
    return data[col][row];
}

template<typename T>
const T &tmat4_c<T>::operator()(size_t row, size_t col) const noexcept
{
    return data[col][row];
}

template<typename T>
const tvec4<T> &tmat4_c<T>::get_col(size_t idx) const noexcept
{
    return data[idx];
}

template<typename T>
tvec4<T> tmat4_c<T>::get_row(size_t idx) const noexcept
{
    return tvec4<T>(data[0][idx], data[1][idx], data[2][idx], data[3][idx]);
}

template<typename T>
auto tmat4_c<T>::det() const noexcept
{
    return determinant(data[0][0], data[1][0], data[2][0], data[3][0],
                       data[0][1], data[1][1], data[2][1], data[3][1],
                       data[0][2], data[1][2], data[2][2], data[3][2],
                       data[0][3], data[1][3], data[2][3], data[3][3]);
}

template<typename T>
auto tmat4_c<T>::determinant() const noexcept
{
    return det();
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::inv() const noexcept
{
    return inv_from_adj(adj());
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::inverse() const noexcept
{
    return inv();
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::t() const noexcept
{
    return self_t::from_rows(data[0], data[1], data[2], data[3]);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::transpose() const noexcept
{
    return t();
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::adj() const noexcept
{
    T blk00 = data[2][2] * data[3][3] - data[3][2] * data[2][3];
    T blk02 = data[1][2] * data[3][3] - data[3][2] * data[1][3];
    T blk03 = data[1][2] * data[2][3] - data[2][2] * data[1][3];

    T blk04 = data[2][1] * data[3][3] - data[3][1] * data[2][3];
    T blk06 = data[1][1] * data[3][3] - data[3][1] * data[1][3];
    T blk07 = data[1][1] * data[2][3] - data[2][1] * data[1][3];

    T blk08 = data[2][1] * data[3][2] - data[3][1] * data[2][2];
    T blk10 = data[1][1] * data[3][2] - data[3][1] * data[1][2];
    T blk11 = data[1][1] * data[2][2] - data[2][1] * data[1][2];

    T blk12 = data[2][0] * data[3][3] - data[3][0] * data[2][3];
    T blk14 = data[1][0] * data[3][3] - data[3][0] * data[1][3];
    T blk15 = data[1][0] * data[2][3] - data[2][0] * data[1][3];

    T blk16 = data[2][0] * data[3][2] - data[3][0] * data[2][2];
    T blk18 = data[1][0] * data[3][2] - data[3][0] * data[1][2];
    T blk19 = data[1][0] * data[2][2] - data[2][0] * data[1][2];

    T blk20 = data[2][0] * data[3][1] - data[3][0] * data[2][1];
    T blk22 = data[1][0] * data[3][1] - data[3][0] * data[1][1];
    T blk23 = data[1][0] * data[2][1] - data[2][0] * data[1][1];

    tvec4<T> fac0(blk00, blk00, blk02, blk03);
    tvec4<T> fac1(blk04, blk04, blk06, blk07);
    tvec4<T> fac2(blk08, blk08, blk10, blk11);
    tvec4<T> fac3(blk12, blk12, blk14, blk15);
    tvec4<T> fac4(blk16, blk16, blk18, blk19);
    tvec4<T> fac5(blk20, blk20, blk22, blk23);

    tvec4<T> vec0(data[1][0], data[0][0], data[0][0], data[0][0]);
    tvec4<T> vec1(data[1][1], data[0][1], data[0][1], data[0][1]);
    tvec4<T> vec2(data[1][2], data[0][2], data[0][2], data[0][2]);
    tvec4<T> vec3(data[1][3], data[0][3], data[0][3], data[0][3]);

    tvec4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
    tvec4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
    tvec4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
    tvec4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

    tvec4<T> sign_a(+1, -1, +1, -1);
    tvec4<T> sign_b(-1, +1, -1, +1);

    return from_cols(inv0 * sign_a, inv1 * sign_b, inv2 * sign_a, inv3 * sign_b);
}

template<typename T>
typename tmat4_c<T>::self_t tmat4_c<T>::adjoint() const noexcept
{
    return adj();
}

template<typename T>
typename tmat4_c<T>::self_t &tmat4_c<T>::operator+=(const self_t &rhs) noexcept
{
    return *this = *this + rhs;
}

template<typename T>
typename tmat4_c<T>::self_t &tmat4_c<T>::operator-=(const self_t &rhs) noexcept
{
    return *this = *this - rhs;
}

template<typename T>
typename tmat4_c<T>::self_t &tmat4_c<T>::operator*=(const self_t &rhs) noexcept
{
    return *this = *this * rhs;
}

template<typename T>
typename tmat4_c<T>::self_t &tmat4_c<T>::operator*=(T rhs) noexcept
{
    return *this = *this * rhs;
}

template<typename T>
typename tmat4_c<T>::self_t &tmat4_c<T>::operator/=(T rhs) noexcept
{
    return *this = *this / rhs;
}

template<typename T>
tmat4_c<T> operator+(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept
{
    return tmat4_c<T>::from_cols(lhs[0] + rhs[0],
                                 lhs[1] + rhs[1],
                                 lhs[2] + rhs[2],
                                 lhs[3] + rhs[3]);
}

template<typename T>
tmat4_c<T> operator-(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept
{
    return tmat4_c<T>::from_cols(lhs[0] - rhs[0],
                                 lhs[1] - rhs[1],
                                 lhs[2] - rhs[2],
                                 lhs[3] - rhs[3]);
}

template<typename T>
tmat4_c<T> operator*(const tmat4_c<T> &lhs, const tmat4_c<T> &rhs) noexcept
{
    tmat4_c<T> ret(UNINIT);
    for(int c = 0; c != 4; ++c)
    {
        for(int r = 0; r != 4; ++r)
            ret(r, c) = dot(lhs.get_row(r), rhs.get_col(c));
    }
    return ret;
}

template<typename T>
tvec4<T> operator*(const tmat4_c<T> &lhs, const tvec4<T> &rhs) noexcept
{
    /*return tvec4<T>(dot(lhs.get_row(0), rhs),
                    dot(lhs.get_row(1), rhs),
                    dot(lhs.get_row(2), rhs),
                    dot(lhs.get_row(3), rhs));*/
    tvec4<T> col0_rhs = rhs[0] * lhs.data[0];
    tvec4<T> col1_rhs = rhs[1] * lhs.data[1];
    tvec4<T> col2_rhs = rhs[2] * lhs.data[2];
    tvec4<T> col3_rhs = rhs[3] * lhs.data[3];
    return col0_rhs + col1_rhs + col2_rhs + col3_rhs;
    /*return tvec4<T>(col0_rhs[0] + col1_rhs[0] + col2_rhs[0] + col3_rhs[0],
                    col0_rhs[1] + col1_rhs[1] + col2_rhs[1] + col3_rhs[1],
                    col0_rhs[2] + col1_rhs[2] + col2_rhs[2] + col3_rhs[2],
                    col0_rhs[3] + col1_rhs[3] + col2_rhs[3] + col3_rhs[3]);*/
}

template<typename T>
tvec4<T> operator*(const tvec4<T> &lhs, const tmat4_c<T> &rhs) noexcept
{
    return tvec4<T>(dot(lhs, rhs.get_col(0)),
                    dot(lhs, rhs.get_col(1)),
                    dot(lhs, rhs.get_col(2)),
                    dot(lhs, rhs.get_col(3)));
}

template<typename T> tmat4_c<T> operator*(const tmat4_c<T> &lhs, T rhs) noexcept
{
    return tmat4_c<T>::from_cols(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
}

template<typename T> tmat4_c<T> operator/(const tmat4_c<T> &lhs, T rhs) noexcept
{
    return tmat4_c<T>::from_cols(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
}

template<typename T> tmat4_c<T> operator*(T lhs, const tmat4_c<T> &rhs) noexcept
{
    return rhs * lhs;
}

} // namespace agz::math
