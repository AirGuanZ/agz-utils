#pragma once

#include "common.h"
#include "vec3.h"

namespace agz::math {

template<typename T>
class tcoord3
{
public:

    using axis_t = tvec3<T>;
    using self_t = tcoord3<T>;
    using vec_t  = tvec3<T>;

    axis_t x, y, z;

    tcoord3()                                                  noexcept;
    tcoord3(const axis_t &x, const axis_t &y, const axis_t &z) noexcept;
    explicit tcoord3(uninitialized_t)                          noexcept;

    static self_t from_x(axis_t new_x) noexcept;
    static self_t from_y(axis_t new_y) noexcept;
    static self_t from_z(axis_t new_z) noexcept;

    vec_t global_to_local(const vec_t &global_vector) const noexcept;
    vec_t local_to_global(const vec_t &local_vector)  const noexcept;

    self_t rotate_to_new_x(const axis_t &new_x) const noexcept;
    self_t rotate_to_new_y(const axis_t &new_y) const noexcept;
    self_t rotate_to_new_z(const axis_t &new_z) const noexcept;

    bool in_positive_x_hemisphere(const vec_t &v) const noexcept;
    bool in_positive_y_hemisphere(const vec_t &v) const noexcept;
    bool in_positive_z_hemisphere(const vec_t &v) const noexcept;
};

template<typename T>
tcoord3<T> operator-(const tcoord3<T> &coord) noexcept;

} // namespace agz::math
