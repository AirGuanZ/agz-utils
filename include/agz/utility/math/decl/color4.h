#pragma once

#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tcolor4
{
public:

    using self_t = tcolor4<T>;
    using elem_t = T;

    T r, g, b, a;

    tcolor4()                           noexcept;
    tcolor4(T val, T a)                 noexcept;
    tcolor4(T r, T g, T b, T a = 1)     noexcept;
    explicit tcolor4(T val)             noexcept;
    explicit tcolor4(uninitialized_t)   noexcept;
    explicit tcolor4(const tvec4<T> &c) noexcept;
    
    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low (T min_v)     const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

    self_t saturate() const noexcept;

    auto lum()                const noexcept;
    auto relative_luminance() const noexcept;

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

#include "../impl/swizzle_color4.inl"
};

template<typename T> tcolor4<T> operator-(const tcolor4<T> &color) noexcept;

template<typename T> tcolor4<T> operator+(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;
template<typename T> tcolor4<T> operator-(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;
template<typename T> tcolor4<T> operator*(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;
template<typename T> tcolor4<T> operator/(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;

template<typename T> tcolor4<T> operator+(
    const tcolor4<T> &lhs, T rhs) noexcept;
template<typename T> tcolor4<T> operator-(
    const tcolor4<T> &lhs, T rhs) noexcept;
template<typename T> tcolor4<T> operator*(
    const tcolor4<T> &lhs, T rhs) noexcept;
template<typename T> tcolor4<T> operator/(
    const tcolor4<T> &lhs, T rhs) noexcept;

template<typename T> tcolor4<T> operator+(
    T lhs, const tcolor4<T> &rhs) noexcept;
template<typename T> tcolor4<T> operator*(
    T lhs, const tcolor4<T> &rhs) noexcept;

template<typename T> bool operator==(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;
template<typename T> bool operator!=(
    const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept;

using color4f = tcolor4<float>;
using color4d = tcolor4<double>;
using color4b = tcolor4<unsigned char>;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
color4b to_color4b(const tcolor4<T> &c) noexcept;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
tcolor4<T> from_color4b(const color4b &c) noexcept;

AGZ_MATH_END
