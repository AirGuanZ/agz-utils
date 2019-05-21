#pragma once

#include "common.h"
#include "vec.h"

AGZM_BEGIN

template<typename T>
class tvec2
{
public:

    using self_t = tvec2<T>;
    using elem_t = T;

    T x, y;

             tvec2()                    noexcept;
             tvec2(T x, T y)            noexcept;
    explicit tvec2(T val)               noexcept;
    explicit tvec2(const tvec<T, 2> &v) noexcept;
    explicit tvec2(const tcolor2<T> &v) noexcept;
    explicit tvec2(uninitialized_t)     noexcept;

    bool is_zero() const noexcept;
    auto length()        const noexcept;
    auto length_square() const noexcept;

    self_t normalize() const noexcept;

    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low (T min_v)     const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

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

#include "../impl/swizzle_vec2.inl"
};

template<typename T> tvec2<T> operator-(const tvec2<T> &vec) noexcept;

template<typename T> tvec2<T> operator+(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> tvec2<T> operator-(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> tvec2<T> operator*(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> tvec2<T> operator/(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

template<typename T> tvec2<T> operator+(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> tvec2<T> operator-(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> tvec2<T> operator*(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> tvec2<T> operator/(const tvec2<T> &lhs, T rhs) noexcept;

template<typename T> tvec2<T> operator+(T lhs, const tvec2<T> &rhs) noexcept;
template<typename T> tvec2<T> operator*(T lhs, const tvec2<T> &rhs) noexcept;

template<typename T> bool operator==(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> bool operator!=(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

template<typename T> auto dot  (const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> auto cross(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T> auto cos  (const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

using vec2f = tvec2<float>;
using vec2d = tvec2<double>;
using vec2i = tvec2<int>;

AGZM_END