#pragma once

#include "../decl/func.h"

namespace agz::math {

template<typename T>
tcolor2<T>::tcolor2() noexcept
    : tcolor2(0)
{

}

template<typename T>
tcolor2<T>::tcolor2(T c, T a) noexcept
    : c(c), a(a)
{

}

template<typename T>
tcolor2<T>::tcolor2(T val) noexcept
    : tcolor2(val, val)
{

}

template<typename T>
tcolor2<T>::tcolor2(uninitialized_t) noexcept
{

}

template<typename T>
tcolor2<T>::tcolor2(const tvec2<T> &c) noexcept
    : tcolor2(c.x, c.y)
{

}

template<typename T>
typename tcolor2<T>::self_t tcolor2<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(::agz::math::clamp(c, min_v, max_v),
                  ::agz::math::clamp(a, min_v, max_v));
}

template<typename T>
typename tcolor2<T>::self_t tcolor2<T>::clamp_low(T min_v) const noexcept
{
    return self_t(std::max(c, min_v),
                  std::max(a, min_v));
}

template<typename T>
typename tcolor2<T>::self_t tcolor2<T>::clamp_high(T max_v) const noexcept
{
    return self_t(std::min(c, max_v),
                  std::min(a, max_v));
}

template<typename T>
typename tcolor2<T>::self_t tcolor2<T>::saturate() const noexcept
{
    return self_t(::agz::math::saturate(c), ::agz::math::saturate(a));
}

template<typename T>
bool tcolor2<T>::is_finite() const noexcept
{
    return math::is_finite(c) && math::is_finite(a);
}

template<typename T>
template<typename F>
auto tcolor2<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(c))>;
    return tcolor2<ret_elem_t>(f(c), f(a));
}

template<typename T>
T &tcolor2<T>::operator[](size_t idx) noexcept
{
    return *(&c + idx);
}

template<typename T>
const T &tcolor2<T>::operator[](size_t idx) const noexcept
{
    return *(&c + idx);
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator+=(const self_t &rhs) noexcept
{
    c += rhs.c;
    a += rhs.a;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator-=(const self_t &rhs) noexcept
{
    c -= rhs.c;
    a -= rhs.a;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator*=(const self_t &rhs) noexcept
{
    c *= rhs.c;
    a *= rhs.a;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator/=(const self_t &rhs) noexcept
{
    c /= rhs.c;
    a /= rhs.a;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator+=(T rhs) noexcept
{
    c += rhs.c;
    a += rhs.a;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator-=(T rhs) noexcept
{
    c -= rhs;
    a -= rhs;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator*=(T rhs) noexcept
{
    c *= rhs;
    a *= rhs;
    return *this;
}

template<typename T>
typename tcolor2<T>::self_t &tcolor2<T>::operator/=(T rhs) noexcept
{
    c /= rhs;
    a /= rhs;
    return *this;
}

template<typename T>
tcolor2<T> operator-(const tcolor2<T> &color) noexcept
{
    return tcolor2<T>(-color.c, -color.a);
}

template<typename T>
tcolor2<T> operator+(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return tcolor2<T>(lhs.c + rhs.c,
                      lhs.a + rhs.a);
}

template<typename T>
tcolor2<T> operator-(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return tcolor2<T>(lhs.c - rhs.c,
                      lhs.a - rhs.a);
}

template<typename T>
tcolor2<T> operator*(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return tcolor2<T>(lhs.c * rhs.c,
                      lhs.a * rhs.a);
}

template<typename T>
tcolor2<T> operator/(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return tcolor2<T>(lhs.c / rhs.c,
                      lhs.a / rhs.a);
}

template<typename T>
tcolor2<T> operator+(const tcolor2<T> &lhs, T rhs) noexcept
{
    return tcolor2<T>(lhs.c + rhs,
                      lhs.a + rhs);
}

template<typename T>
tcolor2<T> operator-(const tcolor2<T> &lhs, T rhs) noexcept
{
    return tcolor2<T>(lhs.c - rhs,
                      lhs.a - rhs);
}

template<typename T>
tcolor2<T> operator*(const tcolor2<T> &lhs, T rhs) noexcept
{
    return tcolor2<T>(lhs.c * rhs,
                      lhs.a * rhs);
}

template<typename T>
tcolor2<T> operator/(const tcolor2<T> &lhs, T rhs) noexcept
{
    return tcolor2<T>(lhs.c / rhs,
                      lhs.a / rhs);
}

template<typename T>
tcolor2<T> operator+(T lhs, const tcolor2<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tcolor2<T> operator*(T lhs, const tcolor2<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return lhs.c == rhs.c && lhs.a == rhs.a;
}

template<typename T>
bool operator!=(const tcolor2<T> &lhs, const tcolor2<T> &rhs) noexcept
{
    return lhs.c != rhs.c || lhs.a != rhs.a;
}

template<typename T, typename>
color3b to_color3b(const tcolor2<T> &c) noexcept
{
    return c.clamp(0, 1).map([](T s) { return static_cast<unsigned char>(s * 255); });
}

template<typename T, typename>
tcolor2<T> from_color3b(const color3b &c) noexcept
{
    return c.map([](unsigned char s) { return s / T(255); });
}

} // namespace agz::math
