#pragma once

#include "./vec2.h"

namespace agz::math
{

template<typename T>
class taabb2
{
public:

    using self_t = taabb2<T>;

    tvec2<T> low;
    tvec2<T> high;

    taabb2() noexcept;
    taabb2(uninitialized_t) noexcept;
    taabb2(const tvec2<T> &low, const tvec2<T> &high) noexcept;

    bool is_valid() const noexcept;

    bool contains(const tvec2<T> &p) const noexcept;

    self_t operator|(const tvec2<T> &rhs) const noexcept;
    self_t &operator|=(const tvec2<T> &rhs) noexcept;

    self_t operator|(const self_t &rhs) const noexcept;
    self_t &operator|=(const self_t &rhs) noexcept;

    self_t operator*(T rhs) const noexcept;
    self_t &operator*=(T rhs) noexcept;
};

template<typename T>
taabb2<T> operator|(const tvec2<T> &lhs, const taabb2<T> &rhs) noexcept;

template<typename T>
taabb2<T> operator*(T lhs, const taabb2<T> &rhs) noexcept;

using aabb2f = taabb2<float>;
using aabb2d = taabb2<double>;
using aabb2i = taabb2<int>;

} // namespace agz::math
