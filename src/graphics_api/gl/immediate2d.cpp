#ifdef AGZ_ENABLE_GL

#include <agz-utils/graphics_api/gl/immediate2d.h>

AGZ_GL_BEGIN

immediate2d_t::immediate2d_t(const vec2i &framebuffer_size)
{
    set_framebuffer_size(framebuffer_size);
    init_color_shader();
}

immediate2d_t::immediate2d_t(window_t &window)
{
    set_framebuffer_size(window.get_framebuffer_size());

    window.attach(std::make_shared<window_resize_handler_t>([&]
    {
        set_framebuffer_size(window.get_framebuffer_size());
    }));

    init_color_shader();
}

void immediate2d_t::set_framebuffer_size(const vec2i &framebuffer_size)
{
    inv_fb_size_ = {
        1.0f / framebuffer_size.x,
        1.0f / framebuffer_size.y
    };
}

void immediate2d_t::draw_line(
    const vec2 &a,
    const vec2 &b,
    const vec4 &colora,
    const vec4 &colorb) const
{
    const color_vertex_t vtx_data[] = {
        { a + a - 1.0f, colora },
        { b + b - 1.0f, colorb }
    };

    vertex_buffer_t<color_vertex_t> vbo;
    vbo.initialize_data(vtx_data, 2, GL_STATIC_DRAW);

    color_vao_.bind_vertex_buffer(vbo, 0);

    color_shader_.bind();
    color_vao_.bind();

    glDrawArrays(GL_LINES, 0, 2);

    color_vao_.unbind();
    color_shader_.unbind();

    color_vao_.unbind_vertex_buffer(0);
}

void immediate2d_t::draw_line_pixel(
    const vec2 &a,
    const vec2 &b,
    const vec4 &colora,
    const vec4 &colorb) const
{
    draw_line(a * inv_fb_size_, b * inv_fb_size_, colora, colorb);
}

void immediate2d_t::draw_rect(
    const vec2 &_a,
    const vec2 &_b,
    const vec4 &color) const
{
    vec2 a = vec2((std::min)(_a.x, _b.x), (std::min)(_a.y, _b.y));
    vec2 b = vec2((std::max)(_a.x, _b.x), (std::max)(_a.y, _b.y));

    a = a + a - 1.0f;
    b = b + b - 1.0f;

    const color_vertex_t vtx_data[] = {
        { a, color }, { { a.x, b.y }, color }, { b, color },
        { a, color }, { b, color }, { { b.x, a.y }, color }
    };

    vertex_buffer_t<color_vertex_t> vbo;
    vbo.initialize_data(vtx_data, 6, GL_STATIC_DRAW);

    color_vao_.bind_vertex_buffer(vbo, 0);

    color_shader_.bind();
    color_vao_.bind();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    color_vao_.unbind();
    color_shader_.unbind();

    color_vao_.unbind_vertex_buffer(0);
}

void immediate2d_t::draw_rect_pixel(
    const vec2 &a,
    const vec2 &b,
    const vec4 &color) const
{
    draw_rect(a * inv_fb_size_, b * inv_fb_size_, color);
}

void immediate2d_t::init_color_shader()
{
    static const char *VS = R"___(
#version 450 core

in vec2 vsPosition;
in vec4 vsColor;

out vec4 fsColor;

void main()
{
    gl_Position = vec4(vsPosition, 0.5, 1);
    fsColor     = vsColor;
}
    )___";

    static const char *FS = R"___(
#version 450 core

in vec4 fsColor;

out vec4 target;

void main()
{
    target = fsColor;
}
    )___";

    color_shader_ = shader_t(
        vertex_shader_t  ::from_string(VS),
        fragment_shader_t::from_string(FS));

    const auto pos   = color_shader_.get_attribute_variable<vec2>("vsPosition");
    const auto color = color_shader_.get_attribute_variable<vec4>("vsColor");

    color_vao_.enable_attributes(pos, color);
    color_vao_.set_attribute(pos, 0, offsetof(color_vertex_t, position), false);
    color_vao_.set_attribute(color, 0, offsetof(color_vertex_t, color), false);
}

AGZ_GL_END

#endif // #ifdef AGZ_ENABLE_GL
