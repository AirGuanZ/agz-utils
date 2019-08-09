#pragma once

#include "common.h"

namespace agz::math {

template<typename T>
class tcolor2
{
public:

    using self_t = tcolor2<T>;
    using elem_t = T;

    T c, a;

    tcolor2()                           noexcept;
    tcolor2(T c, T a)                   noexcept;
    explicit tcolor2(T val)             noexcept;
    explicit tcolor2(uninitialized_t)   noexcept;
    explicit tcolor2(const tvec2<T> &c) noexcept;

    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low(T min_v)      const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

    bool is_finite() const noexcept;

    template<typename F> auto map(F &&f) const noexcept;

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

#include "../impl/swizzle_color2.inl"
};

template<typename T> tcolor2<T> operator-(const tcolor2<T> &color) noexcept;

template<typename T> tcolor2<T> operator+(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T> tcolor2<T> operator-(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T> tcolor2<T> operator*(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T> tcolor2<T> operator/(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;

template<typename T> tcolor2<T> operator+(const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T> tcolor2<T> operator-(const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T> tcolor2<T> operator*(const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T> tcolor2<T> operator/(const tcolor2<T> &lhs, T rhs) noexcept;

template<typename T> tcolor2<T> operator+(T lhs, const tcolor2<T> &rhs) noexcept;
template<typename T> tcolor2<T> operator*(T lhs, const tcolor2<T> &rhs) noexcept;

template<typename T> bool operator==(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T> bool operator!=(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;

using color2f = tcolor2<float>;
using color2d = tcolor2<double>;
using color2b = tcolor2<unsigned char>;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
color2b to_color2b(const tcolor2<T> &c) noexcept;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
tcolor2<T> from_color2b(const color2b &c) noexcept;

} // namespace agz::math
