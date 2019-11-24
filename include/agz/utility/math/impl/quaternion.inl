#pragma once

#include <cmath>

namespace agz::math
{

template<typename T>
tquaternion_t<T>::tquaternion_t() noexcept
    : tquaternion_t(0, tvec3<T>(0))
{
    
}

template<typename T>
tquaternion_t<T>::tquaternion_t(T a, T b, T c, T d) noexcept
    : a(a), b(b, c, d)
{
    
}

template<typename T>
tquaternion_t<T>::tquaternion_t(T a, const tvec3<T> &b) noexcept
    : a(a), b(b)
{
    
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::construct_quaternion(const tvec3<T> &axis, T rad) noexcept
{
    T half_angle = T(-0.5) * rad;
    return self_t(std::cos(half_angle), std::sin(half_angle) * axis.normalize());
}

template<typename T>
tvec3<T> tquaternion_t<T>::apply_to_vector(const tvec3<T> &vec) noexcept
{
    return (*this * self_t(0, vec) * conjugate()).b;
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::operator*(const self_t &rhs) const noexcept
{
    T new_a = a * rhs.a - dot(b, rhs.b);
    tvec3<T> new_b = a * rhs.b + b * rhs.a + cross(b, rhs.b);
    return self_t(new_a, new_b);
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::operator*(T rhs) const noexcept
{
    return self_t(a * rhs, b * rhs);
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::operator/(T rhs) const noexcept
{
    return self_t(a / rhs, b / rhs);
}

template<typename T>
typename tquaternion_t<T>::self_t& tquaternion_t<T>::operator*=(const self_t &rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

template<typename T>
bool tquaternion_t<T>::operator==(const self_t &rhs) const noexcept
{
    return a == rhs.a && b == rhs.b;
}

template<typename T>
bool tquaternion_t<T>::operator!=(const self_t &rhs) const noexcept
{
    return !(*this == rhs);
}

template<typename T>
T tquaternion_t<T>::length_square() const noexcept
{
    return a * a + b.length_square();
}

template<typename T>
T tquaternion_t<T>::length() const noexcept
{
    return std::sqrt(this->length_square());
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::conjugate() const noexcept
{
    return self_t(a, -b);
}

template<typename T>
typename tquaternion_t<T>::self_t tquaternion_t<T>::inverse() const noexcept
{
    return this->conjugate() / this->length_square();
}

template<typename T>
tquaternion_t<T> operator*(T lhs, const tquaternion_t<T> &rhs) noexcept
{
    return rhs * lhs;
}

template<typename T>
tquaternion_t<T> slerp(const tquaternion_t<T> &a, const tquaternion_t<T> &b, T interp_factor) noexcept
{
    /*tquaternion_t<T> delta = b * a.inverse();
    T cos_half_theta = delta.a;
    T half_theta     = std::acos(cos_half_theta);
    T t_theta        = interp_factor * half_theta;
    T cos_t_theta    = std::cos(t_theta);
    T sin_t_theta    = std::sin(t_theta);

    tquaternion_t<T> t_delta = tquaternion_t<T>(cos_t_theta, sin_t_theta * delta.b.normalize());
    return t_delta * a;*/

    T cos = a.a * b.a + dot(a.b, b.b);

    auto _b = b;
    if(cos < T(0))
    {
        cos = -cos;
        _b = tquaternion_t<T>(-_b.a, -_b.b);
    }

    T linear_a, linear_b;
    T theta  = std::acos(cos);
    T sin    = std::sin(theta);
    linear_a = std::sin((T(1) - interp_factor) * theta) / sin;
    linear_b = std::sin(interp_factor          * theta) / sin;

    return tquaternion_t<T>(
        linear_a * a.a + linear_b * _b.a,
        linear_a * a.b + linear_b * _b.b);
}

} // namespace agz::math
