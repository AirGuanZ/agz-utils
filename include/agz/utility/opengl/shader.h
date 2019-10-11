#pragma once

#include "../file.h"
#include "../misc.h"
#include "./common.h"

namespace agz::gl
{

/**
 * @brief 对gl shader object的直接封装
 */
template<GLenum TShaderType>
class shader_t : public gl_object_t
{
public:

    static constexpr GLenum ShaderType = TShaderType;

    using self_t = shader_t<ShaderType>;

    shader_t() = default;

    shader_t(self_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    self_t &operator=(self_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~shader_t()
    {
        destroy();
    }

    /**
     * @brief 若含有shader name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteShader(handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 从源代码编译shader
     * 
     * 若已含有别的shader，先destroy掉
     */
    void load_from_memory(std::string_view src)
    {
        destroy();

        GLuint new_handle = glCreateShader(ShaderType);
        if(!new_handle)
            throw opengl_exception_t("failed to create shader object");
        misc::scope_guard_t new_handle_guard([&] { glDeleteShader(new_handle); });

        const char *char_src = src.data();
        GLint len_src = static_cast<GLint>(src.length());
        glShaderSource(new_handle, 1, &char_src, &len_src);

        GLint result;
        glCompileShader(new_handle);
        glGetShaderiv(new_handle, GL_COMPILE_STATUS, &result);
        if(result != GL_TRUE)
        {
            GLint log_len;
            glGetShaderiv(new_handle, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log_buf(log_len + 1);
            glGetShaderInfoLog(new_handle, log_len + 1, nullptr, log_buf.data());
            throw opengl_exception_t(log_buf.data());
        }

        handle_ = new_handle;
        new_handle_guard.dismiss();
    }

    /**
     * @brief 从源文件中编译shader
     * 
     * 若已含有别的shader，先destroy掉
     */
    void load_from_file(std::string_view filename)
    {
        std::string src = file::read_txt_file(std::string(filename));
        load_from_memory(src);
    }

    /**
     * @brief 从源代码中编译shader，返回得到的shader_t
     */
    static self_t from_memory(std::string_view src)
    {
        self_t ret;
        ret.load_from_memory(src);
        return std::move(ret);
    }

    /**
     * @brief 从源文件中编译shader，返回得到的shader_t
     */
    static self_t from_file(std::string_view filename)
    {
        self_t ret;
        ret.load_from_file(filename);
        return std::move(ret);
    }
};

using vertex_shader_t   = shader_t<GL_VERTEX_SHADER>;
using geometry_shader_t = shader_t<GL_GEOMETRY_SHADER>;
using fragment_shader_t = shader_t<GL_FRAGMENT_SHADER>;

} // namespace agz::gl
