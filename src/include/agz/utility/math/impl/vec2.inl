#pragma once

#include <cmath>

namespace agz::math {

template<typename T>
tvec2<T>::tvec2() noexcept
    : tvec2(T(0))
{
    
}

template<typename T>
tvec2<T>::tvec2(T x, T y) noexcept
    : x(x), y(y)
{
    
}

template<typename T>
tvec2<T>::tvec2(T val) noexcept
    : tvec2(val, val)
{
    
}

template<typename T>
tvec2<T>::tvec2(const tvec<T, 2> &v) noexcept
    : x(v[0]), y(v[1])
{
    
}

template<typename T>
tvec2<T>::tvec2(const tcolor2<T> &c) noexcept
    : tvec2(c.c, c.a)
{

}

template<typename T>
tvec2<T>::tvec2(uninitialized_t) noexcept
{
    
}

template<typename T>
bool tvec2<T>::is_zero() const noexcept
{
    return !x && !y;
}

template<typename T>
auto tvec2<T>::length() const noexcept
{
    return std::sqrt(length_square());
}

template<typename T>
auto tvec2<T>::length_square() const noexcept
{
    return x * x + y * y;
}

template<typename T>
typename tvec2<T>::self_t tvec2<T>::normalize() const noexcept
{
    static_assert(std::is_floating_point_v<T>);
    return *this / length();
}

template<typename T>
typename tvec2<T>::self_t tvec2<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(::agz::math::clamp(x, min_v, max_v), ::agz::math::clamp(y, min_v, max_v));
}

template<typename T>
typename tvec2<T>::self_t tvec2<T>::clamp_low(T min_v) const noexcept
{
    return self_t(std::max(x, min_v), std::max(y, min_v));
}

template<typename T>
typename tvec2<T>::self_t tvec2<T>::clamp_high(T max_v) const noexcept
{
    return self_t(std::min(x, max_v), std::min(y, max_v));
}

template<typename T>
auto tvec2<T>::sum() const noexcept
{
    return x + y;
}

template<typename T>
auto tvec2<T>::product() const noexcept
{
    return x * y;
}

template<typename T>
template<typename F>
auto tvec2<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(x))>;
    return tvec2<ret_elem_t>(f(x), f(y));
}

template<typename T>
bool tvec2<T>::operator!() const noexcept
{
    return is_zero();
}

template<typename T>
T &tvec2<T>::operator[](size_t idx) noexcept
{
    return *(&x + idx);
}

template<typename T>
const T &tvec2<T>::operator[](size_t idx) const noexcept
{
    return *(&x + idx);
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator+=(const self_t &rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator-=(const self_t &rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator*=(const self_t &rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator/=(const self_t &rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator+=(T rhs) noexcept
{
    x += rhs;
    y += rhs;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator-=(T rhs) noexcept
{
    x -= rhs;
    y -= rhs;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator*=(T rhs) noexcept
{
    x *= rhs;
    y *= rhs;
    return *this;
}

template<typename T>
typename tvec2<T>::self_t &tvec2<T>::operator/=(T rhs) noexcept
{
    x /= rhs;
    y /= rhs;
    return *this;
}

template<typename T>
tvec2<T> operator-(const tvec2<T> &vec) noexcept
{
    return tvec2<T>(-vec.x, -vec.y);
}

template<typename T>
tvec2<T> operator+(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return tvec2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename T>
tvec2<T> operator-(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return tvec2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<typename T>
tvec2<T> operator*(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return tvec2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template<typename T>
tvec2<T> operator/(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return tvec2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template<typename T>
tvec2<T> operator+(const tvec2<T> &lhs, T rhs) noexcept
{
    return tvec2<T>(lhs.x + rhs, lhs.y + rhs);
}

template<typename T>
tvec2<T> operator-(const tvec2<T> &lhs, T rhs) noexcept
{
    return tvec2<T>(lhs.x - rhs, lhs.y - rhs);
}

template<typename T>
tvec2<T> operator*(const tvec2<T> &lhs, T rhs) noexcept
{
    return tvec2<T>(lhs.x * rhs, lhs.y * rhs);
}

template<typename T>
tvec2<T> operator/(const tvec2<T> &lhs, T rhs) noexcept
{
    return tvec2<T>(lhs.x / rhs, lhs.y / rhs);
}

template<typename T>
tvec2<T> operator+(T lhs, const tvec2<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tvec2<T> operator*(T lhs, const tvec2<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename T>
bool operator!=(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

template<typename T>
auto dot(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T>
auto cross(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

template<typename T>
auto cos(const tvec2<T> &lhs, const tvec2<T> &rhs) noexcept
{
    return dot(lhs, rhs) / (lhs.length() * rhs.length());
}

} // namespace agz::math
