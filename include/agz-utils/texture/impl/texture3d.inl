#pragma once

#include <cstring>

namespace agz::texture
{

template<typename T>
texture3d_t<T>::texture3d_t(int d, int h, int w, uninitialized_t)
    : data_({ d, h, w }, UNINIT)
{

}

template<typename T>
texture3d_t<T>::texture3d_t(int d, int h, int w, const texel_t *data)
    : data_({ d, h, w }, UNINIT)
{
    if constexpr(std::is_trivially_copy_constructible_v<T>)
    {
        std::memcpy(data_.raw_data(), data, sizeof(texel_t) * d * h * w);
    }
    else
    {
        int k = 0;
        for(int z = 0; z < d; ++z)
        {
            for(int y = 0; y < h; ++y)
            {
                for(int x = 0; x < w; ++x)
                    at(z, y, x) = data[k++];
            }
        }
    }
}

template<typename T>
texture3d_t<T>::texture3d_t(int d, int h, int w, const texel_t &init_texel)
    : data_({ d, h, w }, init_texel)
{
    
}

template<typename T>
texture3d_t<T>::texture3d_t(const data_t &data)
    : data_(data)
{
    
}

template<typename T>
texture3d_t<T>::texture3d_t(data_t &&data) noexcept
    : data_(std::move(data))
{
    
}

template<typename T>
void texture3d_t<T>::initialize(int d, int h, int w, uninitialized_t)
{
    self_t t(d, h, w, UNINIT);
    this->swap(t);
}

template<typename T>
void texture3d_t<T>::initialize(int d, int h, int w, const texel_t *data)
{
    self_t t(d, h, w, data);
    this->swap(t);
}

template<typename T>
void texture3d_t<T>::initialize(int d, int h, int w, const texel_t &init_texel)
{
    self_t t(d, h, w, init_texel);
    this->swap(t);
}

template<typename T>
void texture3d_t<T>::swap(self_t &swap_with) noexcept
{
    data_.swap(swap_with.data_);
}

template<typename T>
void texture3d_t<T>::destroy()
{
    data_.destroy();
}

template<typename T>
bool texture3d_t<T>::is_available() const noexcept
{
    return data_.is_available();
}

template<typename T>
int texture3d_t<T>::width() const noexcept
{
    return data_.shape()[2];
}

template<typename T>
int texture3d_t<T>::height() const noexcept
{
    return data_.shape()[1];
}

template<typename T>
int texture3d_t<T>::depth() const noexcept
{
    return data_.shape()[0];
}

template<typename T>
math::vec3i texture3d_t<T>::size() const noexcept
{
    auto shape = data_.shape();
    return { shape[2], shape[1], shape[0] };
}

template<typename T>
typename texture3d_t<T>::texel_t &texture3d_t<T>::operator()(
    int z, int y, int x) noexcept
{
    return data_(z, y, x);
}

template<typename T>
const typename texture3d_t<T>::texel_t &texture3d_t<T>::operator()(
    int z, int y, int x) const noexcept
{
    return data_(z, y, x);
}

template<typename T>
typename texture3d_t<T>::texel_t &texture3d_t<T>::at(
    int z, int y, int x) noexcept
{
    return data_(z, y, x);
}

template<typename T>
const typename texture3d_t<T>::texel_t &texture3d_t<T>::at(
    int z, int y, int x) const noexcept
{
    return data_(z, y, x);
}

template<typename T>
typename texture3d_t<T>::data_t &texture3d_t<T>::get_data() noexcept
{
    return data_;
}

template<typename T>
const typename texture3d_t<T>::data_t &texture3d_t<T>::get_data() const noexcept
{
    return data_;
}

template<typename T>
template<typename Func>
auto texture3d_t<T>::map(Func &&func) const
{
    using ret_texel_t = rm_rcv_t<decltype(func(data_.at({ 0, 0, 0 })))> ;
    return texture3d_t<ret_texel_t>(data_.map(std::forward<Func>(func)));
}

template<typename T>
T *texture3d_t<T>::raw_data() noexcept
{
    return data_.raw_data();
}

template<typename T>
const T *texture3d_t<T>::raw_data() const noexcept
{
    return data_.raw_data();
}

template<typename T>
void texture3d_t<T>::clear(const T &value)
{
    if(!is_available())
        return;
    for(int i = 0; i < data_.elem_count(); ++i)
        data_.raw_data()[i] = value;
}

template<typename T>
typename texture3d_t<T>::self_t texture3d_t<T>::subtex(
    int z_beg, int z_end, int y_beg, int y_end, int x_beg, int x_end) const
{
    assert(is_available());
    assert(0 <= z_beg && z_beg < z_end && z_end < depth());
    assert(0 <= y_beg && y_beg < y_end && y_end < height());
    assert(0 <= x_beg && x_beg < x_end && x_end < width());

    const int x_size = x_end - x_beg;
    const int y_size = y_end - y_beg;
    const int z_size = z_end - z_beg;
    self_t ret(z_size, y_size, x_size);
    for(int lz = 0, z = z_beg; lz < z_size; ++lz, ++z)
    {
        for(int ly = 0, y = y_beg; ly < y_size; ++ly, ++y)
        {
            for(int lx = 0, x = x_beg; lx < x_size; ++lx, ++x)
                ret(lz, ly, lx) = at(z, y, x);
        }
    }
    return ret;
}

template<typename T>
texture3d_view_t<T, false> texture3d_t<T>::subview(
    const math::vec3i &beg, const math::vec3i &end) noexcept
{
    const math::vec3i size = end - beg;
    return texture3d_view_t<T, false>(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

template<typename T>
texture3d_view_t<T, true> texture3d_t<T>::subview(
    const math::vec3i &beg, const math::vec3i &end) const noexcept
{
    const math::vec3i size = end - beg;
    return texture3d_view_t<T, true>(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

template<typename T>
texture3d_view_t<T, true> texture3d_t<T>::subview_const(
    const math::vec3i &beg, const math::vec3i &end) const noexcept
{
    const math::vec3i size = end - beg;
    return texture3d_view_t<T, true>(data_.get_subview(
        { beg.z, beg.y, beg.x }, { size.z, size.y, size.x }));
}

} // namespace agz::texture
