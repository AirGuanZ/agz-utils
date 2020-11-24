#pragma once

#include <cmath>

#include "../decl/common.h"

namespace agz::math {

template<typename T>
constexpr tvec3<T>::tvec3() noexcept
    : tvec3(T(0))
{

}

template<typename T>
constexpr tvec3<T>::tvec3(T x, T y, T z) noexcept
    : x(x), y(y), z(z)
{

}

template<typename T>
constexpr tvec3<T>::tvec3(T val) noexcept
    : tvec3(val, val, val)
{

}

template<typename T>
tvec3<T>::tvec3(const tvec<T, 3> &v) noexcept
    : x(v[0]), y(v[1]), z(v[2])
{

}

template<typename T>
tvec3<T>::tvec3(uninitialized_t) noexcept
{

}

template<typename T>
tvec3<T>::tvec3(const tcolor3<T> &c) noexcept
    : tvec3(c.r, c.g, c.b)
{
    
}

template<typename T>
bool tvec3<T>::is_zero() const noexcept
{
    return !x && !y && !z;
}

template<typename T>
auto tvec3<T>::length() const noexcept
{
    return std::sqrt(length_square());
}

template<typename T>
auto tvec3<T>::length_square() const noexcept
{
    return x * x + y * y + z * z;
}

template<typename T>
typename tvec3<T>::self_t tvec3<T>::normalize() const noexcept
{
    static_assert(std::is_floating_point_v<T>);
    return *this / length();
}

template<typename T>
typename tvec3<T>::self_t tvec3<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(
        ::agz::math::clamp(x, min_v, max_v),
        ::agz::math::clamp(y, min_v, max_v),
        ::agz::math::clamp(z, min_v, max_v));
}

template<typename T>
typename tvec3<T>::self_t tvec3<T>::clamp_low(T min_v) const noexcept
{
    return self_t((std::max)(x, min_v), (std::max)(y, min_v), (std::max)(z, min_v));
}

template<typename T>
typename tvec3<T>::self_t tvec3<T>::clamp_high(T max_v) const noexcept
{
    return self_t((std::min)(x, max_v), (std::min)(y, max_v), (std::min)(z, max_v));
}

template<typename T>
typename tvec3<T>::self_t tvec3<T>::saturate() const noexcept
{
    return self_t(
        ::agz::math::saturate(x),
        ::agz::math::saturate(y),
        ::agz::math::saturate(z));
}

template<typename T>
auto tvec3<T>::sum() const noexcept
{
    return x + y + z;
}

template<typename T>
auto tvec3<T>::product() const noexcept
{
    return x * y * z;
}

template<typename T>
T tvec3<T>::max_elem() const noexcept
{
    return (std::max)({ x, y, z });
}

template<typename T>
T tvec3<T>::min_elem() const noexcept
{
    return (std::min)({ x, y, z });
}

template<typename T>
template<typename F>
auto tvec3<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(x))>;
    return tvec3<ret_elem_t>(f(x), f(y), f(z));
}

template<typename T>
bool tvec3<T>::operator!() const noexcept
{
    return is_zero();
}

template<typename T>
T &tvec3<T>::operator[](size_t idx) noexcept
{
    return *(&x + idx);
}

template<typename T>
const T &tvec3<T>::operator[](size_t idx) const noexcept
{
    return *(&x + idx);
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator+=(const self_t &rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator-=(const self_t &rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator*=(const self_t &rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator/=(const self_t &rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator+=(T rhs) noexcept
{
    x += rhs;
    y += rhs;
    z += rhs;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator-=(T rhs) noexcept
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator*=(T rhs) noexcept
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

template<typename T>
typename tvec3<T>::self_t &tvec3<T>::operator/=(T rhs) noexcept
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

template<typename T>
tvec3<T> operator-(const tvec3<T> &vec) noexcept
{
    return tvec3<T>(-vec.x, -vec.y, -vec.z);
}

template<typename T>
tvec3<T> operator+(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return tvec3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template<typename T>
tvec3<T> operator-(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return tvec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template<typename T>
tvec3<T> operator*(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return tvec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

template<typename T>
tvec3<T> operator/(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return tvec3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

template<typename T>
tvec3<T> operator+(const tvec3<T> &lhs, T rhs) noexcept
{
    return tvec3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

template<typename T>
tvec3<T> operator-(const tvec3<T> &lhs, T rhs) noexcept
{
    return tvec3<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

template<typename T>
tvec3<T> operator*(const tvec3<T> &lhs, T rhs) noexcept
{
    return tvec3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template<typename T>
tvec3<T> operator/(const tvec3<T> &lhs, T rhs) noexcept
{
    return tvec3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

template<typename T>
tvec3<T> operator+(T lhs, const tvec3<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tvec3<T> operator*(T lhs, const tvec3<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<typename T>
bool operator!=(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

template<typename T>
bool operator<(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return lhs.x < rhs.x ||
           (lhs.x == rhs.x && lhs.y < rhs.y) ||
           (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z);
}

template<typename T>
auto dot(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename T>
auto cross(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return tvec3<T>(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

template<typename T>
auto cos(const tvec3<T> &lhs, const tvec3<T> &rhs) noexcept
{
    return dot(lhs, rhs) / (lhs.length() * rhs.length());
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const tvec3<T> &vec)
{
    return out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

} // namespace agz::math
