#pragma once

#include "common.h"

namespace agz::math {

template<typename T>
class tcolor3
{
public:

    using self_t = tcolor3<T>;
    using elem_t = T;

    T r, g, b;

    tcolor3()                           noexcept;
    tcolor3(T r, T g, T b)              noexcept;
    explicit tcolor3(T val)             noexcept;
    explicit tcolor3(uninitialized_t)   noexcept;
    explicit tcolor3(const tvec3<T> &c) noexcept;

    bool is_black() const noexcept;
    
    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low (T min_v)     const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

    self_t saturate() const noexcept;

    auto lum()                const noexcept;
    auto relative_luminance() const noexcept;

    bool is_finite() const noexcept;

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

#include "../impl/swizzle_color3.inl"
};

template<typename T> tcolor3<T> operator-(const tcolor3<T> &color) noexcept;

template<typename T> tcolor3<T> operator+(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T> tcolor3<T> operator-(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T> tcolor3<T> operator*(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T> tcolor3<T> operator/(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;

template<typename T> tcolor3<T> operator+(const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T> tcolor3<T> operator-(const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T> tcolor3<T> operator*(const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T> tcolor3<T> operator/(const tcolor3<T> &lhs, T rhs) noexcept;

template<typename T> tcolor3<T> operator+(T lhs, const tcolor3<T> &rhs) noexcept;
template<typename T> tcolor3<T> operator*(T lhs, const tcolor3<T> &rhs) noexcept;

template<typename T> bool operator==(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T> bool operator!=(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;

using color3f = tcolor3<float>;
using color3d = tcolor3<double>;
using color3b = tcolor3<unsigned char>;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
color3b to_color3b(const tcolor3<T> &c) noexcept;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
tcolor3<T> from_color3b(const color3b &c) noexcept;

} // namespace agz::math
