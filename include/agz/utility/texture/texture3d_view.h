#pragma once

#include "../math/math.h"

namespace agz::texture
{

template<typename T, bool IS_CONST>
class texture3d_view_t
{
public:

    using data_t        = math::tensor_view_t<T, 3, IS_CONST>;
    using texel_t       = typename data_t::elem_t;
    using const_texel_t = std::add_const_t<std::remove_const_t<texel_t>>;
    using self_t        = texture3d_view_t<T, IS_CONST>;

    texture3d_view_t() = default;

    texture3d_view_t(const self_t &rhs) = default;
    self_t &operator=(const self_t &rhs) = default;

    template<typename U, bool IS_CONST2,
             typename = std::enable_if_t<
                std::is_same_v<T, U> && IS_CONST && !IS_CONST2>>
    texture3d_view_t(const texture3d_view_t<U, IS_CONST2> &rhs) noexcept;

    explicit texture3d_view_t(data_t data);

    void swap(self_t &swap_with) noexcept;

    int         width()  const noexcept;
    int         height() const noexcept;
    int         depth()  const noexcept;
    math::vec3i size()   const noexcept;

          texel_t &operator()(int z, int y, int x)       noexcept;
    const_texel_t &operator()(int z, int y, int x) const noexcept;

          texel_t &at(int z, int y, int x)       noexcept;
    const_texel_t &at(int z, int y, int x) const noexcept;

          data_t &get_data()       noexcept;
    const data_t &get_data() const noexcept;

    self_t subtex(
        const math::vec3i &beg, const math::vec3i &end) noexcept;

    texture3d_view_t<T, true> subtex(
        const math::vec3i &beg, const math::vec3i &end) const noexcept;

    texture3d_view_t<T, true> subtex_const(
        const math::vec3i &beg, const math::vec3i &end) const noexcept;

private:

    data_t data_;
};

} // namespace agz::texture

#include "./impl/texture3d_view.inl"
