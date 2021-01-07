#pragma once

#include "common.h"
#include "coord.h"
#include "mat4_c.h"
#include "vec3.h"

AGZ_MATH_BEGIN

/**
 * @brief 三维空间中的仿射变换，由一个四阶变换矩阵与它的逆矩阵构成
 */
template<typename T, bool COLUMN_MAJOR = true>
class ttransform3
{
    // TODO: row-major mat4
    using mat_t = tmat4_c<T>;

    mat_t mat_;
    mat_t inv_;

public:

    using self_t = ttransform3<T, COLUMN_MAJOR>;

    AGZ_MATH_API ttransform3()                                 noexcept;
    AGZ_MATH_API ttransform3(const mat_t &m, const mat_t &inv) noexcept;
    AGZ_MATH_API explicit ttransform3(const mat_t &m)          noexcept;
    AGZ_MATH_API explicit ttransform3(uninitialized_t)         noexcept;

    AGZ_MATH_API static self_t translate(const tvec3<T> &offset) noexcept;
    AGZ_MATH_API static self_t translate(T x, T y, T z)          noexcept;

    AGZ_MATH_API static self_t rotate(const tvec3<T> &axis, T rad) noexcept;
    AGZ_MATH_API static self_t rotate_x(T rad)                     noexcept;
    AGZ_MATH_API static self_t rotate_y(T rad)                     noexcept;
    AGZ_MATH_API static self_t rotate_z(T rad)                     noexcept;

    AGZ_MATH_API static self_t scale(const tvec3<T> &ratio) noexcept;
    AGZ_MATH_API static self_t scale(T x, T y, T z)         noexcept;

    AGZ_MATH_API self_t operator*=(const self_t &rhs) noexcept;

    AGZ_MATH_API tvec3<T>   apply_to_point (const tvec3<T> &point)   const noexcept;
    AGZ_MATH_API tvec3<T>   apply_to_vector(const tvec3<T> &vector)  const noexcept;
    AGZ_MATH_API tvec3<T>   apply_to_normal(const tvec3<T> &normal)  const noexcept;
    AGZ_MATH_API tcoord3<T> apply_to_coord (const tcoord3<T> &coord) const noexcept;
    
    AGZ_MATH_API tvec3<T>   apply_inverse_to_point (const tvec3<T> &point)   const noexcept;
    AGZ_MATH_API tvec3<T>   apply_inverse_to_vector(const tvec3<T> &vector)  const noexcept;
    AGZ_MATH_API tvec3<T>   apply_inverse_to_normal(const tvec3<T> &normal)  const noexcept;
    AGZ_MATH_API tcoord3<T> apply_inverse_to_coord (const tcoord3<T> &coord) const noexcept;

    AGZ_MATH_API self_t inv()     const noexcept;
    AGZ_MATH_API self_t inverse() const noexcept;

    AGZ_MATH_API const mat_t &get_mat()     const noexcept;
    AGZ_MATH_API const mat_t &get_inv_mat() const noexcept;
};

template<typename T, bool C>
AGZ_MATH_API ttransform3<T, C> operator*(
    const ttransform3<T, C> &lhs, const ttransform3<T, C> &rhs) noexcept;

using transform3f = ttransform3<float>;
using transform3d = ttransform3<double>;

AGZ_MATH_END
