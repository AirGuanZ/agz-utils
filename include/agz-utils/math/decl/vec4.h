﻿#pragma once

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

    tvec4()                         noexcept;
    tvec4(T x, T y, T z, T w)       noexcept;
    tvec4(const tvec3<T> &xyz, T w) noexcept;
    explicit tvec4(T val)           noexcept;
    explicit tvec4(uninitialized_t) noexcept;
    explicit tvec4(const tvec<T, 4> &v) noexcept;
    explicit tvec4(const tcolor4<T> &c) noexcept;

    bool is_zero() const noexcept;

    auto length()        const noexcept;
    auto length_square() const noexcept;

    self_t normalize() const noexcept;

    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low (T min_v)     const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

    self_t saturate() const noexcept;

    auto sum()     const noexcept;
    auto product() const noexcept;

    T max_elem() const noexcept;
    T min_elem() const noexcept;

    template<typename F>
    auto map(F &&f) const noexcept;

    template<typename U>
    auto to() const noexcept
    {
        return tvec4<U>(U(x), U(y), U(z), U(w));
    }

    bool operator!() const noexcept;
    T       &operator[](size_t idx)       noexcept;
    const T &operator[](size_t idx) const noexcept;

    self_t &operator+=(const self_t &rhs) noexcept;
    self_t &operator-=(const self_t &rhs) noexcept;
    self_t &operator*=(const self_t &rhs) noexcept;
    self_t &operator/=(const self_t &rhs) noexcept;

    self_t &operator+=(T rhs) noexcept;
    self_t &operator-=(T rhs) noexcept;
    self_t &operator*=(T rhs) noexcept;
    self_t &operator/=(T rhs) noexcept;

    std::string to_string() const;

    tvec3<T> homogenize() const noexcept;

#include "../impl/swizzle_vec4.inl"
};

template<typename T>
tvec4<T> operator-(const tvec4<T> &vec) noexcept;

template<typename T>
tvec4<T> operator+(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
tvec4<T> operator-(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
tvec4<T> operator*(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
tvec4<T> operator/(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> tvec4<T> operator+(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator-(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator*(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator/(const tvec4<T> &lhs, T rhs) noexcept;

template<typename T> tvec4<T> operator+(T lhs, const tvec4<T> &rhs) noexcept;
template<typename T> tvec4<T> operator*(T lhs, const tvec4<T> &rhs) noexcept;

template<typename T>
bool operator==(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
bool operator!=(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T>
auto dot(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T>
auto cos(
    const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> std::ostream &operator<<(
    std::ostream &out, const tvec4<T> &vec);

using vec4f = tvec4<float>;
using vec4d = tvec4<double>;
using vec4i = tvec4<int>;
using vec4b = tvec4<uint8_t>;

AGZ_MATH_END
