#pragma once

#include "../math/math.h"

namespace agz::texture
{

/**
 * @brief ¶þÎ¬ÎÆÀí¶ÔÏó
 */
template<typename T>
class texture2d_t
{
public:

    using data_t  = math::tensor_t<T, 2>;
    using texel_t = T;
    using self_t  = texture2d_t<T>;

             texture2d_t() = default;
             texture2d_t(int h, int w,            uninitialized_t);
             texture2d_t(const math::vec2i &size, uninitialized_t);
             texture2d_t(int h, int w,            const texel_t *data);
             texture2d_t(const math::vec2i &size, const texel_t *data);
    explicit texture2d_t(int h, int w,            const texel_t &init_texel = texel_t());
    explicit texture2d_t(const math::vec2i &size, const texel_t &init_texel = texel_t());

    texture2d_t(const self_t&)               = default;
    texture2d_t<T> &operator=(const self_t&) = default;

    explicit texture2d_t(const data_t &data);
    explicit texture2d_t(data_t &&data) noexcept;

    texture2d_t(self_t &&move_from)               noexcept;
    texture2d_t<T> &operator=(self_t &&move_from) noexcept;

    void initialize(int h, int w,            uninitialized_t);
    void initialize(const math::vec2i &size, uninitialized_t);
    void initialize(int h, int w,            const texel_t *data);
    void initialize(const math::vec2i &size, const texel_t *data);
    void initialize(int h, int w,            const texel_t &init_texel = texel_t());
    void initialize(const math::vec2i &size, const texel_t &init_texel = texel_t());

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

protected:

    data_t data_;
};

} // namespace agz::texture

#include "impl/texture2d.inl"
