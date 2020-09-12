#pragma once

namespace agz::math
{

template<typename T, bool C>
ttransform2<T, C>::ttransform2() noexcept
    : mat_(mat_t::identity()), inv_(mat_t::identity())
{
    
}

template<typename T, bool C>
ttransform2<T, C>::ttransform2(const mat_t &m) noexcept
    : ttransform2(m, m.inv())
{
    
}

template<typename T, bool C>
ttransform2<T, C>::ttransform2(const mat_t &m, const mat_t &inv) noexcept
    : mat_(m), inv_(inv)
{
    
}

template<typename T, bool C>
ttransform2<T, C>::ttransform2(uninitialized_t) noexcept
    : mat_(UNINIT), inv_(UNINIT)
{
    
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::translate(const tvec2<T> &offset) noexcept
{
    return self_t(mat_t::translate(offset), mat_t::translate(-offset));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::translate(T x, T y) noexcept
{
    return self_t(mat_t::translate(x, y), mat_t::translate(-x, -y));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::rotate(T rad) noexcept
{
    return self_t(mat_t::rotate_z(rad), mat_t::rotate_z(-rad));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::scale(T x, T y) noexcept
{
    return self_t(mat_t::scale(x, y), mat_t::scale(1 / x, 1 / y));
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::scale(const tvec2<T> &ratio) noexcept
{
    return scale(ratio.x, ratio.y);
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::operator*=(const self_t &rhs) noexcept
{
    mat_ *= rhs.mat_;
    inv_ = rhs.inv_ * inv_;
    return *this;
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_to_point(const tvec2<T> &point) const noexcept
{
    auto p3 = mat_ * tvec3<T>(point.x, point.y, 1);
    return tvec2<T>(p3.x, p3.y) / p3.z;
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_to_vector(
    const tvec2<T> &vector) const noexcept
{
    const auto v3 = mat_ * tvec3<T>(vector.x, vector.y, 0);
    return tvec2<T>(v3.x, v3.y);
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_to_normal(
    const tvec2<T> &normal) const noexcept
{
    const auto v4 = (inv_.t() * tvec3<T>(normal.x, normal.y, 0)).normalize();
    return tvec2<T>(v4.x, v4.y).normalize();
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_inverse_to_point(
    const tvec2<T> &point) const noexcept
{
    const auto p3 = inv_ * tvec3<T>(point.x, point.y, 1);
    return tvec2<T>(p3.x, p3.y) / p3.z;
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_inverse_to_vector(
    const tvec2<T> &vector) const noexcept
{
    const auto v3 = inv_ * tvec3<T>(vector.x, vector.y, 0);
    return tvec2<T>(v3.x, v3.y);
}

template<typename T, bool COLUMN_MAJOR>
tvec2<T> ttransform2<T, COLUMN_MAJOR>::apply_inverse_to_normal(
    const tvec2<T> &normal) const noexcept
{
    const auto v3 = (mat_.t() * tvec3<T>(normal.x, normal.y, 0)).normalize();
    return tvec3<T>(v3.x, v3.y).normalize();
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::inv() const noexcept
{
    return self_t(inv_, mat_);
}

template<typename T, bool COLUMN_MAJOR>
typename ttransform2<T, COLUMN_MAJOR>::self_t
    ttransform2<T, COLUMN_MAJOR>::inverse() const noexcept
{
    return inv();
}

template<typename T, bool COLUMN_MAJOR>
const typename ttransform2<T, COLUMN_MAJOR>::mat_t&
ttransform2<T, COLUMN_MAJOR>::get_mat() const noexcept
{
    return mat_;
}

template<typename T, bool COLUMN_MAJOR>
const typename ttransform2<T, COLUMN_MAJOR>::mat_t&
ttransform2<T, COLUMN_MAJOR>::get_inv_mat() const noexcept
{
    return inv_;
}

template<typename T, bool COLUMN_MAJOR>
bool ttransform2<T, COLUMN_MAJOR>::operator==(const self_t &rhs) const noexcept
{
    return mat_ == rhs.mat_;
}

template<typename T, bool COLUMN_MAJOR>
bool ttransform2<T, COLUMN_MAJOR>::operator!=(const self_t &rhs) const noexcept
{
    return mat_ != rhs.mat_;
}

template<typename T, bool C>
ttransform2<T, C> operator*(
    const ttransform2<T, C> &lhs, const ttransform2<T, C> &rhs) noexcept
{
    return ttransform2<T>(
        lhs.get_mat() * rhs.get_mat(), rhs.get_inv_mat() * lhs.get_inv_mat());
}

} // namespace agz::math
