#pragma once

#include "../texture.h"
#include "./common.h"

namespace agz::gl
{
    
namespace impl
{
    
    template<typename DataTixel> struct data_type_to_texel_type;
    
    template<> struct data_type_to_texel_type<GLfloat>       { static constexpr GLenum format = GL_RED;  static constexpr GLenum type = GL_FLOAT;         static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<vec3>          { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_FLOAT;         static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<vec4>          { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_FLOAT;         static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<math::color3f> { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_FLOAT;         static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<math::color4f> { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_FLOAT;         static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<GLubyte>       { static constexpr GLenum format = GL_RED;  static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint row_align = 1; };
    template<> struct data_type_to_texel_type<vec3b>         { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint row_align = 1; };
    template<> struct data_type_to_texel_type<vec4b>         { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<math::color3b> { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint row_align = 1; };
    template<> struct data_type_to_texel_type<math::color4b> { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint row_align = 4; };
    template<> struct data_type_to_texel_type<GLuint>        { static constexpr GLenum format = GL_R;    static constexpr GLenum type = GL_UNSIGNED_INT;  static constexpr GLint row_align = 4; };

} // namespace impl

/**
 * @brief 对opengl中二维纹理对象的直接封装
 */
class texture2d_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即申请一个gl texture name
     */
    explicit texture2d_t(bool init_handle = false)
    {
        if(init_handle)
            initialize_handle();
    }

    texture2d_t(texture2d_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    texture2d_t &operator=(texture2d_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~texture2d_t()
    {
        destroy();
    }

    /**
     * @brief 申请一个gl texture name
     *
     * 不得在已经持有texture name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateTextures(GL_TEXTURE_2D, 1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create texture2d object");
    }

    /**
     * @brief 若含有texture name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 初始化texture的格式和数据
     * 
     * internal_format取值参见glTextureStorage2D
     */
    template<typename DataTexel>
    void initialize_format_and_data(
        GLsizei levels, GLenum internal_format,
        GLsizei width, GLsizei height, const DataTexel *data)
    {
        assert(handle_);
        initialize_format(levels, internal_format, width, height);
        reinitialize_data(width, height, data);
    }

    /**
     * @brief 初始化texture的格式和数据
     *
     * internal_format取值参见glTextureStorage2D
     */
    template<typename DataTexel>
    void initialize_format_and_data(
        GLsizei levels, GLenum internal_format,
        const texture::texture2d_t<DataTexel> &data)
    {
        initialize_format_and_data(
            levels, internal_format,
            GLsizei(data.width()), GLsizei(data.height()),
            data.get_data().raw_data());
    }

    /**
     * @brief 初始化texture的格式和数据
     *
     * internal_format取值参见glTextureStorage2D
     */
    template<typename DataTexel>
    void initialize_format_and_data(
        GLsizei levels, GLenum internal_format,
        const math::tensor_t<DataTexel, 2> &data)
    {
        initialize_format_and_data(
            levels, internal_format,
            GLsizei(data.shape()[1]), GLsizei(data.shape()[0]),
            data.raw_data());
    }

    /**
     * @brief 初始化texture的存储格式
     *
     * internal_format取值参见glTextureStorage2D
     */
    void initialize_format(
        GLsizei levels, GLenum internal_format, GLsizei width, GLsizei height)
    {
        assert(handle_);
        glTextureStorage2D(handle_, levels, internal_format, width, height);
    }

    /**
     * @brief 初始化texture的数据
     */
    template<typename DataTexel>
    void reinitialize_data(GLsizei width, GLsizei height, const DataTexel *data)
    {
        assert(handle_);
        GLint old_align;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &old_align);
        glPixelStorei(
            GL_UNPACK_ALIGNMENT,
            impl::data_type_to_texel_type<DataTexel>::row_align);
        glTextureSubImage2D(
            handle_, 0, 0, 0, width, height,
            impl::data_type_to_texel_type<DataTexel>::format,
            impl::data_type_to_texel_type<DataTexel>::type, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, old_align);
        glGenerateTextureMipmap(handle_);
    }

    /**
     * @brief 设置默认采样参数
     */
    void set_param(GLenum param_name, GLint param_value)
    {
        assert(handle_);
        glTextureParameteri(handle_, param_name, param_value);
    }

    /**
     * @brief 绑定到指定texture_unit
     */
    void bind(GLuint texture_unit) const noexcept
    {
        assert(handle_);
        glBindTextureUnit(texture_unit, handle_);
    }

    /**
     * @brief 从指定texture_unit解绑
     */
    void unbind(GLuint texture_unit) const noexcept
    {
        assert(handle_);
        glBindTextureUnit(texture_unit, 0);
    }
};

} // namespace agz::gl
