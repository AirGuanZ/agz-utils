#pragma once

#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tcolor2
{
public:

    using self_t = tcolor2<T>;
    using elem_t = T;

    T c, a;

    AGZ_MATH_API tcolor2()                           noexcept;
    AGZ_MATH_API tcolor2(T c, T a)                   noexcept;
    AGZ_MATH_API explicit tcolor2(T val)             noexcept;
    AGZ_MATH_API explicit tcolor2(uninitialized_t)   noexcept;
    AGZ_MATH_API explicit tcolor2(const tvec2<T> &c) noexcept;

    AGZ_MATH_API self_t clamp(T min_v, T max_v) const noexcept;
    AGZ_MATH_API self_t clamp_low(T min_v)      const noexcept;
    AGZ_MATH_API self_t clamp_high(T max_v)     const noexcept;

    AGZ_MATH_API self_t saturate() const noexcept;

    AGZ_MATH_API bool is_finite() const noexcept;

    template<typename F>
    AGZ_MATH_API auto map(F &&f) const noexcept;

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

#include "../impl/swizzle_color2.inl"
};

template<typename T>
AGZ_MATH_API tcolor2<T> operator-(
    const tcolor2<T> &color) noexcept;

template<typename T>
AGZ_MATH_API tcolor2<T> operator+(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator-(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator*(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator/(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API tcolor2<T> operator+(
    const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator-(
    const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator*(
    const tcolor2<T> &lhs, T rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator/(
    const tcolor2<T> &lhs, T rhs) noexcept;

template<typename T>
AGZ_MATH_API tcolor2<T> operator+(
    T lhs, const tcolor2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tcolor2<T> operator*(
    T lhs, const tcolor2<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator==(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API bool operator!=(
    const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept;

using color2f = tcolor2<float>;
using color2d = tcolor2<double>;
using color2b = tcolor2<unsigned char>;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
AGZ_MATH_API color2b to_color2b(const tcolor2<T> &c) noexcept;

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
AGZ_MATH_API tcolor2<T> from_color2b(const color2b &c) noexcept;

AGZ_MATH_END
