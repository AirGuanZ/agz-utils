#pragma once

#include "../math/math.h"
#include "texture2d_view.h"

namespace agz::texture
{

/**
 * @brief 二维纹理对象
 */
template<typename T>
class texture2d_t
{
public:

    using data_t  = math::tensor_t<T, 2>;
    using texel_t = T;
    using self_t  = texture2d_t<T>;

    texture2d_t() = default;
    texture2d_t(int h, int w, uninitialized_t);
    texture2d_t(int h, int w, const texel_t *data);
    texture2d_t(int h, int w, const texel_t &init_texel = texel_t());

    explicit texture2d_t(const data_t &data);
    explicit texture2d_t(data_t &&data) noexcept;

    texture2d_t(const self_t&)               = default;
    texture2d_t<T> &operator=(const self_t&) = default;

    texture2d_t(self_t &&move_from)               noexcept;
    texture2d_t<T> &operator=(self_t &&move_from) noexcept;

    ~texture2d_t() = default;

    void initialize(int h, int w, uninitialized_t);
    void initialize(int h, int w, const texel_t *data);
    void initialize(int h, int w, const texel_t &init_texel = texel_t());
    
    void swap(self_t &swap_with) noexcept;

    void destroy();

    bool is_available() const noexcept;

    int width()        const noexcept;
    int height()       const noexcept;
    math::vec2i size() const noexcept;

          texel_t &operator()(int y, int x)       noexcept;
    const texel_t &operator()(int y, int x) const noexcept;

          texel_t &at(int y, int x)       noexcept;
    const texel_t &at(int y, int x) const noexcept;

          data_t &get_data()       noexcept;
    const data_t &get_data() const noexcept;

    template<typename Func>
    auto map(Func &&func) const;

    template<typename Func>
    void map_inplace(Func &&func) const;

    const T *raw_data() const noexcept;
          T *raw_data()       noexcept;

    void clear(const T &value);

    self_t subtex(int y_beg, int y_end, int x_beg, int x_end) const;

    texture2d_view_t<T, false> subview(
        int y_beg, int y_end, int x_beg, int x_end) noexcept;

    texture2d_view_t<T, true> subview(
        int y_beg, int y_end, int x_beg, int x_end) const noexcept;

    texture2d_view_t<T, true> subview_const(
        int y_beg, int y_end, int x_beg, int x_end) const noexcept;

    self_t flip_vertically() const;

    self_t flip_horizontally() const;

    template<typename S>
    auto operator+(const texture2d_t<S> &rhs) const;

    template<typename S>
    auto operator*(const texture2d_t<S> &rhs) const;

    template<typename S>
    texture2d_t<T> &operator+=(const texture2d_t<S> &rhs);

    template<typename S, typename = std::enable_if_t<std::is_arithmetic_v<S>>>
    auto operator*(S rhs) const;

protected:

    data_t data_;
};

template<typename T, typename S,
         typename = std::enable_if_t<std::is_arithmetic_v<S>>>
auto operator*(S lhs, const texture2d_t<T> &rhs);

} // namespace agz::texture

#include "impl/texture2d.inl"
