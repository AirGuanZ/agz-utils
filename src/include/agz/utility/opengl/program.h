#pragma once

#include <cassert>
#include <vector>

#include "../system/platform.h"
#include "./attrib.h"
#include "./shader.h"
#include "./uniform.h"

namespace agz::gl
{
    
/**
 * @brief 对shader program的直接封装
 */
class program_t : public gl_object_t
{
public:

    explicit program_t(GLuint handle = 0) noexcept
        : gl_object_t(handle)
    {
        AGZ_WHEN_DEBUG({ assert(!handle || glIsProgram(handle)); });
    }

    program_t(program_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    program_t &operator=(program_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~program_t()
    {
        destroy();
    }

    /**
     * @brief 将给定的几个shader object链接，返回得到的program
     */
    template<GLenum...ShaderTypes>
    static program_t build_from(const shader_t<ShaderTypes>&...shaders);

    /**
     * @brief 将持有的shader program标记为删除，并释放其所有权
     */
    void destroy() noexcept
    {
        if(handle_)
        {
            glDeleteProgram(handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 开始使用该shader program
     */
    void bind() const noexcept
    {
        assert(handle_);
        glUseProgram(handle_);
    }

    /**
     * @brief 结束对该shader program的使用
     */
    void unbind() const noexcept
    {
        assert(handle_);
        AGZ_WHEN_DEBUG({
            GLint cur;
            glGetIntegerv(GL_CURRENT_PROGRAM, &cur);
            assert(cur == GLint(handle_));
        });
        glUseProgram(0);
    }

    /**
     * @brief 不作类型检查，直接设置某个uniform变量的值
     */
    template<typename Var>
    void set_uniform_unchecked(const char *name, const Var &val) const
    {
        assert(handle_);
        auto loc = glGetUniformLocation(handle_, name);
        if(loc < 0)
            return;
        impl::set_uniform(loc, val);
    }

    /**
     * @brief 返回shader中具有指定类型和名字的uniform variable
     * 
     * 会进行类型检查
     */
    template<typename Var>
    uniform_variable_t<Var> uniform_variable(const char *name) const
    {
        assert(handle_ && name);

        GLuint index;
        glGetUniformIndices(handle_, 1, &name, &index);
        if(index == GL_INVALID_INDEX)
            throw opengl_exception_t("invalid uniform variable name: " + std::string(name));

        GLint size;
        GLenum type;
        glGetActiveUniform(handle_, index, 0, nullptr, &size, &type, nullptr);
        if(type != var_to_gl_type<Var>::type)
            throw opengl_exception_t("invalid uniform variable type of " + std::string(name));

        auto loc = glGetUniformLocation(handle_, name);
        return uniform_variable_t<Var>(loc);
    }

    /**
     * @brief 返回program中具有指定名字的uniform block
     * 
     * 会进行uniform block和Block间的大小检查
     */
    template<typename Block>
    std140_uniform_block_t<Block> std140_uniform_block(const char *name) const
    {
        assert(handle_ && name);

        GLuint index = glGetUniformBlockIndex(handle_, name);
        if(index == GL_INVALID_INDEX)
            throw opengl_exception_t("invalid std140 uniform block name: " + std::string(name));

        GLint size;
        glGetActiveUniformBlockiv(handle_, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
        if(sizeof(Block) != size)
            throw opengl_exception_t("unmatched std140 uniform block size of " + std::string(name));

        return std140_uniform_block_t<Block>(handle_, index);
    }

    /**
     * @brief 返回program中具有指定名字的attrib variable loc
     * 
     * 会根据模板参数和glsl反射进行类型检查
     */
    template<typename Var>
    attrib_variable_t<Var> attrib_variable(const char *name) const
    {
        assert(handle_);

        GLint loc = glGetAttribLocation(handle_, name);
        if(loc < 0)
            throw opengl_exception_t("invalid attrib variable name: " + std::string(name));

        GLint size; GLenum type;
        glGetActiveAttrib(handle_, loc, 0, nullptr, &size, &type, nullptr);
        if(type != var_to_gl_type<Var>::type)
            throw opengl_exception_t("unmatched attrib variable type of " + std::string(name));

        return attrib_variable_t<Var>(loc);
    }
};

namespace impl
{

    class program_builder_t
    {
        std::vector<GLuint> shader_handles_;

        template<GLenum ShaderType>
        void add_shader_impl(const shader_t<ShaderType> &shader)
        {
            shader_handles_.push_back(shader.handle());
        }

        template<GLenum ShaderType0, GLenum ShaderType1, GLenum...OtherShaderTypes>
        void add_shader_impl(
            const shader_t<ShaderType0> &shader0,
            const shader_t<ShaderType1> &shader1,
            const shader_t<OtherShaderTypes>&...other_shaders)
        {
            add_shader_impl(shader0);
            add_shader_impl(shader1, other_shaders...);
        }

    public:

        template<GLenum...ShaderTypes>
        static program_t build(const shader_t<ShaderTypes>&...shaders)
        {
            program_builder_t builder;
            builder.add_shader_impl(shaders...);
            return builder.build();
        }

        program_t build() const
        {
            GLuint handle = glCreateProgram();
            if(!handle)
                throw opengl_exception_t("failed to create program object");
            misc::scope_guard_t handle_guard([=]()
            {
                if(handle)
                {
                    std::vector<GLuint> attached_shaders(shader_handles_.size());
                    GLsizei count;
                    glGetAttachedShaders(handle, GLsizei(attached_shaders.size()), &count, attached_shaders.data());
                    for(GLsizei i = 0; i < count; ++i)
                        glDetachShader(handle, attached_shaders[i]);
                    glDeleteProgram(handle);
                }
            });

            for(auto shader_handle : shader_handles_)
                glAttachShader(handle, shader_handle);

            glLinkProgram(handle);

            GLint result;
            glGetProgramiv(handle, GL_LINK_STATUS, &result);
            if(result != GL_TRUE)
            {
                GLint log_len;
                glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_len);
                std::vector<char> log_buf(log_len + 1);
                glGetProgramInfoLog(handle, GLsizei(log_buf.size()), nullptr, log_buf.data());
                throw opengl_exception_t(log_buf.data());
            }

            for(auto shader_handle : shader_handles_)
                glDetachShader(handle, shader_handle);

            handle_guard.dismiss();
            return program_t(handle);
        }
    };

} // namespace impl

template<GLenum... ShaderTypes>
program_t program_t::build_from(const shader_t<ShaderTypes>&...shaders)
{
    return impl::program_builder_t::build(shaders...);
}

} // namespace agz::gl
