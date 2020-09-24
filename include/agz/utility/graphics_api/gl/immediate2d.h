#pragma once

#include <agz/utility/graphics_api/gl/shader.h>
#include <agz/utility/graphics_api/gl/vertex_array_object.h>
#include <agz/utility/graphics_api/gl/window.h>

AGZ_GL_BEGIN

class immediate2d_t
{
public:

    /**
     * screen coord: [0, 1]^2
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     *
     * pixel coord: [0, width] * [0, height]
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     */

    explicit immediate2d_t(const vec2i &framebuffer_size);

    explicit immediate2d_t(window_t &window);

    void set_framebuffer_size(const vec2i &framebuffer_size);

    void draw_line(
        const vec2 &a,
        const vec2 &b,
        const vec4 &colora,
        const vec4 &colorb) const;

    void draw_line_pixel(
        const vec2 &a,
        const vec2 &b,
        const vec4 &colora,
        const vec4 &colorb) const;

    void draw_rect(
        const vec2 &a,
        const vec2 &b,
        const vec4 &color) const;

    void draw_rect_pixel(
        const vec2 &a,
        const vec2 &b,
        const vec4 &color) const;

private:

    void init_color_shader();

    vec2 inv_fb_size_;

    struct color_vertex_t
    {
        vec2 position;
        vec4 color;
    };

    shader_t color_shader_;

    mutable vertex_array_object_t color_vao_;
};

AGZ_GL_END
