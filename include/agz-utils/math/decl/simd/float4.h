#pragma once

#ifdef AGZ_UTILS_SSE

#include <emmintrin.h>

#include "../vec2.h"
#include "../vec3.h"
#include "./float3.h"

namespace agz::math
{

class alignas(16) _simd_float4_t
{
public:

    using self_t = _simd_float4_t;
    using elem_t = float;

    union
    {
        __m128 m128;
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    _simd_float4_t()                                   noexcept;
    _simd_float4_t(float x, float y, float z, float w) noexcept;
    _simd_float4_t(const _simd_float3_t &xyz, float w) noexcept;

    explicit _simd_float4_t(const __m128 &m128) noexcept;
    _simd_float4_t &operator=(const __m128 &m128) noexcept;

    explicit _simd_float4_t(float v)         noexcept;
    explicit _simd_float4_t(uninitialized_t) noexcept;

    _simd_float4_t(const _simd_float4_t &other) noexcept;
    _simd_float4_t &operator=(const _simd_float4_t &other) noexcept;

    bool is_zero() const noexcept;
    
    float length()        const noexcept;
    float length_square() const noexcept;

    _simd_float4_t normalize() const noexcept;

    _simd_float4_t clamp(float min_v, float max_v) const noexcept;
    _simd_float4_t clamp_low (float min_v)         const noexcept;
    _simd_float4_t clamp_high(float max_v)         const noexcept;

    _simd_float4_t saturate() const noexcept;

    float sum()     const noexcept;
    float product() const noexcept;

    float max_elem() const noexcept;
    float min_elem() const noexcept;
    
    bool operator!() const noexcept;

    float       &operator[](size_t idx)       noexcept;
    const float &operator[](size_t idx) const noexcept;

    operator       __m128 &()       noexcept;
    operator const __m128 &() const noexcept;

    template<int I0, int I1, int I2, int I3>
    _simd_float4_t swizzle() const noexcept;

    // conv with normal float4

    _simd_float4_t(const vec4f &v) noexcept : _simd_float4_t(v.x, v.y, v.z, v.w) { }
    operator vec4f () const noexcept { return { x, y, z, w }; }
};

using float4 = _simd_float4_t;

float dot(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;
float cos(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;

_simd_float4_t operator+(const _simd_float4_t &lhs, float rhs) noexcept;
_simd_float4_t operator-(const _simd_float4_t &lhs, float rhs) noexcept;
_simd_float4_t operator*(const _simd_float4_t &lhs, float rhs) noexcept;
_simd_float4_t operator/(const _simd_float4_t &lhs, float rhs) noexcept;

_simd_float4_t operator+(float lhs, const _simd_float4_t &rhs) noexcept;
_simd_float4_t operator*(float lhs, const _simd_float4_t &rhs) noexcept;

_simd_float4_t operator/(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;
_simd_float4_t operator*(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;
_simd_float4_t operator+(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;
_simd_float4_t operator-(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;

bool operator==(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;
bool operator!=(const _simd_float4_t &lhs, const _simd_float4_t &rhs) noexcept;

} // agz::math

#endif // #ifdef AGZ_UTILS_SSE
