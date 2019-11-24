#pragma once

#include <cmath>

namespace agz::math {

template<typename T, int D>
tvec<T, D>::tvec() noexcept
    : tvec(T(0))
{

}

template<typename T, int D>
tvec<T, D>::tvec(T val) noexcept
    : tvec(UNINIT)
{
    for(int i = 0; i != D; ++i)
        data[i] = val;
}

template<typename T, int D>
tvec<T, D>::tvec(uninitialized_t) noexcept
{

}

template<typename T, int D>
template<typename...As, typename>
tvec<T, D>::tvec(As&&...args) noexcept
    : data{ T(std::forward<As>(args))... }
{

}

template<typename T, int D>
bool tvec<T, D>::is_zero() const noexcept
{
    for(auto c : data)
    {
        if(c)
            return false;
    }
    return true;
}

template<typename T, int D>
auto tvec<T, D>::length() const noexcept
{
    return std::sqrt(length_square());
}

template<typename T, int D>
auto tvec<T, D>::length_square() const noexcept
{
    T ret = T(0);
    for(auto c : data)
        ret += c * c;
    return ret;
}

template<typename T, int D>
typename tvec<T, D>::self_t tvec<T, D>::normalize() const noexcept
{
    return *this / length();
}

template<typename T, int D>
typename tvec<T, D>::self_t tvec<T, D>::clamp(T min_v, T max_v) const noexcept
{
    self_t ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = agz::math::clamp(data[i], min_v, max_v);
    return ret;
}

template<typename T, int D>
typename tvec<T, D>::self_t tvec<T, D>::clamp_low(T min_v) const noexcept
{
    self_t ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = std::max(data[i], min_v);
    return ret;
}

template<typename T, int D>
typename tvec<T, D>::self_t tvec<T, D>::clamp_high(T max_v) const noexcept
{
    self_t ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = std::min(data[i], max_v);
    return ret;
}

template<typename T, int D>
auto tvec<T, D>::sum() const noexcept
{
    T ret = T(0);
    for(auto c : data)
        ret += c;
    return ret;
}

template<typename T, int D>
auto tvec<T, D>::product() const noexcept
{
    T ret = T(1);
    for(auto c : data)
        ret *= c;
    return ret;
}

template<typename T, int D>
template<typename F>
auto tvec<T, D>::map(F &&f) const noexcept
{
    using ret_t = tvec<rm_rcv_t<decltype(f(data[0]))>, D>;
    ret_t ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = f(data[0]);
    return ret;
}

template<typename T, int D>
bool tvec<T, D>::operator!() const noexcept
{
    return is_zero();
}

template<typename T, int D>
T &tvec<T, D>::operator[](size_t idx) noexcept
{
    return data[idx];
}

template<typename T, int D>
const T &tvec<T, D>::operator[](size_t idx) const noexcept
{
    return data[idx];
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator+=(const self_t &rhs) noexcept
{
    return *this = *this + rhs;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator-=(const self_t &rhs) noexcept
{
    return *this = *this - rhs;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator*=(const self_t &rhs) noexcept
{
    return *this = *this * rhs;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator/=(const self_t &rhs) noexcept
{
    return *this = *this / rhs;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator+=(T rhs) noexcept
{
    for(auto &c : data)
        c += rhs;
    return *this;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator-=(T rhs) noexcept
{
    for(auto &c : data)
        c -= rhs;
    return *this;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator*=(T rhs) noexcept
{
    for(auto &c : data)
        c *= rhs;
    return *this;
}

template<typename T, int D>
typename tvec<T, D>::self_t &tvec<T, D>::operator/=(T rhs) noexcept
{
    for(auto &c : data)
        c /= rhs;
    return *this;
}

template<typename T, int D>
tvec<T, D> operator-(const tvec<T, D> &vec) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = -vec[i];
    return ret;
}

template<typename T, int D>
tvec<T, D> operator+(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] + rhs[i];
    return ret;
}

template<typename T, int D>
tvec<T, D> operator-(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] - rhs[i];
    return ret;
}

template<typename T, int D>
tvec<T, D> operator*(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] * rhs[i];
    return ret;
}

template<typename T, int D>
tvec<T, D> operator/(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] / rhs[i];
    return ret;
}

template<typename T, int D>
tvec<T, D> operator+(const tvec<T, D> &lhs, T rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] + rhs;
    return ret;
}

template<typename T, int D>
tvec<T, D> operator-(const tvec<T, D> &lhs, T rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] - rhs;
    return ret;
}

template<typename T, int D>
tvec<T, D> operator*(const tvec<T, D> &lhs, T rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] * rhs;
    return ret;
}

template<typename T, int D>
tvec<T, D> operator/(const tvec<T, D> &lhs, T rhs) noexcept
{
    tvec<T, D> ret(UNINIT);
    for(int i = 0; i != D; ++i)
        ret[i] = lhs[i] / rhs;
    return ret;
}

template<typename T, int D>
tvec<T, D> operator+(T lhs, const tvec<T, D> &rhs) noexcept
{
    return rhs + lhs;
}

template<typename T, int D>
tvec<T, D> operator*(T lhs, const tvec<T, D> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T, int D>
bool operator==(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    for(int i = 0; i != D; ++i)
    {
        if(lhs[i] != rhs[i])
            return false;
    }
    return true;
}

template<typename T, int D>
bool operator!=(const tvec<T, D>& lhs, const tvec<T, D>& rhs) noexcept
{
    for(int i = 0; i != D; ++i)
    {
        if(lhs[i] != rhs[i])
            return true;
    }
    return false;
}

template<typename T, int D>
auto dot(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    T ret = T(0);
    for(int i = 0; i != D; ++i)
        ret += lhs[i] * rhs[i];
    return ret;
}

template<typename T, int D>
auto cos(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept
{
    return dot(lhs, rhs) / (lhs.length() * rhs.length());
}

template<typename T, int D>
std::ostream &operator<<(std::ostream &out, const tvec<T, D> &vec)
{
    out << "(" << vec[0];
    for(int i = 1; i < D; ++i)
        out << ", " << vec[i];
    out << ")";
    return out;
}

} // namespace agz::math
