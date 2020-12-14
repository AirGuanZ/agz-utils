#pragma once

#include "./vec2.h"

AGZ_MATH_BEGIN

template<typename T>
class taabb2
{
public:

    using self_t = taabb2<T>;

    tvec2<T> low;
    tvec2<T> high;

    AGZ_MATH_API taabb2() noexcept;
    AGZ_MATH_API taabb2(uninitialized_t) noexcept;
    AGZ_MATH_API taabb2(const tvec2<T> &low, const tvec2<T> &high) noexcept;

    AGZ_MATH_API bool is_valid() const noexcept;

    AGZ_MATH_API bool contains(const tvec2<T> &p) const noexcept;

    AGZ_MATH_API self_t operator|(const tvec2<T> &rhs) const noexcept;
    AGZ_MATH_API self_t &operator|=(const tvec2<T> &rhs) noexcept;

    AGZ_MATH_API self_t operator|(const self_t &rhs) const noexcept;
    AGZ_MATH_API self_t &operator|=(const self_t &rhs) noexcept;

    AGZ_MATH_API self_t operator*(T rhs) const noexcept;
    AGZ_MATH_API self_t &operator*=(T rhs) noexcept;
};

template<typename T>
AGZ_MATH_API taabb2<T> operator|(const tvec2<T> &lhs, const taabb2<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API taabb2<T> operator*(T lhs, const taabb2<T> &rhs) noexcept;

using aabb2f = taabb2<float>;
using aabb2d = taabb2<double>;
using aabb2i = taabb2<int>;

AGZ_MATH_END
