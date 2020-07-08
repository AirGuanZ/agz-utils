#pragma once

#ifdef AGZ_UTILS_SSE

#include <immintrin.h>

namespace agz::math
{

inline _simd_float3_t::_simd_float3_t() noexcept
    : _simd_float3_t(0)
{
    
}

inline _simd_float3_t::_simd_float3_t(float x, float y, float z) noexcept
    : _simd_float3_t(_mm_set_ps(z, z, y, x))
{
    
}

inline _simd_float3_t::_simd_float3_t(const __m128 &m128) noexcept
    : m128(m128)
{
    
}

inline _simd_float3_t &_simd_float3_t::operator=(const __m128 &m128) noexcept
{
    this->m128 = m128;
    return *this;
}

inline _simd_float3_t::_simd_float3_t(float v) noexcept
    : _simd_float3_t(_mm_set_ps1(v))
{
    
}

inline _simd_float3_t::_simd_float3_t(uninitialized_t) noexcept
{
    
}

inline _simd_float3_t::_simd_float3_t(const _simd_float3_t &other) noexcept
    : m128(other.m128)
{
    
}

inline _simd_float3_t &_simd_float3_t::operator=(const _simd_float3_t &other) noexcept
{
    m128 = other.m128;
    return *this;
}

inline bool _simd_float3_t::is_zero() const noexcept
{
    return *this == self_t(0);
}

inline float _simd_float3_t::length() const noexcept
{
    return std::sqrt(length_square());
}

inline float _simd_float3_t::length_square() const noexcept
{
    return ((*this) * (*this)).sum();
}

inline _simd_float3_t _simd_float3_t::normalize() const noexcept
{
    return *this / this->length();
}

inline _simd_float3_t _simd_float3_t::clamp(float min_v, float max_v) const noexcept
{
    return clamp_high(max_v).clamp_low(min_v);
}

inline _simd_float3_t _simd_float3_t::clamp_high(float max_v) const noexcept
{
    return self_t(_mm_min_ps(m128, _mm_set_ps1(max_v)));
}

inline _simd_float3_t _simd_float3_t::clamp_low(float min_v) const noexcept
{
    return self_t(_mm_max_ps(m128, _mm_set_ps1(min_v)));
}

inline _simd_float3_t _simd_float3_t::saturate() const noexcept
{
    return clamp(0, 1);
}

inline float _simd_float3_t::sum() const noexcept
{
    return x + y + z;
}

inline float _simd_float3_t::product() const noexcept
{
    return x * y * z;
}

inline float _simd_float3_t::max_elem() const noexcept
{
    return (std::max)(x, (std::max)(y, z));
}

inline float _simd_float3_t::min_elem() const noexcept
{
    return (std::min)(x, (std::min)(y, z));
}

inline bool _simd_float3_t::operator!() const noexcept
{
    return is_zero();
}

inline float &_simd_float3_t::operator[](size_t idx) noexcept
{
    return *(&x + idx);
}

inline const float &_simd_float3_t::operator[](size_t idx) const noexcept
{
    return *(&x + idx);
}

inline _simd_float3_t::operator union __m128&() noexcept
{
    return m128;
}

inline _simd_float3_t::operator const union __m128&() const noexcept
{
    return m128;
}

template<int I0, int I1, int I2>
_simd_float3_t _simd_float3_t::swizzle() const noexcept
{
    return _simd_float3_t(_mm_castsi128_ps(
        _mm_shuffle_epi32(_mm_castps_si128(m128), _MM_SHUFFLE(I2, I2, I1, I0))));
}

inline float dot(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xff));
}

inline float cos(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return dot(lhs, rhs) / (lhs.length() * rhs.length());
}

inline _simd_float3_t cross(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _simd_float3_t(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

inline _simd_float3_t operator+(const _simd_float3_t &lhs, float rhs) noexcept
{
    return lhs + _simd_float3_t(rhs);
}

inline _simd_float3_t operator-(const _simd_float3_t &lhs, float rhs) noexcept
{
    return lhs - _simd_float3_t(rhs);
}

inline _simd_float3_t operator*(const _simd_float3_t &lhs, float rhs) noexcept
{
    return lhs * _simd_float3_t(rhs);
}

inline _simd_float3_t operator/(const _simd_float3_t &lhs, float rhs) noexcept
{
    return lhs / _simd_float3_t(rhs);
}

inline _simd_float3_t operator+(float lhs, const _simd_float3_t &rhs) noexcept
{
    return rhs + lhs;
}

inline _simd_float3_t operator*(float lhs, const _simd_float3_t &rhs) noexcept
{
    return rhs * lhs;
}

inline _simd_float3_t operator+(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _simd_float3_t(_mm_add_ps(lhs, rhs));
}

inline _simd_float3_t operator-(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _simd_float3_t(_mm_sub_ps(lhs, rhs));
}

inline _simd_float3_t operator-(const _simd_float3_t &v) noexcept
{
    return _simd_float3_t(0) - v;
}

inline _simd_float3_t operator*(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _simd_float3_t(_mm_mul_ps(lhs, rhs));
}

inline _simd_float3_t operator/(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return _simd_float3_t(_mm_div_ps(lhs, rhs));
}

inline bool operator==(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return (_mm_movemask_ps(_mm_cmpeq_ps(lhs, rhs)) & 15) == 15;
}

inline bool operator!=(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept
{
    return !(lhs == rhs);
}

inline auto distance(const _simd_float3_t &a, const _simd_float3_t &b) noexcept
{
    return (a - b).length();
}

inline auto distance2(const _simd_float3_t &a, const _simd_float3_t &b) noexcept
{
    return (a - b).length_square();
}

inline _simd_float3_t exp(const _simd_float3_t &v) noexcept
{
    return _simd_float3_t(_mm_exp_ps(v));
}

} // namespace agz::math

#endif // #ifdef AGZ_UTILS_SSE
