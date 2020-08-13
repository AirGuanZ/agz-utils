#pragma once

#include "../math/math.h"

namespace agz::texture
{

template<typename T, bool IS_CONST>
class texture2d_view_t
{
public:

    using data_t  = math::tensor_view_t<T, 2, IS_CONST>;
    using texel_t = typename data_t::elem_t;
    using self_t  = texture2d_view_t<T, IS_CONST>;

    texture2d_view_t() = default;

    template<typename U, bool CONST2,
             typename = std::enable_if_t<
                std::is_same_v<T, U> &&IS_CONST && !CONST2>>
    texture2d_view_t(const texture2d_view_t<U, CONST2> &rhs) noexcept;

    explicit texture2d_view_t(data_t data);

    void swap(self_t &swap_with) noexcept;

    int width()        const noexcept;
    int height()       const noexcept;
    math::vec2i size() const noexcept;

    texel_t       &operator()(int y, int x)       noexcept;
    const texel_t &operator()(int y, int x) const noexcept;
    
    texel_t       &at(int y, int x)       noexcept;
    const texel_t &at(int y, int x) const noexcept;

    data_t       &get_data()       noexcept;
    const data_t &get_data() const noexcept;

    self_t                    subtex(
        int y_beg, int y_end, int x_beg, int x_end) noexcept;
    texture2d_view_t<T, true> subtex(
        int y_beg, int y_end, int x_beg, int x_end) const noexcept;
    texture2d_view_t<T, true> subtex_const(
        int y_beg, int y_end, int x_beg, int x_end) const noexcept;

private:

    data_t data_;
};

} // namespace agz::texture

#include "impl/texture2d_view.inl"
