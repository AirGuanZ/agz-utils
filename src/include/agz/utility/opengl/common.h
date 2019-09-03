#pragma once

#if !(defined(__gl_h_) || defined(__GL_H__))
    #error "include opengl header before this"
#endif

#include <cassert>

#include "../math.h"

namespace agz::gl
{

using vec2 = math::vec2f;
using vec3 = math::vec3f;
using vec4 = math::vec4f;

using vec2i = math::vec2i;
using vec3i = math::vec3i;
using vec4i = math::vec4i;

using vec2b = math::tvec2<unsigned char>;
using vec3b = math::tvec3<unsigned char>;
using vec4b = math::tvec4<unsigned char>;

using mat3 = math::mat3f_c;
using mat4 = math::mat4f_c;

struct texture2d_unit_t { GLuint unit; };

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

    GLuint handle() const noexcept
    {
        return handle_;
    }
};

class opengl_exception_t : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

template<typename Var> struct var_to_gl_type;

template<> struct var_to_gl_type<GLfloat>          { static constexpr GLenum type = GL_FLOAT;      static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 1; };
template<> struct var_to_gl_type<GLint>            { static constexpr GLenum type = GL_INT;        static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 1; };
template<> struct var_to_gl_type<vec2>             { static constexpr GLenum type = GL_FLOAT_VEC2; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 2; };
template<> struct var_to_gl_type<vec3>             { static constexpr GLenum type = GL_FLOAT_VEC3; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 3; };
template<> struct var_to_gl_type<vec4>             { static constexpr GLenum type = GL_FLOAT_VEC4; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 4; };
template<> struct var_to_gl_type<vec2i>            { static constexpr GLenum type = GL_INT_VEC2;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 2; };
template<> struct var_to_gl_type<vec3i>            { static constexpr GLenum type = GL_INT_VEC3;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 3; };
template<> struct var_to_gl_type<vec4i>            { static constexpr GLenum type = GL_INT_VEC4;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 4; };
template<> struct var_to_gl_type<mat4>             { static constexpr GLenum type = GL_FLOAT_MAT4; };
template<> struct var_to_gl_type<texture2d_unit_t> { static constexpr GLenum type = GL_SAMPLER_2D; };

} // namespace agz::gl
