#pragma once

#include <type_traits>

#ifdef AGZ_UTILS_SSE

#include <emmintrin.h>

#include "../common.h"

namespace agz::math
{

class alignas(16) _simd_float3_t
{
public:

    using self_t = _simd_float3_t;
    using elem_t = float;

    union
    {
        __m128 m128;
        struct { float x, y, z, _private_w; };
        struct { float r, g, b, _private_a; };
    };
    
    _simd_float3_t()                          noexcept;
    _simd_float3_t(float x, float y, float z) noexcept;

    explicit _simd_float3_t(const __m128 &m128) noexcept;

    explicit _simd_float3_t(float v)         noexcept;
    explicit _simd_float3_t(uninitialized_t) noexcept;

    _simd_float3_t(const _simd_float3_t &other) noexcept;
    _simd_float3_t &operator=(const _simd_float3_t &other) noexcept;

    bool is_zero() const noexcept;
    bool is_black() const noexcept;
    
    float length()        const noexcept;
    float length_square() const noexcept;

    _simd_float3_t normalize() const noexcept;

    _simd_float3_t clamp(float min_v, float max_v) const noexcept;
    _simd_float3_t clamp_low (float min_v)         const noexcept;
    _simd_float3_t clamp_high(float max_v)         const noexcept;

    _simd_float3_t saturate() const noexcept;

    float sum()     const noexcept;
    float product() const noexcept;

    float max_elem() const noexcept;
    float min_elem() const noexcept;

    float lum() const noexcept;

    bool is_finite() const noexcept;
    
    bool operator!() const noexcept;

    float       &operator[](size_t idx)       noexcept;
    const float &operator[](size_t idx) const noexcept;

    operator       __m128 &()       noexcept;
    operator const __m128 &() const noexcept;

    template<int I0, int I1, int I2>
    _simd_float3_t swizzle() const noexcept;

    // conv with normal float3

    _simd_float3_t(const vec3f &v) noexcept : _simd_float3_t(v.x, v.y, v.z) { }
    operator vec3f () const noexcept { return { x, y, z }; }

    // conv with color3f

    _simd_float3_t(const color3f &c) noexcept : _simd_float3_t(c.r, c.g, c.b) { }
    operator color3f() const noexcept { return { r, g, b }; }
};

using float3 = _simd_float3_t;

float dot(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
float cos(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

float distance(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
float distance2(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

_simd_float3_t exp(const _simd_float3_t &v) noexcept;
_simd_float3_t sqrt(const _simd_float3_t &v) noexcept;

_simd_float3_t cross(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

_simd_float3_t elem_min(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
_simd_float3_t elem_max(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

_simd_float3_t operator+(const _simd_float3_t &lhs, float rhs) noexcept;
_simd_float3_t operator-(const _simd_float3_t &lhs, float rhs) noexcept;
_simd_float3_t operator*(const _simd_float3_t &lhs, float rhs) noexcept;
_simd_float3_t operator/(const _simd_float3_t &lhs, float rhs) noexcept;

_simd_float3_t operator+(float lhs, const _simd_float3_t &rhs) noexcept;
_simd_float3_t operator*(float lhs, const _simd_float3_t &rhs) noexcept;

_simd_float3_t operator/(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
_simd_float3_t operator*(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
_simd_float3_t operator+(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
_simd_float3_t operator-(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

_simd_float3_t operator-(const _simd_float3_t &v) noexcept;

inline _simd_float3_t &operator+=(_simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept { lhs = lhs + rhs; return lhs; }
inline _simd_float3_t &operator-=(_simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept { lhs = lhs - rhs; return lhs; }
inline _simd_float3_t &operator*=(_simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept { lhs = lhs * rhs; return lhs; }
inline _simd_float3_t &operator/=(_simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept { lhs = lhs / rhs; return lhs; }

inline _simd_float3_t &operator+=(_simd_float3_t &lhs, float rhs) noexcept { lhs = lhs + rhs; return lhs; }
inline _simd_float3_t &operator-=(_simd_float3_t &lhs, float rhs) noexcept { lhs = lhs - rhs; return lhs; }
inline _simd_float3_t &operator*=(_simd_float3_t &lhs, float rhs) noexcept { lhs = lhs * rhs; return lhs; }
inline _simd_float3_t &operator/=(_simd_float3_t &lhs, float rhs) noexcept { lhs = lhs / rhs; return lhs; }

bool operator==(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;
bool operator!=(const _simd_float3_t &lhs, const _simd_float3_t &rhs) noexcept;

} // namespace agz::math

namespace std
{
    template<>
    struct hash<agz::math::_simd_float3_t>
    {
        size_t operator()(const agz::math::_simd_float3_t &vec) const noexcept
        {
            return agz::misc::hash(vec.x, vec.y, vec.z);
        }
    };

} // namespace std

#endif // #ifdef AGZ_UTILS_SSE
