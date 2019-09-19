#pragma once

#include "common.h"
#include "vec.h"

namespace agz::math {

template<typename T>
class tvec4
{
public:

    using self_t = tvec4<T>;
    using elem_t = T;

    T x, y, z, w;

    tvec4()                         noexcept;
    tvec4(T x, T y, T z, T w)       noexcept;
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

    template<typename F> auto map(F &&f) const noexcept;

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

#include "../impl/swizzle_vec4.inl"
};

template<typename T> tvec4<T> operator-(const tvec4<T> &vec) noexcept;

template<typename T> tvec4<T> operator+(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T> tvec4<T> operator-(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T> tvec4<T> operator*(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T> tvec4<T> operator/(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> tvec4<T> operator+(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator-(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator*(const tvec4<T> &lhs, T rhs) noexcept;
template<typename T> tvec4<T> operator/(const tvec4<T> &lhs, T rhs) noexcept;

template<typename T> tvec4<T> operator+(T lhs, const tvec4<T> &rhs) noexcept;
template<typename T> tvec4<T> operator*(T lhs, const tvec4<T> &rhs) noexcept;

template<typename T> bool operator==(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T> bool operator!=(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

template<typename T> auto dot(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;
template<typename T> auto cos(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept;

using vec4f = tvec4<float>;
using vec4d = tvec4<double>;
using vec4i = tvec4<int>;

} // namespace agz::math
