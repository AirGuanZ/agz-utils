#pragma once

#include <ostream>
#include <string>

#include "../../misc/hash.h"
#include "common.h"

AGZ_MATH_BEGIN

template<typename T>
class tvec2
{
public:

    using self_t = tvec2<T>;
    using elem_t = T;

    T x, y;

    AGZ_MATH_API          constexpr tvec2()                    noexcept;
    AGZ_MATH_API          constexpr tvec2(T x, T y)            noexcept;
    AGZ_MATH_API explicit constexpr tvec2(T val)               noexcept;
    AGZ_MATH_API explicit tvec2(const tvec<T, 2> &v) noexcept;
    AGZ_MATH_API explicit tvec2(const tcolor2<T> &v) noexcept;
    AGZ_MATH_API explicit tvec2(uninitialized_t)     noexcept;

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

#include "../impl/swizzle_vec2.inl"

    AGZ_MATH_API bool operator<(const tvec2 &rhs) const noexcept;
};

template<typename T>
AGZ_MATH_API tvec2<T> operator-(const tvec2<T> &vec) noexcept;

template<typename T>
AGZ_MATH_API tvec2<T> operator+(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec2<T> operator-(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec2<T> operator*(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API tvec2<T> operator/(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

template<typename T> AGZ_MATH_API tvec2<T> operator+(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec2<T> operator-(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec2<T> operator*(const tvec2<T> &lhs, T rhs) noexcept;
template<typename T> AGZ_MATH_API tvec2<T> operator/(const tvec2<T> &lhs, T rhs) noexcept;

template<typename T> AGZ_MATH_API tvec2<T> operator+(T lhs, const tvec2<T> &rhs) noexcept;
template<typename T> AGZ_MATH_API tvec2<T> operator*(T lhs, const tvec2<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API bool operator==(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API bool operator!=(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API auto dot(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API auto cross(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;
template<typename T>
AGZ_MATH_API auto cos(
    const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept;

template<typename T>
std::ostream &operator<<(
    std::ostream &out, const tvec2<T> &vec);

using vec2f = tvec2<float>;
using vec2d = tvec2<double>;
using vec2i = tvec2<int>;
using vec2b = tvec2<uint8_t>;

AGZ_MATH_END

#ifndef __CUDACC__

namespace std
{
    template<typename T>
    struct hash<agz::math::tvec2<T>>
    {
        size_t operator()(const agz::math::tvec2<T> &vec) const noexcept
        {
            return agz::misc::hash(vec.x, vec.y);
        }
    };
}

#endif
