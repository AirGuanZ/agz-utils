#pragma once

#include "../decl/func.h"

namespace agz::math {

template<typename T>
tcolor4<T>::tcolor4() noexcept
    : tcolor4(0)
{
    
}

template<typename T>
tcolor4<T>::tcolor4(T r, T g, T b, T a) noexcept
    : r(r), g(g), b(b), a(a)
{
    
}

template<typename T>
tcolor4<T>::tcolor4(T val, T a) noexcept
    : tcolor4(val, val, val, a)
{
    
}

template<typename T>
tcolor4<T>::tcolor4(T val) noexcept
    : tcolor4(val, val, val, val)
{
    
}

template<typename T>
tcolor4<T>::tcolor4(uninitialized_t) noexcept
{
    
}

template<typename T>
tcolor4<T>::tcolor4(const tvec4<T> &c) noexcept
    : tcolor4(c.x, c.y, c.z, c.w)
{

}

template<typename T>
typename tcolor4<T>::self_t tcolor4<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(::agz::math::clamp(r, min_v, max_v),
                  ::agz::math::clamp(g, min_v, max_v),
                  ::agz::math::clamp(b, min_v, max_v),
                  ::agz::math::clamp(a, min_v, max_v));
}

template<typename T>
typename tcolor4<T>::self_t tcolor4<T>::clamp_low(T min_v) const noexcept
{
    return self_t(std::max(r, min_v),
                  std::max(g, min_v),
                  std::max(b, min_v),
                  std::max(a, min_v));
}

template<typename T>
typename tcolor4<T>::self_t tcolor4<T>::clamp_high(T max_v) const noexcept
{
    return self_t(std::min(r, max_v),
                  std::min(g, max_v),
                  std::min(b, max_v),
                  std::min(a, max_v));
}

template<typename T>
typename tcolor4<T>::self_t tcolor4<T>::saturate() const noexcept
{
    return self_t(::agz::math::saturate(r),
                  ::agz::math::saturate(g),
                  ::agz::math::saturate(b),
                  ::agz::math::saturate(a));
}

template<typename T>
auto tcolor4<T>::lum() const noexcept
{
    return T(0.2126) * r + T(0.7152) * g + T(0.0722) * b;
}

template<typename T>
auto tcolor4<T>::relative_luminance() const noexcept
{
    return lum();
}

template<typename T>
bool tcolor4<T>::is_finite() const noexcept
{
    return math::is_finite(r) && math::is_finite(g) &&
           math::is_finite(b) && math::is_finite(a);
}

template<typename T>
template<typename F>
auto tcolor4<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(r))>;
    return tcolor4<ret_elem_t>(f(r), f(g), f(b), f(a));
}

template<typename T>
T &tcolor4<T>::operator[](size_t idx) noexcept
{
    return *(&r + idx);
}

template<typename T>
const T &tcolor4<T>::operator[](size_t idx) const noexcept
{
    return *(&r + idx);
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator+=(const self_t &rhs) noexcept
{
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator-=(const self_t &rhs) noexcept
{
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    a -= rhs.a;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator*=(const self_t &rhs) noexcept
{
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    a *= rhs.a;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator/=(const self_t &rhs) noexcept
{
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    a /= rhs.a;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator+=(T rhs) noexcept
{
    r += rhs;
    g += rhs;
    b += rhs;
    a += rhs;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator-=(T rhs) noexcept
{
    r -= rhs;
    g -= rhs;
    b -= rhs;
    a -= rhs;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator*=(T rhs) noexcept
{
    r *= rhs;
    g *= rhs;
    b *= rhs;
    a *= rhs;
    return *this;
}

template<typename T>
typename tcolor4<T>::self_t &tcolor4<T>::operator/=(T rhs) noexcept
{
    r /= rhs;
    g /= rhs;
    b /= rhs;
    a /= rhs;
    return *this;
}

template<typename T>
tcolor4<T> operator-(const tcolor4<T> &color) noexcept
{
    return tcolor4<T>(-color.r, -color.g, -color.b, -color.a);
}

template<typename T>
tcolor4<T> operator+(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return tcolor4<T>(lhs.r + rhs.r,
                      lhs.g + rhs.g,
                      lhs.b + rhs.b,
                      lhs.a + rhs.a);
}

template<typename T>
tcolor4<T> operator-(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return tcolor4<T>(lhs.r - rhs.r,
                      lhs.g - rhs.g,
                      lhs.b - rhs.b,
                      lhs.a - rhs.a);
}

template<typename T>
tcolor4<T> operator*(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return tcolor4<T>(lhs.r * rhs.r,
                      lhs.g * rhs.g,
                      lhs.b * rhs.b,
                      lhs.a * rhs.a);
}

template<typename T>
tcolor4<T> operator/(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return tcolor4<T>(lhs.r / rhs.r,
                      lhs.g / rhs.g,
                      lhs.b / rhs.b,
                      lhs.a / rhs.a);
}

template<typename T>
tcolor4<T> operator+(const tcolor4<T> &lhs, T rhs) noexcept
{
    return tcolor4<T>(lhs.r + rhs,
                      lhs.g + rhs,
                      lhs.b + rhs,
                      lhs.a + rhs);
}

template<typename T>
tcolor4<T> operator-(const tcolor4<T> &lhs, T rhs) noexcept
{
    return tcolor4<T>(lhs.r - rhs,
                      lhs.g - rhs,
                      lhs.b - rhs,
                      lhs.a - rhs);
}

template<typename T>
tcolor4<T> operator*(const tcolor4<T> &lhs, T rhs) noexcept
{
    return tcolor4<T>(lhs.r * rhs,
                      lhs.g * rhs,
                      lhs.b * rhs,
                      lhs.a * rhs);
}

template<typename T>
tcolor4<T> operator/(const tcolor4<T> &lhs, T rhs) noexcept
{
    return tcolor4<T>(lhs.r / rhs,
                      lhs.g / rhs,
                      lhs.b / rhs,
                      lhs.a / rhs);
}

template<typename T>
tcolor4<T> operator+(T lhs, const tcolor4<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tcolor4<T> operator*(T lhs, const tcolor4<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

template<typename T>
bool operator!=(const tcolor4<T> &lhs, const tcolor4<T> &rhs) noexcept
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}

template<typename T, typename>
color4b to_color4b(const tcolor4<T> &c) noexcept
{
    return c.clamp(0, 1).map(
        [](T s) { return static_cast<unsigned char>(s * 255); });
}

template<typename T, typename>
tcolor4<T> from_color4b(const color4b &c) noexcept
{
    return c.map([](unsigned char s) { return s / T(255); });
}

} // namespace agz::math
