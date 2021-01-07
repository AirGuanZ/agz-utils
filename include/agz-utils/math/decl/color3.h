#pragma once

#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tcolor3
{
public:

    using self_t = tcolor3<T>;
    using elem_t = T;

    T r, g, b;

    AGZ_MATH_API tcolor3()                           noexcept;
    AGZ_MATH_API tcolor3(T r, T g, T b)              noexcept;
    AGZ_MATH_API explicit tcolor3(T val)             noexcept;
    AGZ_MATH_API explicit tcolor3(uninitialized_t)   noexcept;
    AGZ_MATH_API explicit tcolor3(const tvec3<T> &c) noexcept;

    AGZ_MATH_API bool is_black() const noexcept;
    
    AGZ_MATH_API self_t clamp(T min_v, T max_v) const noexcept;
    AGZ_MATH_API self_t clamp_low (T min_v)     const noexcept;
    AGZ_MATH_API self_t clamp_high(T max_v)     const noexcept;

    AGZ_MATH_API self_t saturate() const noexcept;

    AGZ_MATH_API auto lum()                const noexcept;
    AGZ_MATH_API auto relative_luminance() const noexcept;

    AGZ_MATH_API bool is_finite() const noexcept;

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

    AGZ_MATH_API bool operator<(const self_t &rhs) const noexcept;

#include "../impl/swizzle_color3.inl"
};

template<typename T>
AGZ_MATH_API tcolor3<T> operator-(
    const tcolor3<T> &color) noexcept;

template<typename T>
AGZ_MATH_API tcolor3<T> operator+(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator-(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator*(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator/(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API tcolor3<T> operator+(
    const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator-(
    const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator*(
    const tcolor3<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator/(
    const tcolor3<T> &lhs, T rhs) noexcept;

template<typename T>
AGZ_MATH_API tcolor3<T> operator+(
    T lhs, const tcolor3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor3<T> operator*(
    T lhs, const tcolor3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator==(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API bool operator!=(
    const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept;

using color3f = tcolor3<float>;
using color3d = tcolor3<double>;
using color3b = tcolor3<unsigned char>;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
AGZ_MATH_API color3b to_color3b(const tcolor3<T> &c) noexcept;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
AGZ_MATH_API tcolor3<T> from_color3b(const color3b &c) noexcept;

AGZ_MATH_END
