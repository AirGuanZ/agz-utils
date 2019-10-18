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
    T half_angle = T(0.5) * rad;
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

} // namespace agz::math