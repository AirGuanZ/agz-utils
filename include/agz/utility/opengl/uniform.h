#pragma once

#include <map>

#include "../alloc.h"
#include "./buffer.h"

namespace agz::gl
{

namespace impl
{
    
    inline void set_uniform(GLint loc, GLfloat x)                             { glUniform1f(loc, x);                                  }
    inline void set_uniform(GLint loc, const vec2 &v)                         { glUniform2f(loc, v.x, v.y);                           }
    inline void set_uniform(GLint loc, const vec3 &v)                         { glUniform3f(loc, v.x, v.y, v.z);                      }
    inline void set_uniform(GLint loc, const vec4 &v)                         { glUniform4f(loc, v.x, v.y, v.z, v.w);                 }
    inline void set_uniform(GLint loc, GLint x)                               { glUniform1i(loc, x);                                  }
    inline void set_uniform(GLint loc, GLuint x)                              { glUniform1ui(loc, x);                                 }
    inline void set_uniform(GLint loc, const vec2i &v)                        { glUniform2i(loc, v.x, v.y);                           }
    inline void set_uniform(GLint loc, const vec3i &v)                        { glUniform3i(loc, v.x, v.y, v.z);                      }
    inline void set_uniform(GLint loc, const vec4i &v)                        { glUniform4i(loc, v.x, v.y, v.z, v.w);                 }
    inline void set_uniform(GLint loc, const mat4 &m, bool transpose = false) { glUniformMatrix4fv(loc, 1, transpose, &m.data[0][0]); }
    inline void set_uniform(GLint loc, texture2d_unit_t v)                    { set_uniform(loc, static_cast<GLint>(v.unit));       }

} // namespace impl

/**
 * @brief location of glsl uniform variable
 */
template<typename Var>
class uniform_variable_t
{
    GLint loc_;

public:

    explicit uniform_variable_t(GLint loc = 0) noexcept
        : loc_(loc)
    {
        
    }

    void bind_value(const Var &var) const
    {
        impl::set_uniform(loc_, var);
    }

    GLint location() const noexcept
    {
        return loc_;
    }

    template<typename OtherVarType>
    bool operator<(uniform_variable_t<OtherVarType> rhs) const noexcept
    {
        return loc_ < rhs.location();
    }
};

/**
 * @brief index of glsl uniform block
 */
template<typename Block>
class std140_uniform_block_t
{
    GLuint program_;
    GLuint idx_;

public:

    explicit std140_uniform_block_t(GLuint program = 0, GLuint idx = 0) noexcept
        : program_(program), idx_(idx)
    {
        
    }

    GLuint index() const noexcept
    {
        return idx_;
    }

    void bind(GLuint binding_point) const noexcept
    {
        assert(program_);
        glUniformBlockBinding(program_, idx_, binding_point);
    }
};

/**
 * @brief 一组针对uniform variable/block的赋值
 * 
 * 相当于可以一次设置一堆uniform variable/block的值
 */
class uniform_variable_assignment_t
{
    class record_interface_t
    {
    public:

        virtual ~record_interface_t() = default;

        virtual void bind() const noexcept = 0;

        virtual GLenum glsl_type() const noexcept = 0;

        virtual void set(const void *value) noexcept = 0;
    };

    template<typename Var>
    class uniform_var_record_t : public record_interface_t
    {
    public:

        uniform_variable_t<Var> var;
        Var value;

        uniform_var_record_t(
            uniform_variable_t<Var> var, const Var &value) noexcept
            : var(var), value(value)
        {

        }

        void bind() const noexcept override
        {
            var.bind_value(value);
        }

        GLenum glsl_type() const noexcept override
        {
            return var_to_gl_type<Var>::type;
        }

        void set(const void *value) noexcept override
        {
            assert(value);
            value = *static_cast<const Var*>(value);
        }
    };

    template<typename Block>
    struct std140_uniform_block_record_value_t
    {
        const std140_uniform_block_buffer_t<Block> *buffer;
        GLuint bindingPoint;
    };

    template<typename Block>
    class std140_uniform_block_record_t : public record_interface_t
    {
    public:

        std140_uniform_block_t<Block> block;
        const std140_uniform_block_buffer_t<Block> *buffer;
        GLuint bindingPoint;

        std140_uniform_block_record_t(
            std140_uniform_block_t<Block> block,
            const std140_uniform_block_buffer_t<Block> *buffer,
            GLuint bindingPoint) noexcept
            : block(block), buffer(buffer), bindingPoint(bindingPoint)
        {

        }

        void bind() const noexcept override
        {
            buffer->Bind(bindingPoint);
        }

        [[noreturn]] GLenum glsl_type() const noexcept override
        {
            std::terminate();
        }

        void set(const void *value) noexcept override
        {
            auto tvalue =
                static_cast<const std140_uniform_block_record_value_t<Block>*>(
                    value);
            buffer = tvalue->buffer;
            bindingPoint = tvalue->bindingPoint;
        }
    };

    std::map<GLint, std::unique_ptr<record_interface_t>> assignments_;

public:

    /**
     * @brief 设置uniform变量的值
     */
    template<typename Var>
    void set(uniform_variable_t<Var> var, const Var &val)
    {

        auto it = assignments_.find(var.location());
        if(it != assignments_.end())
        {
            assert(it->second->glsl_type() == var_to_gl_type<Var>::type);
            it->second->set(&val);
            return;
        }
        auto rc = std::make_unique<uniform_var_record_t<Var>>(var, val);
        assignments_[var.location()] = std::move(rc);
    }

    /**
     * @brief 设置uniform block所绑定的buffer
     */
    template<typename Block>
    void set(
        std140_uniform_block_t<Block> block,
        const std140_uniform_block_buffer_t<Block> *buffer,
        GLuint binding_point)
    {
        GLint key = static_cast<GLint>(block.index()) | 0xF00000;
        assert(static_cast<GLuint>(key) != block.index());
        auto it = assignments_.find(key);
        if(it != assignments_.end())
        {
            std140_uniform_block_record_value_t<Block> val =
                { buffer, binding_point };
            it->second->set(&val);
            return;
        }
        auto rc = std::make_unique<std140_uniform_block_record_t<Block>>(
            block, buffer, binding_point);
        assignments_[key] = std::move(rc);
    }

    /**
     * @brief 清空之前所有的设置
     */
    void clear()
    {
        assignments_.clear();
    }

    /**
     * @brief 将之前设置的值和绑定的buffer应用于shader uniform参数
     */
    void bind() const noexcept
    {
        for(auto &it : assignments_)
            it.second->bind();
    }
};

} // namespace agz::gl
