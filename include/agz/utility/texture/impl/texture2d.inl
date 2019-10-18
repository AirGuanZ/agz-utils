#pragma once

namespace agz::texture
{

template<typename T>
texture2d_t<T>::texture2d_t(int h, int w, const texel_t *data)
    : texture2d_t({ h, w }, data)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(const math::vec2i &size, const texel_t *data)
    : data_({ size[0], size[1] }, data)
{

}

template<typename T>
texture2d_t<T>::texture2d_t(int h, int w, uninitialized_t)
    : texture2d_t({ h, w }, UNINIT)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(const math::vec2i &size, uninitialized_t)
    : data_({ size[0], size[1] }, UNINIT)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(int h, int w, const texel_t &init_texel)
    : texture2d_t({ h, w }, init_texel)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(const math::vec2i &size, const texel_t &init_texel)
    : data_({ size[0], size[1] }, init_texel)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(const data_t &data)
    : data_(data)
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(data_t &&data) noexcept
    : data_(std::move(data))
{
    
}

template<typename T>
texture2d_t<T>::texture2d_t(self_t &&move_from) noexcept
    : data_(std::move(move_from.data_))
{

}

template<typename T>
texture2d_t<T> &texture2d_t<T>::operator=(self_t &&move_from) noexcept
{
    data_ = std::move(move_from.data_);
    return *this;
}

template<typename T>
void texture2d_t<T>::initialize(int h, int w, uninitialized_t)
{
    self_t t(h, w, UNINIT);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::initialize(const math::vec2i &size, uninitialized_t)
{
    self_t t(size, UNINIT);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::initialize(int h, int w, const texel_t &init_texel)
{
    self_t t(h, w, init_texel);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::initialize(const math::vec2i &size, const texel_t &init_texel)
{
    self_t t(size, init_texel);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::initialize(int h, int w, const texel_t *data)
{
    self_t t(h, w, data);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::initialize(const math::vec2i &size, const texel_t *data)
{
    self_t t(size, data);
    this->swap(t);
}

template<typename T>
void texture2d_t<T>::swap(self_t &swap_with) noexcept
{
    data_.swap(swap_with.data_);
}

template<typename T>
void texture2d_t<T>::destroy()
{
    data_.destroy();
}

template<typename T>
bool texture2d_t<T>::is_available() const noexcept
{
    return data_.is_available();
}

template<typename T>
int texture2d_t<T>::width() const noexcept
{
    return data_.shape()[1];
}

template<typename T>
int texture2d_t<T>::height() const noexcept
{
    return data_.shape()[0];
}

template<typename T>
math::vec2i texture2d_t<T>::size() const noexcept
{
    return { data_.shape()[1], data_.shape()[0] };
}

template<typename T>
typename texture2d_t<T>::texel_t& texture2d_t<T>::operator()(int y, int x) noexcept
{
    return data_(y, x);
}

template<typename T>
const typename texture2d_t<T>::texel_t& texture2d_t<T>::operator()(int y, int x) const noexcept
{
    return data_(y, x);
}

template<typename T>
typename texture2d_t<T>::texel_t& texture2d_t<T>::at(int y, int x) noexcept
{
    return data_(y, x);
}

template<typename T>
const typename texture2d_t<T>::texel_t& texture2d_t<T>::at(int y, int x) const noexcept
{
    return data_(y, x);
}

template<typename T>
typename texture2d_t<T>::data_t &texture2d_t<T>::get_data() noexcept
{
    return data_;
}

template<typename T>
const typename texture2d_t<T>::data_t &texture2d_t<T>::get_data() const noexcept
{
    return data_;
}

template<typename T>
template<typename Func>
auto texture2d_t<T>::map(Func &&func) const
{
    using ret_pixel_t = rm_rcv_t<decltype(func(data_.at({ 0, 0 }))) > ;
    return texture2d_t<ret_pixel_t>(data_.map(std::forward<Func>(func)));
}

template<typename T>
T *texture2d_t<T>::raw_data() noexcept
{
    return is_available() ? data_.raw_data() : nullptr;
}

template<typename T>
const T *texture2d_t<T>::raw_data() const noexcept
{
    return is_available() ? data_.raw_data() : nullptr;
}

template<typename T>
void texture2d_t<T>::clear(const T &value)
{
    if(!is_available())
        return;
    for(int i = 0; i < data_.elem_count(); ++i)
        data_.raw_data()[i] = value;
}

template<typename T>
typename texture2d_t<T>::self_t texture2d_t<T>::subtex(int y_beg, int y_end, int x_beg, int x_end) const
{
    assert(is_available());
    assert(0 <= y_beg && y_beg < y_end && y_end < height());
    assert(0 <= x_beg && x_beg < x_end && x_end < width());
    int y_size = y_end - y_beg, x_size = x_end - x_beg;
    self_t ret(y_size, x_size);
    for(int ly = 0, y = y_beg; ly < y_size; ++ly, ++y)
    {
        for(int lx = 0, x = x_beg; lx < x_size; ++lx, ++x)
            ret(ly, lx) = at(y, x);
    }
    return ret;
}

} // namespace agz::texture