#pragma once

#include <cmath>

#include "../decl/common.h"

namespace agz::math {

template<typename T>
tvec4<T>::tvec4() noexcept
    : tvec4(T(0))
{

}

template<typename T>
tvec4<T>::tvec4(T x, T y, T z, T w) noexcept
    : x(x), y(y), z(z), w(w)
{

}

template<typename T>
tvec4<T>::tvec4(T val) noexcept
    : tvec4(val, val, val, val)
{

}

template<typename T>
tvec4<T>::tvec4(const tvec<T, 4> &v) noexcept
    : x(v[0]), y(v[1]), z(v[2]), w(v[3])
{

}

template<typename T>
tvec4<T>::tvec4(uninitialized_t) noexcept
{

}

template<typename T>
tvec4<T>::tvec4(const tcolor4<T> &c) noexcept
    : tvec4(c.r, c.g, c.b, c.a)
{

}

template<typename T>
bool tvec4<T>::is_zero() const noexcept
{
    return !x && !y && !z && !w;
}

template<typename T>
auto tvec4<T>::length() const noexcept
{
    return std::sqrt(length_square());
}

template<typename T>
auto tvec4<T>::length_square() const noexcept
{
    return x * x + y * y + z * z + w * w;
}

template<typename T>
typename tvec4<T>::self_t tvec4<T>::normalize() const noexcept
{
    static_assert(std::is_floating_point_v<T>);
    return *this / length();
}

template<typename T>
typename tvec4<T>::self_t tvec4<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(
        ::agz::math::clamp(x, min_v, max_v),
        ::agz::math::clamp(y, min_v, max_v),
        ::agz::math::clamp(z, min_v, max_v),
        ::agz::math::clamp(w, min_v, max_v));
}

template<typename T>
typename tvec4<T>::self_t tvec4<T>::clamp_low(T min_v) const noexcept
{
    return self_t(
        std::max(x, min_v),
        std::max(y, min_v),
        std::max(z, min_v),
        std::max(w, min_v));
}

template<typename T>
typename tvec4<T>::self_t tvec4<T>::clamp_high(T max_v) const noexcept
{
    return self_t(
        std::min(x, max_v),
        std::min(y, max_v),
        std::min(z, max_v),
        std::min(w, max_v));
}

template<typename T>
typename tvec4<T>::self_t tvec4<T>::saturate() const noexcept
{
    return self_t(::agz::math::saturate(x), ::agz::math::saturate(y), ::agz::math::saturate(z), ::agz::math::saturate(w));
}

template<typename T>
auto tvec4<T>::sum() const noexcept
{
    return x + y + z + w;
}

template<typename T>
auto tvec4<T>::product() const noexcept
{
    return x * y * z * w;
}

template<typename T>
T tvec4<T>::max_elem() const noexcept
{
    return (std::max)({ x, y, z, w });
}

template<typename T>
T tvec4<T>::min_elem() const noexcept
{
    return (std::min)({ x, y, z, w });
}

template<typename T>
template<typename F>
auto tvec4<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(x))>;
    return tvec4<ret_elem_t>(f(x), f(y), f(z), f(w));
}

template<typename T>
bool tvec4<T>::operator!() const noexcept
{
    return is_zero();
}

template<typename T>
T &tvec4<T>::operator[](size_t idx) noexcept
{
    return *(&x + idx);
}

template<typename T>
const T &tvec4<T>::operator[](size_t idx) const noexcept
{
    return *(&x + idx);
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator+=(const self_t &rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator-=(const self_t &rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator*=(const self_t &rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator/=(const self_t &rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator+=(T rhs) noexcept
{
    x += rhs;
    y += rhs;
    z += rhs;
    w += rhs;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator-=(T rhs) noexcept
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
    w -= rhs;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator*=(T rhs) noexcept
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

template<typename T>
typename tvec4<T>::self_t &tvec4<T>::operator/=(T rhs) noexcept
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

template<typename T>
tvec4<T> operator-(const tvec4<T> &vec) noexcept
{
    return tvec4<T>(-vec.x, -vec.y, -vec.z, -vec.w);
}

template<typename T>
tvec4<T> operator+(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return tvec4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template<typename T>
tvec4<T> operator-(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return tvec4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

template<typename T>
tvec4<T> operator*(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return tvec4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

template<typename T>
tvec4<T> operator/(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return tvec4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

template<typename T>
tvec4<T> operator+(const tvec4<T> &lhs, T rhs) noexcept
{
    return tvec4<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
}

template<typename T>
tvec4<T> operator-(const tvec4<T> &lhs, T rhs) noexcept
{
    return tvec4<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
}

template<typename T>
tvec4<T> operator*(const tvec4<T> &lhs, T rhs) noexcept
{
    return tvec4<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

template<typename T>
tvec4<T> operator/(const tvec4<T> &lhs, T rhs) noexcept
{
    return tvec4<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

template<typename T>
tvec4<T> operator+(T lhs, const tvec4<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tvec4<T> operator*(T lhs, const tvec4<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<typename T>
bool operator!=(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
}

template<typename T>
auto dot(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;;
}

template<typename T>
auto cos(const tvec4<T> &lhs, const tvec4<T> &rhs) noexcept
{
    return dot(lhs, rhs) / (lhs.length() * rhs.length());
}

} // namespace agz::math
