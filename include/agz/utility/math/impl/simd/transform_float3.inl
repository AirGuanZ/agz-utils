#pragma once

#ifdef AGZ_UTILS_SSE

namespace agz::math
{

inline _simd_transform_float3_t::_simd_transform_float3_t() noexcept
{
    
}

inline _simd_transform_float3_t::_simd_transform_float3_t(
    const float4x4 &m, const float4x4 &inv) noexcept
    : mat_(m), inv_(inv)
{
    
}

inline _simd_transform_float3_t::_simd_transform_float3_t(
    const float4x4 &m) noexcept
    : _simd_transform_float3_t(m, m.inv())
{
    
}

inline _simd_transform_float3_t::_simd_transform_float3_t(
    uninitialized_t) noexcept
    : mat_(UNINIT), inv_(UNINIT)
{
    
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::translate(
    const float3 &offset) noexcept
{
    return self_t(
        float4x4::left_transform::translate(offset),
        float4x4::left_transform::translate(-offset));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::translate(
    float x, float y, float z) noexcept
{
    return translate({ x, y, z });
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::rotate(
    const float3 &axis, float rad) noexcept
{
    return self_t(
        float4x4::left_transform::rotate(axis, rad),
        float4x4::left_transform::rotate(axis, -rad));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::rotate_x(
    float rad) noexcept
{
    return self_t(
        float4x4::left_transform::rotate_x(rad),
        float4x4::left_transform::rotate_x(-rad));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::rotate_y(
    float rad) noexcept
{
    return self_t(
        float4x4::left_transform::rotate_y(rad),
        float4x4::left_transform::rotate_y(-rad));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::rotate_z(
    float rad) noexcept
{
    return self_t(
        float4x4::left_transform::rotate_z(rad),
        float4x4::left_transform::rotate_z(-rad));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::scale(
    const float3 &scale) noexcept
{
    const float3 inv_scale = float3(1) / scale;
    return self_t(
        float4x4::left_transform::scale(scale),
        float4x4::left_transform::scale(inv_scale));
}

inline _simd_transform_float3_t::self_t _simd_transform_float3_t::scale(
    float x, float y, float z) noexcept
{
    return scale({ x, y, z });
}

inline _simd_transform_float3_t::self_t &_simd_transform_float3_t::operator*=(
    const self_t &rhs) noexcept
{
    mat_ *= rhs.mat_;
    inv_ = rhs.inv_ * inv_;
    return *this;
}

inline float3 _simd_transform_float3_t::apply_to_point(
    const float3 &point) const noexcept
{
    const auto p4 = mat_ * float4(point.x, point.y, point.z, 1);
    return float3(p4.x, p4.y, p4.z) / p4.w;
}

inline float3 _simd_transform_float3_t::apply_to_vector(
    const float3 &vector) const noexcept
{
    const auto v4 = mat_ * float4(vector.x, vector.y, vector.z, 0);
    return float3(v4.x, v4.y, v4.z);
}

inline float3 _simd_transform_float3_t::apply_to_normal(
    const float3 &normal) const noexcept
{
    const auto v4 = inv_.t() * float4(normal.x, normal.y, normal.z, 0);
    return float3(v4.x, v4.y, v4.z).normalize();
}

inline float3_coord _simd_transform_float3_t::apply_to_coord(
    const float3_coord &coord) const noexcept
{
    return float3_coord(
        apply_to_vector(coord.x),
        apply_to_vector(coord.y),
        apply_to_vector(coord.z));
}

inline float3_coord _simd_transform_float3_t::apply_inverse_to_coord(
    const float3_coord &coord) const noexcept
{
    return float3_coord(
        apply_inverse_to_vector(coord.x),
        apply_inverse_to_vector(coord.y),
        apply_inverse_to_vector(coord.z));
}

inline float3 _simd_transform_float3_t::apply_inverse_to_point(
    const float3 &point) const noexcept
{
    const auto p4 = inv_ * float4(point.x, point.y, point.z, 1);
    return float3(p4.x, p4.y, p4.z) / p4.w;
}

inline float3 _simd_transform_float3_t::apply_inverse_to_vector(
    const float3 &vector) const noexcept
{
    const auto v4 = inv_ * float4(vector.x, vector.y, vector.z, 0);
    return float3(v4.x, v4.y, v4.z);
}

inline float3 _simd_transform_float3_t::apply_inverse_to_normal(
    const float3 &normal) const noexcept
{
    const auto v4 = mat_.t() * float4(normal.x, normal.y, normal.z, 0);
    return float3(v4.x, v4.y, v4.z).normalize();
}

inline _simd_transform_float3_t::self_t
    _simd_transform_float3_t::inv() const noexcept
{
    return self_t(inv_, mat_);
}

inline _simd_transform_float3_t::self_t
    _simd_transform_float3_t::inverse() const noexcept
{
    return inv();
}

inline const float4x4 &_simd_transform_float3_t::get_mat() const noexcept
{
    return mat_;
}

inline const float4x4 &_simd_transform_float3_t::get_inv_mat() const noexcept
{
    return inv_;
}

inline _simd_transform_float3_t operator*(
    const _simd_transform_float3_t &lhs,
    const _simd_transform_float3_t &rhs) noexcept
{
    return _simd_transform_float3_t(
        lhs.get_mat() * rhs.get_mat(),
        rhs.get_inv_mat() * lhs.get_inv_mat());
}

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
