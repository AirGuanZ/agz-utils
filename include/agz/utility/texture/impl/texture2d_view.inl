#pragma once

namespace agz::texture
{

template<typename T, bool CONST>
template<typename U, bool CONST2, typename>
texture2d_view_t<T, CONST>::texture2d_view_t(
    const texture2d_view_t<U, CONST2> &rhs) noexcept
    : data_(rhs.get_data())
{
    
}

template<typename T, bool CONST>
texture2d_view_t<T, CONST>::texture2d_view_t(data_t data)
    : data_(data)
{
    
}

template<typename T, bool CONST>
void texture2d_view_t<T, CONST>::swap(self_t &swap_with) noexcept
{
    data_.swap(swap_with);
}

template<typename T, bool CONST>
int texture2d_view_t<T, CONST>::width() const noexcept
{
    return data_.shape()[1];
}

template<typename T, bool CONST>
int texture2d_view_t<T, CONST>::height() const noexcept
{
    return data_.shape()[0];
}

template<typename T, bool CONST>
math::vec2i texture2d_view_t<T, CONST>::size() const noexcept
{
    const auto shape = data_.shape();
    return { shape[1], shape[0] };
}

template<typename T, bool CONST>
typename texture2d_view_t<T, CONST>::texel_t &
    texture2d_view_t<T, CONST>::operator()(int y, int x) noexcept
{
    return data_(y, x);
}

template<typename T, bool CONST>
const typename texture2d_view_t<T, CONST>::texel_t &
    texture2d_view_t<T, CONST>::operator()(int y, int x) const noexcept
{
    return data_(y, x);
}

template<typename T, bool CONST>
typename texture2d_view_t<T, CONST>::texel_t &
    texture2d_view_t<T, CONST>::at(int y, int x) noexcept
{
    return data_(y, x);
}

template<typename T, bool CONST>
const typename texture2d_view_t<T, CONST>::texel_t &
    texture2d_view_t<T, CONST>::at(int y, int x) const noexcept
{
    return data_(y, x);
}

template<typename T, bool CONST>
typename texture2d_view_t<T, CONST>::data_t &
    texture2d_view_t<T, CONST>::get_data() noexcept
{
    return data_;
}

template<typename T, bool CONST>
const typename texture2d_view_t<T, CONST>::data_t &
    texture2d_view_t<T, CONST>::get_data() const noexcept
{
    return data_;
}

template<typename T, bool CONST>
typename texture2d_view_t<T, CONST>::self_t texture2d_view_t<T, CONST>::subtex(
    int y_beg, int y_end, int x_beg, int x_end) noexcept
{
    const int w = x_end - x_beg;
    const int h = y_end - y_beg;
    return self_t(data_.get_subview({ y_beg, x_beg }, { h, w }));
}

template<typename T, bool CONST>
texture2d_view_t<T, true> texture2d_view_t<T, CONST>::subtex(
    int y_beg, int y_end, int x_beg, int x_end) const noexcept
{
    const int w = x_end - x_beg;
    const int h = y_end - y_beg;
    return texture2d_view_t<T, true>(
        data_.get_subview({ y_beg, x_beg }, { h, w }));
}

template<typename T, bool CONST>
texture2d_view_t<T, true> texture2d_view_t<T, CONST>::subtex_const(
    int y_beg, int y_end, int x_beg, int x_end) const noexcept
{
    const int w = x_end - x_beg;
    const int h = y_end - y_beg;
    return texture2d_view_t<T, true>(
        data_.get_subview({ y_beg, x_beg }, { h, w }));
}

} // namespace agz::texture
