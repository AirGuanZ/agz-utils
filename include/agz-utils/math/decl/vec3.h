#pragma once

#include <functional>
#include <ostream>

#include "../../misc/hash.h"
#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tvec3
{
public:

    using self_t = tvec3<T>;
    using elem_t = T;

    T x, y, z;

    AGZ_MATH_API constexpr tvec3()                             noexcept;
    AGZ_MATH_API constexpr tvec3(T x, T y, T z)                noexcept;
    AGZ_MATH_API explicit constexpr tvec3(T val)               noexcept;
    AGZ_MATH_API explicit tvec3(uninitialized_t)     noexcept;
    AGZ_MATH_API explicit tvec3(const tvec<T, 3> &v) noexcept;
    AGZ_MATH_API explicit tvec3(const tcolor3<T> &c) noexcept;

    AGZ_MATH_API bool is_zero() const noexcept;

    AGZ_MATH_API auto length()        const noexcept;
    AGZ_MATH_API auto length_square() const noexcept;

    AGZ_MATH_API self_t normalize() const noexcept;

    AGZ_MATH_API self_t clamp(T min_v, T max_v) const noexcept;
    AGZ_MATH_API self_t clamp_low (T min_v)     const noexcept;
    AGZ_MATH_API self_t clamp_high(T max_v)     const noexcept;

    AGZ_MATH_API self_t saturate() const noexcept;

    AGZ_MATH_API auto sum()     const noexcept;
    AGZ_MATH_API auto product() const noexcept;

    AGZ_MATH_API T max_elem() const noexcept;
    AGZ_MATH_API T min_elem() const noexcept;

    template<typename F>
    AGZ_MATH_API auto map(F &&f) const noexcept;

    AGZ_MATH_API bool operator!() const noexcept;
    AGZ_MATH_API T       &operator[](size_t idx)       noexcept;
    AGZ_MATH_API const T &operator[](size_t idx) const noexcept;

    AGZ_MATH_API self_t &operator+=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator-=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator*=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator/=(const self_t &rhs) noexcept;

    AGZ_MATH_API self_t &operator+=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator-=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator*=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator/=(T rhs) noexcept;

#include "../impl/swizzle_vec3.inl"
};

template<typename T>
AGZ_MATH_API tvec3<T> operator-(const tvec3<T> &vec) noexcept;

template<typename T>
AGZ_MATH_API tvec3<T> operator+(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec3<T> operator-(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec3<T> operator*(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec3<T> operator/(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T> AGZ_MATH_API tvec3<T> operator+(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec3<T> operator-(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec3<T> operator*(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec3<T> operator/(const tvec3<T> &lhs, T rhs) noexcept;

template<typename T> AGZ_MATH_API tvec3<T> operator+(T lhs, const tvec3<T> &rhs) noexcept;
template<typename T> AGZ_MATH_API tvec3<T> operator*(T lhs, const tvec3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator==(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API bool operator!=(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator<(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API auto dot(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API auto cross(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API auto cos(
    const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T> std::ostream &operator<<(
    std::ostream &out, const tvec3<T> &vec);

using vec3f = tvec3<float>;
using vec3d = tvec3<double>;
using vec3i = tvec3<int>;
using vec3b = tvec3<uint8_t>;

AGZ_MATH_END

#ifndef __CUDACC__

namespace std
{
    template<typename T>
    struct hash<agz::math::tvec3<T>>
    {
        size_t operator()(const agz::math::tvec3<T> &vec) const noexcept
        {
            return agz::misc::hash(vec.x, vec.y, vec.z);
        }
    };
}

#endif
