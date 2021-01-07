#pragma once

#ifdef AGZ_UTILS_SSE

#include "./float3.h"
#include "./float4.h"
#include "./float4x4.h"
#include "./coord_float3.h"

namespace agz::math
{

class _simd_transform_float3_t
{
    float4x4 mat_;
    float4x4 inv_;

public:

    using self_t = _simd_transform_float3_t;

    _simd_transform_float3_t() noexcept;
    _simd_transform_float3_t(const float4x4 &m, const float4x4 &inv) noexcept;

    explicit _simd_transform_float3_t(const float4x4 &m) noexcept;
    explicit _simd_transform_float3_t(uninitialized_t)   noexcept;

    static self_t translate(const float3 &offset)      noexcept;
    static self_t translate(float x, float y, float z) noexcept;

    static self_t rotate(const float3 &axis, float rad) noexcept;
    static self_t rotate_x(float rad)                   noexcept;
    static self_t rotate_y(float rad)                   noexcept;
    static self_t rotate_z(float rad)                   noexcept;

    static self_t scale(const float3 &scale)       noexcept;
    static self_t scale(float x, float y, float z) noexcept;

    self_t &operator*=(const self_t &rhs) noexcept;

    float3 apply_to_point (const float3 &point)  const noexcept;
    float3 apply_to_vector(const float3 &vector) const noexcept;
    float3 apply_to_normal(const float3 &normal) const noexcept;
    float3_coord apply_to_coord(const float3_coord &coord) const noexcept;

    float3 apply_inverse_to_point (const float3 &point)  const noexcept;
    float3 apply_inverse_to_vector(const float3 &vector) const noexcept;
    float3 apply_inverse_to_normal(const float3 &normal) const noexcept;
    float3_coord apply_inverse_to_coord(const float3_coord &coord) const noexcept;

    self_t inv()     const noexcept;
    self_t inverse() const noexcept;

    const float4x4 &get_mat()     const noexcept;
    const float4x4 &get_inv_mat() const noexcept;

    bool operator==(const self_t &rhs) const noexcept;
    bool operator!=(const self_t &rhs) const noexcept;

    // conv with ttransform3<float3>

    _simd_transform_float3_t(const ttransform3<float> &ori) noexcept
        : _simd_transform_float3_t(ori.get_mat(), ori.get_inv_mat())
    {

    }

    operator ttransform3<float>() const noexcept
    {
        return ttransform3<float>(mat_, inv_);
    }
};

_simd_transform_float3_t operator*(
    const _simd_transform_float3_t &lhs,
    const _simd_transform_float3_t &rhs) noexcept;

using float3_transform = _simd_transform_float3_t;

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
