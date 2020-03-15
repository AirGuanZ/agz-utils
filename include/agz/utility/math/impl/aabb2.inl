#pragma once

namespace agz::math
{

template<typename T>
taabb2<T>::taabb2() noexcept
{
    
}

template<typename T>
taabb2<T>::taabb2(uninitialized_t) noexcept
    : low(UNINIT), high(UNINIT)
{
    
}

template<typename T>
taabb2<T>::taabb2(const tvec2<T> &low, const tvec2<T> &high) noexcept
    : low(low), high(high)
{
    
}

template<typename T>
bool taabb2<T>::is_valid() const noexcept
{
    return low.x <= high.x && low.y <= high.y;
}

template<typename T>
bool taabb2<T>::contains(const tvec2<T> &p) const noexcept
{
    return low.x <= p.x && p.x <= high.x &&
           low.y <= p.y && p.y <= high.y;
}

template<typename T>
typename taabb2<T>::self_t taabb2<T>::operator|(const tvec2<T> &rhs) const noexcept
{
    return self_t(tvec2<T>((std::min)(low.x, rhs.x),
                           (std::min)(low.y, rhs.y)),
                  tvec2<T>((std::max)(high.x, rhs.x),
                           (std::max)(high.y, rhs.y)));
}

template<typename T>
typename taabb2<T>::self_t &taabb2<T>::operator|=(const tvec2<T> &rhs) noexcept
{
    return *this = *this | rhs;
}

template<typename T>
typename taabb2<T>::self_t taabb2<T>::operator|(const self_t &rhs) const noexcept
{
    return self_t(tvec2<T>((std::min)(low.x, rhs.low.x),
                           (std::min)(low.y, rhs.low.y)),
                  tvec2<T>((std::max)(high.x, rhs.high.x),
                           (std::max)(high.y, rhs.high.y)));
}

template<typename T>
typename taabb2<T>::self_t &taabb2<T>::operator|=(const self_t &rhs) noexcept
{
    return *this = *this | rhs;
}

template<typename T>
typename taabb2<T>::self_t taabb2<T>::operator*(T rhs) const noexcept
{
    return self_t(low * rhs, high * rhs);
}

template<typename T>
typename taabb2<T>::self_t &taabb2<T>::operator*=(T rhs) noexcept
{
    return *this = *this * rhs;
}

template<typename T>
taabb2<T> operator|(const tvec2<T> &lhs, const taabb2<T> &rhs) noexcept
{
    return rhs | lhs;
}

template<typename T>
taabb2<T> operator*(T lhs, const taabb2<T> &rhs) noexcept
{
    return rhs * lhs;
}

} // namespace agz::math
