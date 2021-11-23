#pragma once

#include <atomic>
#include <cmath>

#include "./vec2.h"
#include "./vec3.h"
#include "./vec4.h"
#include "./color2.h"
#include "./color3.h"
#include "./color4.h"

AGZ_MATH_BEGIN

/**
 * @brief 将a以b为单位向上取整
 */
template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
I round_up(I a, I b) noexcept
{
    return (a + b - 1) / b * b;
}

/**
 * @brief 浮点数原子加法
 */
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
void atomic_add(std::atomic<T> &original, T add_val)
{
    T cur_val = original.load(std::memory_order_consume);
    T new_val = cur_val + add_val;
    while(!original.compare_exchange_weak(
        cur_val, new_val, std::memory_order_release, std::memory_order_consume))
    {
        new_val = cur_val + add_val;
    }
}
    
/**
 * @brief 线性插值
 */
template<typename T, typename F>
auto mix(const T &left, const T &right, F factor)
    noexcept(noexcept((1 - factor) * left + factor * right))
{
    return (1 - factor) * left + factor * right;
}

/**
 * @brief 线性插值，同mix
 */
template<typename T, typename F>
auto lerp(const T &left, const T &right, F factor)
    noexcept(noexcept((1 - factor) * left + factor * right))
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
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
auto sqr(T val) noexcept
{
    return val * val;
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
auto normalize_radian_0_2pi(T val) noexcept
{
    while(val < 0)
        val += 2 * PI<T>;
    while(val > 2 * PI<T>)
        val -= 2 * PI<T>;
    return val;
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
    return a < b ? std::pair<T, T>{ a, b } : std::pair<T, T>{ b, a };
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

template<typename T>
auto distance2(const tvec2<T> &a, const tvec2<T> &b) noexcept
{
    return (a - b).length_square();
}

template<typename T>
auto distance2(const tvec3<T> &a, const tvec3<T> &b) noexcept
{
    return (a - b).length_square();
}

template<typename T>
auto distance2(const tvec4<T> &a, const tvec4<T> &b) noexcept
{
    return (a - b).length_square();
}

template<typename T, int D>
auto distance2(const tvec<T, D> &a, const tvec<T, D> &b) noexcept
{
    return (a - b).length_square();
}

template<typename T>
const T &max3(const T &a, const T &b, const T &c)
    noexcept(noexcept((std::max)(a, b)))
{
    return (std::max)(a, (std::max)(b, c));
}

template<typename T>
const T &min3(const T &a, const T &b, const T &c)
    noexcept(noexcept((std::min)(a, b)))
{
    return (std::min)(a, (std::min)(b, c));
}

template<typename T>
tvec3<T> vec_max(const tvec3<T> &a, const tvec3<T> &b)
{
    return tvec3<T>(
        (std::max)(a.x, b.x),
        (std::max)(a.y, b.y),
        (std::max)(a.z, b.z));
}

template<typename T>
tvec3<T> vec_min(const tvec3<T> &a, const tvec3<T> &b)
{
    return tvec3<T>(
        (std::min)(a.x, b.x),
        (std::min)(a.y, b.y),
        (std::min)(a.z, b.z));
}

template<typename T>
tvec3<T> vec_max(std::initializer_list<tvec3<T>> vs)
{
    auto it = vs.begin();
    tvec3<T> result = *it++;
    while(it != vs.end())
        result = (math::vec_max)(result, *it++);
    return result;
}

template<typename T>
tvec3<T> vec_min(std::initializer_list<tvec3<T>> vs)
{
    auto it = vs.begin();
    tvec3<T> result = *it++;
    while(it != vs.end())
        result = (math::vec_min)(result, *it++);
    return result;
}

AGZ_MATH_END
