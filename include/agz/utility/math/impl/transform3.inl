#pragma once

namespace agz::math {

template<typename T, bool C>
ttransform3<T, C>::ttransform3() noexcept
    : mat_(mat_t::identity()), inv_(mat_t::identity())
{
    
}

template<typename T, bool C>
ttransform3<T, C>::ttransform3(const mat_t &m) noexcept
    : ttransform3(m, m.inv())
{
    
}

template<typename T, bool C>
ttransform3<T, C>::ttransform3(const mat_t &m, const mat_t &inv) noexcept
    : mat_(m), inv_(inv)
{
    
}

template<typename T, bool C>
ttransform3<T, C>::ttransform3(uninitialized_t) noexcept
    : mat_(UNINIT), inv_(UNINIT)
{
    
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::translate(const tvec3<T> &offset) noexcept
{
    return self_t(mat_t::left_transform::translate(offset), mat_t::left_transform::translate(-offset));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::translate(T x, T y, T z) noexcept
{
    return self_t(mat_t::left_transform::translate(x, y, z), mat_t::left_transform::translate(-x, -y, -z));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::rotate(const tvec3<T> &axis, T rad) noexcept
{
    return self_t(mat_t::left_transform::rotate(axis, rad), mat_t::left_transform::rotate(axis, -rad));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::rotate_x(T rad) noexcept
{
    return self_t(mat_t::left_transform::rotate_x(rad), mat_t::left_transform::rotate_x(-rad));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::rotate_y(T rad) noexcept
{
    return self_t(mat_t::left_transform::rotate_y(rad), mat_t::left_transform::rotate_y(-rad));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::rotate_z(T rad) noexcept
{
    return self_t(mat_t::left_transform::rotate_z(rad), mat_t::left_transform::rotate_z(-rad));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::scale(T x, T y, T z) noexcept
{
    return self_t(mat_t::left_transform::scale(x, y, z), mat_t::left_transform::scale(1 / x, 1 / y, 1 / z));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::scale(const tvec3<T> &ratio) noexcept
{
    return scale(ratio.x, ratio.y, ratio.z);
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::operator*=(const self_t &rhs) noexcept
{
    mat_ *= rhs.mat_;
    inv_ = rhs.inv_ * inv_;
    return *this;
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_to_point(
    const tvec3<T> &point) const noexcept
{
    const auto p4 = mat_ * tvec4<T>(point.x, point.y, point.z, 1);
    return tvec3<T>(p4.x, p4.y, p4.z) / p4.w;
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_to_vector(
    const tvec3<T> &vector) const noexcept
{
    const auto v4 = mat_ * tvec4<T>(vector.x, vector.y, vector.z, 0);
    return tvec3<T>(v4.x, v4.y, v4.z);
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_to_normal(
    const tvec3<T> &normal) const noexcept
{
    const auto v4 = inv_.t() * tvec4<T>(normal.x, normal.y, normal.z, 0);
    return tvec3<T>(v4.x, v4.y, v4.z).normalize();
}

template<typename T, bool COLUMN_MAJOR>
tcoord3<T> ttransform3<T, COLUMN_MAJOR>::apply_to_coord(
    const tcoord3<T> &coord) const noexcept
{
    return tcoord3<T>(apply_to_vector(coord.x),
                      apply_to_vector(coord.y),
                      apply_to_vector(coord.z));
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_inverse_to_point(
    const tvec3<T> &point) const noexcept
{
    const auto p4 = inv_ * tvec4<T>(point.x, point.y, point.z, 1);
    return tvec3<T>(p4.x, p4.y, p4.z) / p4.w;
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_inverse_to_vector(
    const tvec3<T> &vector) const noexcept
{
    const auto v4 = inv_ * tvec4<T>(vector.x, vector.y, vector.z, 0);
    return tvec3<T>(v4.x, v4.y, v4.z);
}

template<typename T, bool COLUMN_MAJOR>
tvec3<T> ttransform3<T, COLUMN_MAJOR>::apply_inverse_to_normal(
    const tvec3<T> &normal) const noexcept
{
    const auto v4 = (mat_.t() * tvec4<T>(normal.x, normal.y, normal.z, 0)).normalize();
    return tvec3<T>(v4.x, v4.y, v4.z).normalize();
}

template<typename T, bool COLUMN_MAJOR>
tcoord3<T> ttransform3<T, COLUMN_MAJOR>::apply_inverse_to_coord(
    const tcoord3<T> &coord) const noexcept
{
    return tcoord3<T>(apply_inverse_to_vector(coord.x),
                      apply_inverse_to_vector(coord.y),
                      apply_inverse_to_vector(coord.z));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::inv() const noexcept
{
    return self_t(inv_, mat_);
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform3<T, COLUMN_MAJOR>::self_t
    ttransform3<T, COLUMN_MAJOR>::inverse() const noexcept
{
    return inv();
}

template<typename T, bool COLUMN_MAJOR>
const typename ttransform3<T, COLUMN_MAJOR>::mat_t&
ttransform3<T, COLUMN_MAJOR>::get_mat() const noexcept
{
    return mat_;
}

template<typename T, bool COLUMN_MAJOR>
const typename ttransform3<T, COLUMN_MAJOR>::mat_t&
ttransform3<T, COLUMN_MAJOR>::get_inv_mat() const noexcept
{
    return inv_;
}

template<typename T, bool C>
ttransform3<T, C> operator*(
    const ttransform3<T, C> &lhs, const ttransform3<T, C> &rhs) noexcept
{
    return ttransform3<T>(
        lhs.get_mat() * rhs.get_mat(), rhs.get_inv_mat() * lhs.get_inv_mat());
}

} // namespace agz::math
