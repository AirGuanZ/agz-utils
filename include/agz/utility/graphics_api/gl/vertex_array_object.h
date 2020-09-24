#pragma once

#include <agz/utility/graphics_api/gl/shader.h>

AGZ_GL_BEGIN

class vertex_array_object_t : public misc::uncopyable_t
{
    GLuint handle_ = 0;

public:

    vertex_array_object_t()
    {
        glCreateVertexArrays(1, &handle_);
        if(!handle_)
            throw gl_exception_t("failed to create vertex array object");
    }

    vertex_array_object_t(vertex_array_object_t &&other) noexcept
        : vertex_array_object_t()
    {
        swap(other);
    }

    vertex_array_object_t &operator=(vertex_array_object_t &&other) noexcept
    {
        swap(other);
        return *this;
    }

    ~vertex_array_object_t()
    {
        if(handle_)
            glDeleteVertexArrays(1, &handle_);
    }

    void swap(vertex_array_object_t &other) noexcept
    {
        std::swap(handle_, other.handle_);
    }

    template<typename Var>
    void enable_attributes(attribute_variable_t<Var> var)
    {
        glEnableVertexArrayAttrib(handle_, var.get_location());
    }

    template<typename Var0, typename Var1, typename...Vars>
    void enable_attributes(
        attribute_variable_t<Var0> var0,
        attribute_variable_t<Var1> var1,
        attribute_variable_t<Vars>...vars)
    {
        this->enable_attributes(var0);
        this->enable_attributes(var1, vars...);
    }

    template<typename Var>
    void set_attribute(
        attribute_variable_t<Var> var,
        GLuint                    input_slot,
        GLuint                    byte_relative_offset,
        bool                      normalized = false)
    {
        glVertexArrayAttribBinding(handle_, var.get_location(), input_slot);
        glVertexArrayAttribFormat(
            handle_, var.get_location(),
            detail::var_to_glsl_type<Var>::usize,
            detail::var_to_glsl_type<Var>::utype,
            normalized, byte_relative_offset);
    }

    template<typename Vertex>
    void bind_vertex_buffer(
        const vertex_buffer_t<Vertex> &buf,
        GLuint                         input_slot,
        size_t                         vertex_offset = 0)
    {
        glVertexArrayVertexBuffer(
            handle_, input_slot, buf.get_handle(),
            vertex_offset * sizeof(Vertex), sizeof(Vertex));
    }

    void unbind_vertex_buffer(GLuint input_slot)
    {
        glVertexArrayVertexBuffer(handle_, input_slot, 0, 0, 0);
    }

    void bind()
    {
        glBindVertexArray(handle_);
    }

    void unbind()
    {
        AGZ_WHEN_DEBUG({
            GLint cur;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cur);
            assert(cur == static_cast<GLint>(handle_));
        });
        glBindVertexArray(0);
    }
};

AGZ_GL_END
