#pragma once

AGZM_BEGIN

template<typename T>
tcolor3<T>::tcolor3() noexcept
    : tcolor3(0)
{
    
}

template<typename T>
tcolor3<T>::tcolor3(T r, T g, T b) noexcept
    : r(r), g(g), b(b)
{
    
}

template<typename T>
tcolor3<T>::tcolor3(T val) noexcept
    : tcolor3(val, val, val)
{
    
}

template<typename T>
tcolor3<T>::tcolor3(uninitialized_t) noexcept
{
    
}

template<typename T>
tcolor3<T>::tcolor3(const tvec3<T> &c) noexcept
    : tcolor3(c.x, c.y, c.z)
{
    
}

template<typename T>
bool tcolor3<T>::is_black() const noexcept
{
    return !r && !g && !b;
}

template<typename T>
typename tcolor3<T>::self_t tcolor3<T>::clamp(T min_v, T max_v) const noexcept
{
    return self_t(::agz::math::clamp(r, min_v, max_v),
                  ::agz::math::clamp(g, min_v, max_v),
                  ::agz::math::clamp(b, min_v, max_v));
}

template<typename T>
typename tcolor3<T>::self_t tcolor3<T>::clamp_low(T min_v) const noexcept
{
    return self_t(std::max(r, min_v),
                  std::max(g, min_v),
                  std::max(b, min_v));
}

template<typename T>
typename tcolor3<T>::self_t tcolor3<T>::clamp_high(T max_v) const noexcept
{
    return self_t(std::min(r, max_v),
                  std::min(g, max_v),
                  std::min(b, max_v));
}

template<typename T>
auto tcolor3<T>::lum() const noexcept
{
    return T(0.2126) * r + T(0.7152) * g + T(0.0722) * b;
}

template<typename T>
auto tcolor3<T>::relative_luminance() const noexcept
{
    return lum();
}

template<typename T>
template<typename F>
auto tcolor3<T>::map(F &&f) const noexcept
{
    using ret_elem_t = rm_rcv_t<decltype(f(r))>;
    return tcolor3<ret_elem_t>(f(r), f(g), f(b));
}

template<typename T>
bool tcolor3<T>::operator!() const noexcept
{
    return is_black();
}

template<typename T>
T &tcolor3<T>::operator[](size_t idx) noexcept
{
    return *(&r + idx);
}

template<typename T>
const T &tcolor3<T>::operator[](size_t idx) const noexcept
{
    return *(&r + idx);
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator+=(const self_t &rhs) noexcept
{
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator-=(const self_t &rhs) noexcept
{
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator*=(const self_t &rhs) noexcept
{
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator/=(const self_t &rhs) noexcept
{
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator+=(T rhs) noexcept
{
    r += rhs;
    g += rhs;
    b += rhs;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator-=(T rhs) noexcept
{
    r -= rhs;
    g -= rhs;
    b -= rhs;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator*=(T rhs) noexcept
{
    r *= rhs;
    g *= rhs;
    b *= rhs;
    return *this;
}

template<typename T>
typename tcolor3<T>::self_t &tcolor3<T>::operator/=(T rhs) noexcept
{
    r /= rhs;
    g /= rhs;
    b /= rhs;
    return *this;
}

template<typename T>
tcolor3<T> operator-(const tcolor3<T> &color) noexcept
{
    return tcolor3<T>(-color.r, -color.g, -color.b);
}

template<typename T>
tcolor3<T> operator+(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return tcolor3<T>(lhs.r + rhs.r,
                      lhs.g + rhs.g,
                      lhs.b + rhs.b);
}

template<typename T>
tcolor3<T> operator-(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return tcolor3<T>(lhs.r - rhs.r,
                      lhs.g - rhs.g,
                      lhs.b - rhs.b);
}

template<typename T>
tcolor3<T> operator*(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return tcolor3<T>(lhs.r * rhs.r,
                      lhs.g * rhs.g,
                      lhs.b * rhs.b);
}

template<typename T>
tcolor3<T> operator/(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return tcolor3<T>(lhs.r / rhs.r,
                      lhs.g / rhs.g,
                      lhs.b / rhs.b);
}

template<typename T>
tcolor3<T> operator+(const tcolor3<T> &lhs, T rhs) noexcept
{
    return tcolor3<T>(lhs.r + rhs,
                      lhs.g + rhs,
                      lhs.b + rhs);
}

template<typename T>
tcolor3<T> operator-(const tcolor3<T> &lhs, T rhs) noexcept
{
    return tcolor3<T>(lhs.r - rhs,
                      lhs.g - rhs,
                      lhs.b - rhs);
}

template<typename T>
tcolor3<T> operator*(const tcolor3<T> &lhs, T rhs) noexcept
{
    return tcolor3<T>(lhs.r * rhs,
                      lhs.g * rhs,
                      lhs.b * rhs);
}

template<typename T>
tcolor3<T> operator/(const tcolor3<T> &lhs, T rhs) noexcept
{
    return tcolor3<T>(lhs.r / rhs,
                      lhs.g / rhs,
                      lhs.b / rhs);
}

template<typename T>
tcolor3<T> operator+(T lhs, const tcolor3<T> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T>
tcolor3<T> operator*(T lhs, const tcolor3<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
bool operator==(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

template<typename T>
bool operator!=(const tcolor3<T> &lhs, const tcolor3<T> &rhs) noexcept
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

template<typename T, typename>
color3b to_color3b(const tcolor3<T> &c) noexcept
{
    return c.clamp(0, 1).map([](T s) { return static_cast<unsigned char>(s * 255); });
}

template<typename T, typename>
tcolor3<T> from_color3b(const color3b &c) noexcept
{
    return c.map([](unsigned char s) { return s / T(255); });
}

AGZM_END
