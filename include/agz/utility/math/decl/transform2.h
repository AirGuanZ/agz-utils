#pragma once

#include "./mat3_c.h"
#include "./vec2.h"

namespace agz::math
{
    
/**
 * @brief 二维空间中的仿射变换，由一个三阶变换矩阵与它的逆矩阵构成
 */
template<typename T, bool COLUMN_MAJOR = true>
class ttransform2
{
    // TODO: row-major mat3
    using mat_t = tmat3_c<T>;

    mat_t mat_;
    mat_t inv_;

public:

    using self_t = ttransform2<T, COLUMN_MAJOR>;

    ttransform2()                                 noexcept;
    ttransform2(const mat_t &m, const mat_t &inv) noexcept;
    explicit ttransform2(const mat_t &m)          noexcept;
    explicit ttransform2(uninitialized_t)         noexcept;

    static self_t translate(const tvec2<T> &offset) noexcept;
    static self_t translate(T x, T y)          noexcept;

    static self_t rotate(T rad)                     noexcept;

    static self_t scale(const tvec2<T> &ratio) noexcept;
    static self_t scale(T x, T y)         noexcept;

    self_t operator*=(const self_t &rhs) noexcept;

    tvec2<T>   apply_to_point (const tvec2<T> &point)   const noexcept;
    tvec2<T>   apply_to_vector(const tvec2<T> &vector)  const noexcept;
    tvec2<T>   apply_to_normal(const tvec2<T> &normal)  const noexcept;

    tvec2<T>   apply_inverse_to_point (const tvec2<T> &point)   const noexcept;
    tvec2<T>   apply_inverse_to_vector(const tvec2<T> &vector)  const noexcept;
    tvec2<T>   apply_inverse_to_normal(const tvec2<T> &normal)  const noexcept;

    self_t inv()     const noexcept;
    self_t inverse() const noexcept;

    const mat_t &get_mat()     const noexcept;
    const mat_t &get_inv_mat() const noexcept;

    bool operator==(const self_t &rhs) const noexcept;
    bool operator!=(const self_t &rhs) const noexcept;
};

template<typename T, bool C>
ttransform2<T, C> operator*(
    const ttransform2<T, C> &lhs, const ttransform2<T, C> &rhs) noexcept;

using transform2f = ttransform2<float>;
using transform2d = ttransform2<double>;

} // namespace agz::math
