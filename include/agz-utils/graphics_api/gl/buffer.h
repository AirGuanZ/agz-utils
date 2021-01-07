#pragma once

#include <agz-utils/graphics_api/gl/common.h>

AGZ_GL_BEGIN

class buffer_t : public misc::uncopyable_t
{
protected:

    GLuint handle_;

public:

    buffer_t();

    ~buffer_t();

    void initialize_data(const void *data, size_t byte_size, GLenum usage);

    void set_data(const void *data, size_t byte_offset, size_t byte_size);

    GLuint get_handle() const noexcept;

    operator GLuint () const noexcept;
};

template<typename Vertex>
class vertex_buffer_t : public buffer_t
{
    size_t vertex_count_ = 0;

public:

    using buffer_t::buffer_t;

    vertex_buffer_t(const Vertex *data, size_t vertex_count, GLenum usage);

    void initialize_data(const Vertex *data, size_t vertex_count, GLenum usage);

    void set_data(const Vertex *data, size_t vertex_offset, size_t vertex_count);

    size_t get_vertex_count() const noexcept;
};

template<typename Struct>
class uniform_buffer_t : public buffer_t
{
public:

    using buffer_t::buffer_t;

    explicit uniform_buffer_t(GLenum usage);

    uniform_buffer_t(const Struct &data, GLenum usage);

    void initialize_data(GLenum usage);

    void initialize_data(const Struct &data, GLenum usage);

    void set_data(const Struct &data);

    void bind(GLuint bind_point) const noexcept;

    void unbind(GLuint bind_point) const noexcept;
};

inline buffer_t::buffer_t()
    : handle_(0)
{
    glCreateBuffers(1, &handle_);
    if(!handle_)
        throw gl_exception_t("failed to create buffer");
}

inline buffer_t::~buffer_t()
{
    glDeleteBuffers(1, &handle_);
}

inline void buffer_t::initialize_data(
    const void *data,
    size_t      byte_size,
    GLenum      usage)
{
    glNamedBufferData(handle_, byte_size, data, usage);
}

inline void buffer_t::set_data(
    const void *data,
    size_t      byte_offset,
    size_t      byte_size)
{
    glNamedBufferSubData(
        handle_,
        static_cast<GLintptr>(byte_offset),
        static_cast<GLsizeiptr>(byte_size),
        data);
}

inline GLuint buffer_t::get_handle() const noexcept
{
    return handle_;
}

inline buffer_t::operator unsigned() const noexcept
{
    return handle_;
}

template<typename Vertex>
vertex_buffer_t<Vertex>::vertex_buffer_t(
    const Vertex *data,
    size_t        vertex_count,
    GLenum        usage)
    : buffer_t()
{
    buffer_t::initialize_data(data, sizeof(Vertex) * vertex_count, usage);
    vertex_count_ = vertex_count;
}

template<typename Vertex>
void vertex_buffer_t<Vertex>::initialize_data(
    const Vertex *data,
    size_t        vertex_count,
    GLenum        usage)
{
    buffer_t::initialize_data(data, sizeof(Vertex) * vertex_count, usage);
    vertex_count_ = vertex_count;
}

template<typename Vertex>
void vertex_buffer_t<Vertex>::set_data(
    const Vertex *data,
    size_t        vertex_offset,
    size_t        vertex_count)
{
    buffer_t::set_data(
        data, sizeof(Vertex) * vertex_offset, sizeof(Vertex) * vertex_count);
}

template<typename Vertex>
size_t vertex_buffer_t<Vertex>::get_vertex_count() const noexcept
{
    return vertex_count_;
}

template<typename Struct>
uniform_buffer_t<Struct>::uniform_buffer_t(GLenum usage)
    : buffer_t()
{
    initialize_data(usage);
}

template<typename Struct>
uniform_buffer_t<Struct>::uniform_buffer_t(const Struct &data, GLenum usage)
    : buffer_t()
{
    initialize_data(data, usage);
}

template<typename Struct>
void uniform_buffer_t<Struct>::initialize_data(GLenum usage)
{
    buffer_t::initialize_data(nullptr, sizeof(Struct), usage);
}

template<typename Struct>
void uniform_buffer_t<Struct>::initialize_data(const Struct &data, GLenum usage)
{
    buffer_t::initialize_data(&data, sizeof(Struct), usage);
}

template<typename Struct>
void uniform_buffer_t<Struct>::set_data(const Struct &data)
{
    buffer_t::set_data(data, 0, sizeof(Struct));
}

template<typename Struct>
void uniform_buffer_t<Struct>::bind(GLuint bind_point) const noexcept
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bind_point, handle_);
}

template<typename Struct>
void uniform_buffer_t<Struct>::unbind(GLuint bind_point) const noexcept
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bind_point, 0);
}

AGZ_GL_END
