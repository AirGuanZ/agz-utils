#pragma once

#include "../system/platform.h"
#include "./attrib.h"
#include "./buffer.h"

namespace agz::gl
{

/**
 * @brief VAO
 */
class vertex_array_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即c创建一个vao
     */
    explicit vertex_array_t(bool init_handle = false)
        : gl_object_t(0)
    {
        if(init_handle)
            initialize_handle();
    }

    vertex_array_t(vertex_array_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    vertex_array_t &operator=(vertex_array_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~vertex_array_t()
    {
        destroy();
    }

    /**
     * @brief 创建一个vao
     *
     * 不得在已经持有vao的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateVertexArrays(1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create vertex array object");
    }

    /**
     * @brief 若含有vao，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteVertexArrays(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 设置指定attrib变量对应的vertex buffer binding point
     */
    template<typename Var>
    void set_attrib_binding_point(attrib_variable_t<Var> var, GLuint binding_point) noexcept
    {
        assert(handle_);
        glVertexArrayAttribBinding(handle_, var.location(), binding_point);
    }

    /**
     * @brief 设置指定attrib变量对应的vertex buffer的输入格式
     * 
     * @param var attrib变量，通常由program_t::get_attrib_variable取得
     * @param byte_relative_offset 该属性在vertex buffer element中的字节偏移量
     * @param normalized 是否在读取该属性时将其normalize
     */
    template<typename Var>
    void set_attrib_format(attrib_variable_t<Var> var, GLuint byte_relative_offset, bool normalized) noexcept
    {
        assert(handle_);
        glVertexArrayAttribFormat(
            handle_, var.location(), var_to_gl_type<Var>::usize, var_to_gl_type<Var>::utype, normalized, byte_relative_offset);
    }

    /**
     * @brief 将vertex buffer绑定到指定的vertex buffer binding point
     */
    template<typename Vertex>
    void set_vertex_buffer_binding_point(const vertex_buffer_t<Vertex> &buf, GLuint binding_point, size_t vertex_offset = 0) noexcept
    {
        assert(handle_);
        glVertexArrayVertexBuffer(
            handle_, binding_point, buf.handle(), GLintptr(vertex_offset * sizeof(Vertex)), GLsizei(sizeof(Vertex)));
    }

    /**
     * @brief 启用指定的attrib输入
     */
    template<typename Var>
    void enable_attrib(attrib_variable_t<Var> var) noexcept
    {
        assert(handle_);
        glEnableVertexArrayAttrib(handle_, var.location());
    }

    /**
     * @brief 将某vertex buffer绑定到指定attrib上
     * 
     * 适用于vertex buffer element正好就是attrib type的情况
     * 
     * set_attrib_binding_point，set_attrib_format，set_vertex_buffer_binding_point三合一
     * 
     * @param var attribute variable
     * @param buf vertex buffer
     * @param binding_point vertex buffer binding point
     * @param normalized 读取输入时是否归一化
     * @param vertex_offset 从vertex buffer中下标为vertex_offset的顶点开始读取
     */
    template<typename Vertex>
    void bind_vertex_buffer_to_attrib(
        attrib_variable_t<Vertex> var, const vertex_buffer_t<Vertex> &buf,
        GLuint binding_point, bool normalized = false, size_t vertex_offset = 0) noexcept
    {
        set_attrib_binding_point(var, binding_point);
        set_attrib_format(var, 0, normalized);
        set_vertex_buffer_binding_point(buf, binding_point, vertex_offset);
    }

    /**
     * @brief 将某vertex buffer绑定到指定attrib上
     *
     * 适用于vertex buffer element的某个成员是需要的attrib type的情况
     *
     * set_attrib_binding_point，set_attrib_format，set_vertex_buffer_binding_point三合一
     *
     * @param var attribute variable
     * @param buf vertex buffer
     * @param mem vertex member pointer，也就是要将顶点的哪个成员绑定到该attrib上
     * @param binding_point vertex buffer binding point
     * @param normalized 读取输入时是否归一化
     * @param vertex_offset 从vertex buffer中下标为vertex_offset的顶点开始读取
     */
    template<typename Vertex, typename Var>
    void bind_vertex_buffer_to_attrib(
        attrib_variable_t<Var> var, const vertex_buffer_t<Vertex> &buf, Var (Vertex::*mem),
        GLuint binding_point, bool normalized = false, size_t vertex_offset = 0) noexcept
    {
        set_attrib_binding_point(var, binding_point);
        set_attrib_format(var, GLuint(byte_offset(mem)), normalized);
        set_vertex_buffer_binding_point(buf, binding_point, vertex_offset);
    }

    /**
     * @brief 启用该vao进行绘制
     */
    void bind() const noexcept
    {
        assert(handle_);
        glBindVertexArray(handle_);
    }

    /**
     * @brief 结束用该vao进行的绘制
     */
    void unbind() const noexcept
    {
        assert(handle_);
        AGZ_WHEN_DEBUG({
            GLint cur;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cur);
            assert(cur == GLint(handle_));
        });
        glBindVertexArray(0);
    }
};
    
} // namespace agz::gl
