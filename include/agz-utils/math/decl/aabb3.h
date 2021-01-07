#pragma once

#include "./vec3.h"

AGZ_MATH_BEGIN

template<typename T>
class taabb3
{
public:

    using self_t = taabb3<T>;

    tvec3<T> low;
    tvec3<T> high;

    AGZ_MATH_API taabb3() noexcept;
    AGZ_MATH_API taabb3(uninitialized_t) noexcept;
    AGZ_MATH_API taabb3(const tvec3<T> &low, const tvec3<T> &high) noexcept;

    AGZ_MATH_API bool is_valid() const noexcept;

    AGZ_MATH_API bool contains(const tvec3<T> &p) const noexcept;

    AGZ_MATH_API self_t operator|(const tvec3<T> &rhs) const noexcept;
    AGZ_MATH_API self_t &operator|=(const tvec3<T> &rhs) noexcept;

    AGZ_MATH_API self_t operator|(const self_t &rhs) const noexcept;
    AGZ_MATH_API self_t &operator|=(const self_t &rhs) noexcept;

    AGZ_MATH_API self_t operator*(T rhs) const noexcept;
    AGZ_MATH_API self_t &operator*=(T rhs) noexcept;
};

template<typename T>
AGZ_MATH_API taabb3<T> operator|(
    const tvec3<T> &lhs, const taabb3<T> &rhs) noexcept;

template<typename T>
AGZ_MATH_API taabb3<T> operator*(T lhs, const taabb3<T> &rhs) noexcept;

using aabb3f = taabb3<float>;
using aabb3d = taabb3<double>;
using aabb3i = taabb3<int>;

AGZ_MATH_END
