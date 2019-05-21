#pragma once

#include "common.h"
#include "vec.h"

AGZM_BEGIN

template<typename T>
class tvec3
{
public:

    using self_t = tvec3<T>;
    using elem_t = T;

    T x, y, z;

    tvec3()                             noexcept;
    tvec3(T x, T y, T z)                noexcept;
    explicit tvec3(T val)               noexcept;
    explicit tvec3(uninitialized_t)     noexcept;
    explicit tvec3(const tvec<T, 3> &v) noexcept;
    explicit tvec3(const tcolor3<T> &c) noexcept;

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

#include "impl/swizzle_vec3.inl"
};

template<typename T> tvec3<T> operator-(const tvec3<T> &vec) noexcept;

template<typename T> tvec3<T> operator+(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> tvec3<T> operator-(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> tvec3<T> operator*(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> tvec3<T> operator/(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T> tvec3<T> operator+(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> tvec3<T> operator-(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> tvec3<T> operator*(const tvec3<T> &lhs, T rhs) noexcept;
template<typename T> tvec3<T> operator/(const tvec3<T> &lhs, T rhs) noexcept;

template<typename T> tvec3<T> operator+(T lhs, const tvec3<T> &rhs) noexcept;
template<typename T> tvec3<T> operator*(T lhs, const tvec3<T> &rhs) noexcept;

template<typename T> bool operator==(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> bool operator!=(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

template<typename T> auto dot(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> auto cross(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;
template<typename T> auto cos(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept;

using vec3f = tvec3<float>;
using vec3d = tvec3<float>;
using vec3i = tvec3<float>;

AGZM_END
