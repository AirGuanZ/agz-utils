#pragma once

#include <ostream>
#include <string>

#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tvec4
{
public:

    using self_t = tvec4<T>;
    using elem_t = T;

    T x, y, z, w;

    AGZ_MATH_API tvec4()                         noexcept;
    AGZ_MATH_API tvec4(T x, T y, T z, T w)       noexcept;
    AGZ_MATH_API tvec4(const tvec3<T> &xyz, T w) noexcept;
    AGZ_MATH_API explicit tvec4(T val)           noexcept;
    AGZ_MATH_API explicit tvec4(uninitialized_t) noexcept;
    AGZ_MATH_API explicit tvec4(const tvec<T, 4> &v) noexcept;
    AGZ_MATH_API explicit tvec4(const tcolor4<T> &c) noexcept;

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

    std::string to_string() const;

    AGZ_MATH_API tvec3<T> homogenize() const noexcept;

#include "../impl/swizzle_vec4.inl"
};

template<typename T>
AGZ_MATH_API tvec4<T> operator-(const tvec4<T> &vec) noexcept;

template<typename T>
AGZ_MATH_API tvec4<T> operator+(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec4<T> operator-(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec4<T> operator*(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec4<T> operator/(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> AGZ_MATH_API tvec4<T> operator+(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec4<T> operator-(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec4<T> operator*(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec4<T> operator/(const tvec4<T> &lhs, T rhs) noexcept;

template<typename T> AGZ_MATH_API tvec4<T> operator+(T lhs, const tvec4<T> &rhs) noexcept;
template<typename T> AGZ_MATH_API tvec4<T> operator*(T lhs, const tvec4<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator==(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API bool operator!=(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API auto dot(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API auto cos(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> std::ostream &operator<<(
    std::ostream &out, const tvec4<T> &vec);

using vec4f = tvec4<float>;
using vec4d = tvec4<double>;
using vec4i = tvec4<int>;
using vec4b = tvec4<uint8_t>;

AGZ_MATH_END
