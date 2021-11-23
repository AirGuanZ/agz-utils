#pragma once

AGZ_MATH_BEGIN

template<typename T>
taabb3<T>::taabb3() noexcept
{
    
}

template<typename T>
taabb3<T>::taabb3(uninitialized_t) noexcept
    : low(UNINIT), high(UNINIT)
{
    
}

template<typename T>
taabb3<T>::taabb3(
    const tvec3<T> &low, const tvec3<T> &high) noexcept
    : low(low), high(high)
{
    
}

template<typename T>
bool taabb3<T>::is_valid() const noexcept
{
    return low.x <= high.x && low.y <= high.y && low.z <= high.z;
}

template<typename T>
bool taabb3<T>::contains(const tvec3<T> &p) const noexcept
{
    return low.x <= p.x && p.x <= high.x &&
           low.y <= p.y && p.y <= high.y &&
           low.z <= p.z && p.z <= high.z;
}

template<typename T>
typename taabb3<T>::self_t taabb3<T>::operator|(
    const tvec3<T> &rhs) const noexcept
{
    return self_t(tvec3<T>((std::min)(low.x, rhs.x),
                           (std::min)(low.y, rhs.y),
                           (std::min)(low.z, rhs.z)),
                  tvec3<T>((std::max)(high.x, rhs.x),
                           (std::max)(high.y, rhs.y),
                           (std::max)(high.z, rhs.z)));
}

template<typename T>
typename taabb3<T>::self_t &taabb3<T>::operator|=(
    const tvec3<T> &rhs) noexcept
{
    return *this = *this | rhs;
}

template<typename T>
typename taabb3<T>::self_t taabb3<T>::operator|(
    const self_t &rhs) const noexcept
{
    return self_t(tvec3<T>((std::min)(low.x, rhs.low.x),
                           (std::min)(low.y, rhs.low.y),
                           (std::min)(low.z, rhs.low.z)),
                  tvec3<T>((std::max)(high.x, rhs.high.x),
                           (std::max)(high.y, rhs.high.y),
                           (std::max)(high.z, rhs.high.z)));
}

template<typename T>
typename taabb3<T>::self_t &taabb3<T>::operator|=(
    const self_t &rhs) noexcept
{
    return *this = *this | rhs;
}

template<typename T>
typename taabb3<T>::self_t taabb3<T>::operator*(
    T rhs) const noexcept
{
    return self_t(low * rhs, high * rhs);
}

template<typename T>
typename taabb3<T>::self_t &taabb3<T>::operator*=(
    T rhs) noexcept
{
    return *this = *this * rhs;
}

template<typename T>
taabb3<T> operator|(
    const tvec3<T> &lhs, const taabb3<T> &rhs) noexcept
{
    return rhs | lhs;
}

template<typename T>
taabb3<T> operator*(T lhs, const taabb3<T> &rhs) noexcept
{
    return rhs * lhs;
}

AGZ_MATH_END
