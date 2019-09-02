#pragma once

#ifdef AGZ_ENABLE_OPENGL

#if !(defined(__gl_h_) || defined(__GL_H__))
#   error "Include gl.h/glew.h before this header"
#endif

#include <cassert>

#include "../math.h"

namespace agz::gl
{

using vec2 = math::vec2f;
using vec3 = math::vec3f;
using vec4 = math::vec4f;

using mat3 = math::mat3f_c;
using mat4 = math::mat4f_c;

class gl_object_t
{
protected:

    GLuint handle_;

public:

    explicit gl_object_t(GLuint handle = 0) noexcept
        : handle_(handle)
    {
        
    }

    gl_object_t(gl_object_t &&move_from) noexcept
        : handle_(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    gl_object_t &operator=(gl_object_t &&move_from) noexcept
    {
        assert(!handle_);
        handle_ = move_from.handle_;
        move_from.handle_ = 0;
        return *this;
    }

    gl_object_t(const gl_object_t&)            = delete;
    gl_object_t &operator=(const gl_object_t&) = delete;

    ~gl_object_t()
    {
        assert(!handle_);
    }

    GLuint native_handle() const noexcept
    {
        return handle_;
    }
};

class opengl_exception_t : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

} // namespace agz::gl

#endif // #ifdef AGZ_ENABLE_OPENGL
