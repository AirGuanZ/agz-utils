#pragma once

namespace agz::texture
{

template<typename T, bool CONST>
template<typename U, bool CONST2, typename>
texture3d_view_t<T, CONST>::texture3d_view_t(
    const texture3d_view_t<U, CONST2> &rhs) noexcept
    : data_(rhs.get_data())
{
    
}

template<typename T, bool CONST>
texture3d_view_t<T, CONST>::texture3d_view_t(data_t data)
    : data_(data)
{
    
}

template<typename T, bool CONST>
void texture3d_view_t<T, CONST>::swap(self_t &swap_with) noexcept
{
    data_.swap(swap_with.data_);
}

template<typename T, bool CONST>
int texture3d_view_t<T, CONST>::width() const noexcept
{
    return data_.shape()[2];
}

template<typename T, bool CONST>
int texture3d_view_t<T, CONST>::height() const noexcept
{
    return data_.shape()[1];
}

template<typename T, bool CONST>
int texture3d_view_t<T, CONST>::depth() const noexcept
{
    return data_.shape()[0];
}

template<typename T, bool CONST>
math::vec3i texture3d_view_t<T, CONST>::size() const noexcept
{
    const auto shape = data_.shape();
    return { shape[2], shape[1], shape[0] };
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::texel_t &
    texture3d_view_t<T, CONST>::operator()(int z, int y, int x) noexcept
{
    return data_(z, y, x);
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::const_texel_t &
    texture3d_view_t<T, CONST>::operator()(int z, int y, int x) const noexcept
{
    return data_(z, y, x);
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::texel_t &
    texture3d_view_t<T, CONST>::at(int z, int y, int x) noexcept
{
    return data_(z, y, x);
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::const_texel_t &
    texture3d_view_t<T, CONST>::at(int z, int y, int x) const noexcept
{
    return data_(z, y, x);
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::data_t &
    texture3d_view_t<T, CONST>::get_data() noexcept
{
    return data_;
}

template<typename T, bool CONST>
const typename texture3d_view_t<T, CONST>::data_t &
    texture3d_view_t<T, CONST>::get_data() const noexcept
{
    return data_;
}

template<typename T, bool CONST>
typename texture3d_view_t<T, CONST>::self_t
    texture3d_view_t<T, CONST>::subtex(
        const math::vec3i &beg, const math::vec3i &end) noexcept
{
    const auto size = end - beg;
    return self_t(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

template<typename T, bool CONST>
texture3d_view_t<T, true> texture3d_view_t<T, CONST>::subtex(
    const math::vec3i &beg, const math::vec3i &end) const noexcept
{
    const auto size = end - beg;
    return self_t(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

template<typename T, bool CONST>
texture3d_view_t<T, true> texture3d_view_t<T, CONST>::subtex_const(
    const math::vec3i &beg, const math::vec3i &end) const noexcept
{
    const auto size = end - beg;
    return self_t(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

} // namespace agz::texture
