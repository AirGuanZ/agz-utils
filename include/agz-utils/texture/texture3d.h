#pragma once

#include "../math/math.h"
#include "texture3d_view.h"

namespace agz::texture
{

template<typename T>
class texture3d_t
{
public:

    using data_t  = math::tensor_t<T, 3>;
    using texel_t = T;
    using self_t  = texture3d_t<T>;

    texture3d_t() = default;
    texture3d_t(int d, int h, int w, uninitialized_t);
    texture3d_t(int d, int h, int w, const texel_t *data);
    texture3d_t(int d, int h, int w, const texel_t &init_texel = texel_t());

    explicit texture3d_t(const data_t &data);
    explicit texture3d_t(data_t &&data) noexcept;

    texture3d_t(const self_t &)               = default;
    texture3d_t<T> &operator=(const self_t &) = default;

    texture3d_t(self_t &&)               noexcept = default;
    texture3d_t<T> &operator=(self_t &&) noexcept = default;

    ~texture3d_t() = default;
    
    void initialize(int d, int h, int w, uninitialized_t);
    void initialize(int d, int h, int w, const texel_t *data);
    void initialize(int d, int h, int w, const texel_t &init_texel = texel_t());
    
    void swap(self_t &swap_with) noexcept;

    void destroy();

    bool is_available() const noexcept;

    int width()  const noexcept;
    int height() const noexcept;
    int depth()  const noexcept;
    math::vec3i size() const noexcept;

          texel_t &operator()(int z, int y, int x)       noexcept;
    const texel_t &operator()(int z, int y, int x) const noexcept;

          texel_t &at(int z, int y, int x)       noexcept;
    const texel_t &at(int z, int y, int x) const noexcept;

          data_t &get_data()       noexcept;
    const data_t &get_data() const noexcept;

    template<typename Func>
    auto map(Func &&func) const;

    const T *raw_data() const noexcept;
          T *raw_data()       noexcept;

    void clear(const T &value);
    
    self_t subtex(
        int z_beg, int z_end, int y_beg, int y_end, int x_beg, int x_end) const;

    texture3d_view_t<T, false> subview(
        const math::vec3i &beg, const math::vec3i &end) noexcept;

    texture3d_view_t<T, true> subview(
        const math::vec3i &beg, const math::vec3i &end) const noexcept;

    texture3d_view_t<T, true> subview_const(
        const math::vec3i &beg, const math::vec3i &end) const noexcept;

protected:

    data_t data_;
};

} // namespace agz::texture

#include "./impl/texture3d.inl"
