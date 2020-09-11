#pragma once

#ifdef AGZ_UTILS_SSE

#include "./float3.h"
#include "./float4.h"

namespace agz::math
{

class _simd_float4x4_c_t
{
    _simd_float4x4_c_t(
        const _simd_float4_t &c0,
        const _simd_float4_t &c1,
        const _simd_float4_t &c2,
        const _simd_float4_t &c3) noexcept;

public:

    using row_t  = _simd_float4_t;
    using col_t  = _simd_float4_t;
    using data_t = col_t[4];

    using self_t = _simd_float4x4_c_t;
    using elem_t = float;

    data_t data;

    _simd_float4x4_c_t() noexcept;

    _simd_float4x4_c_t(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept;

	explicit _simd_float4x4_c_t(uninitialized_t) noexcept;

    static _simd_float4x4_c_t from_rows(
        const row_t &r0,
        const row_t &r1,
        const row_t &r2,
        const row_t &r3) noexcept;

    static _simd_float4x4_c_t from_cols(
        const col_t &c0,
        const col_t &c1,
        const col_t &c2,
        const col_t &c3) noexcept;

    static _simd_float4x4_c_t diag(float val) noexcept;

    static _simd_float4x4_c_t all(float val) noexcept;

    static const self_t &zero()     noexcept;
    static const self_t &one()      noexcept;
    static const self_t &identity() noexcept;

          col_t &operator[](size_t idx)       noexcept;
    const col_t &operator[](size_t idx) const noexcept;

    float &operator()(size_t row, size_t col)       noexcept;
    float  operator()(size_t row, size_t col) const noexcept;

    const row_t get_row(size_t idx) const noexcept;

    float det()         const noexcept;
    float determinant() const noexcept;

    _simd_float4x4_c_t inv()     const noexcept;
	_simd_float4x4_c_t inverse() const noexcept;

	_simd_float4x4_c_t t()         const noexcept;
	_simd_float4x4_c_t transpose() const noexcept;

    // conv with mat4

    _simd_float4x4_c_t(const tmat4_c<float> &m) noexcept
        : _simd_float4x4_c_t(UNINIT)
    {
        data[0] = m[0];
        data[1] = m[1];
        data[2] = m[2];
        data[3] = m[3];
    }

    operator tmat4_c<float>() const noexcept
    {
        return tmat4_c<float>::from_cols(
            data[0], data[1], data[2], data[3]);
    }
    
    struct left_transform
    {
        static self_t translate(const float3 &offset)      noexcept;
        static self_t translate(float x, float y, float z) noexcept;

        static self_t rotate(const float3 &_axis, float rad) noexcept;
        static self_t rotate_x(float rad)                    noexcept;
        static self_t rotate_y(float rad)                    noexcept;
        static self_t rotate_z(float rad)                    noexcept;

        static self_t scale(const float3 &ratio)       noexcept;
        static self_t scale(float x, float y, float z) noexcept;

        static self_t perspective(
            float fov_y_rad, float w_over_h,
            float near_plane, float far_plane) noexcept;

        static self_t orthographics(
            float width, float height,
            float near_plane, float far_plane) noexcept;

        static self_t look_at(
            const float3 &eye, const float3 &dst,
            const float3 &up) noexcept;
    };
};

using float4x4 = _simd_float4x4_c_t;

_simd_float4x4_c_t operator+(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;
_simd_float4x4_c_t operator-(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t operator*(const _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t operator*(const _simd_float4x4_c_t &lhs, float rhs) noexcept;
_simd_float4x4_c_t operator*(float lhs, const _simd_float4x4_c_t &rhs) noexcept;
_simd_float4x4_c_t operator/(const _simd_float4x4_c_t &lhs, float rhs) noexcept;

_simd_float4_t operator*(const _simd_float4x4_c_t &lhs, const _simd_float4_t &rhs) noexcept;
_simd_float4_t operator*(const _simd_float4_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t &operator+=(
    _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t &operator-=(
    _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t &operator*=(
    _simd_float4x4_c_t &lhs, const _simd_float4x4_c_t &rhs) noexcept;

_simd_float4x4_c_t &operator*=(
    _simd_float4x4_c_t &lhs, float rhs) noexcept;

_simd_float4x4_c_t &operator/=(
    _simd_float4x4_c_t &lhs, float rhs) noexcept;

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
