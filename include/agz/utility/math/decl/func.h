#pragma once

#include <cmath>

#include "./common.h"
#include "./vec2.h"
#include "./vec3.h"
#include "./vec4.h"
#include "./color2.h"
#include "./color3.h"
#include "./color4.h"

namespace agz::math
{
    
/**
 * @brief 线性插值
 */
template<typename T, typename F>
auto mix(const T &left, const T &right, F factor) noexcept(noexcept((1 - factor) * left + factor * right))
{
    return (1 - factor) * left + factor * right;
}

/**
 * @brief 线性插值，同mix
 */
template<typename T, typename F>
auto lerp(const T &left, const T &right, F factor) noexcept(noexcept((1 - factor) * left + factor * right))
{
    return ::agz::math::mix(left, right, factor);
}

/**
 * @brief 测试一个浮点数是否具有有限值
 */
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool is_finite(T val) noexcept
{
    return !std::isnan(val) && !std::isinf(val);
}

/**
 * @brief 平方函数
 */
template<typename T>
auto sqr(T val) noexcept
{
    return val * val;
}

template<typename T>
auto exp(const tvec2<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
auto exp(const tvec3<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
auto exp(const tvec4<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
auto exp(const tcolor2<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
auto exp(const tcolor3<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
auto exp(const tcolor4<T> &v) noexcept
{
    return v.map([](auto c) { return std::exp(c); });
}

template<typename T>
std::pair<T, T> minmax(T a, T b) noexcept
{
    return a < b ? std::pair{ a, b } : std::pair{ b, a };
}

template<typename T>
auto distance(const tvec2<T> &a, const tvec2<T> &b) noexcept
{
    return (a - b).length();
}

template<typename T>
auto distance(const tvec3<T> &a, const tvec3<T> &b) noexcept
{
    return (a - b).length();
}
template<typename T>
auto distance(const tvec4<T> &a, const tvec4<T> &b) noexcept
{
    return (a - b).length();
}

template<typename T, int D>
auto distance(const tvec<T, D> &a, const tvec<T, D> &b) noexcept
{
    return (a - b).length();
}

} // namespace agz::math
