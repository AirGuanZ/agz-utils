#pragma once

#include "./vec3.h"

namespace agz::math
{

template<typename T>
class tquaternion_t
{
    static_assert(std::is_floating_point_v<T>);

public:

    using self_t = tquaternion_t<T>;

    T w, x, y, z;

    tquaternion_t() noexcept;

    tquaternion_t(T w, T x, T y, T z) noexcept;

    tquaternion_t(const tvec3<T> &axis, T rad) noexcept;

    self_t normalize() const noexcept;

    self_t conjugate() const noexcept;

    tvec3<T> apply_to_vector(const tvec3<T> &rhs) const noexcept;

    self_t operator*(const self_t &rhs) const noexcept;
};

template<typename T>
tquaternion_t<T> slerp(const tquaternion_t<T> &lhs, const tquaternion_t<T> &rhs, T interp_factor);

} // namespace agz::math
