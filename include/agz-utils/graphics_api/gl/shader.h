#pragma once

#include <map>

#include <agz-utils/file.h>
#include <agz-utils/graphics_api/gl/buffer.h>

AGZ_GL_BEGIN

struct texture_unit_t { GLuint unit; };

namespace detail
{

    inline void set_uniform(GLint loc, GLfloat         x) { glUniform1f(loc, x);                              }
    inline void set_uniform(GLint loc, const vec2     &v) { glUniform2f(loc, v.x, v.y);                       }
    inline void set_uniform(GLint loc, const vec3     &v) { glUniform3f(loc, v.x, v.y, v.z);                  }
    inline void set_uniform(GLint loc, const vec4     &v) { glUniform4f(loc, v.x, v.y, v.z, v.w);             }
    inline void set_uniform(GLint loc, GLint           x) { glUniform1i(loc, x);                              }
    inline void set_uniform(GLint loc, const vec2i    &v) { glUniform2i(loc, v.x, v.y);                       }
    inline void set_uniform(GLint loc, const vec3i    &v) { glUniform3i(loc, v.x, v.y, v.z);                  }
    inline void set_uniform(GLint loc, const vec4i    &v) { glUniform4i(loc, v.x, v.y, v.z, v.w);             }
    inline void set_uniform(GLint loc, const mat4     &m) { glUniformMatrix4fv(loc, 1, false, &m.data[0][0]); }
    inline void set_uniform(GLint loc, texture_unit_t &t) { glUniform1i(loc, t.unit);                         }

    template<typename Var> struct var_to_glsl_type;

    template<> struct var_to_glsl_type<GLfloat>        { static constexpr GLenum type = GL_FLOAT;      static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 1; };
    template<> struct var_to_glsl_type<GLint>          { static constexpr GLenum type = GL_INT;        static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 1; };
    template<> struct var_to_glsl_type<vec2>           { static constexpr GLenum type = GL_FLOAT_VEC2; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 2; };
    template<> struct var_to_glsl_type<vec3>           { static constexpr GLenum type = GL_FLOAT_VEC3; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 3; };
    template<> struct var_to_glsl_type<vec4>           { static constexpr GLenum type = GL_FLOAT_VEC4; static constexpr GLenum utype = GL_FLOAT; static constexpr GLint usize = 4; };
    template<> struct var_to_glsl_type<vec2i>          { static constexpr GLenum type = GL_INT_VEC2;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 2; };
    template<> struct var_to_glsl_type<vec3i>          { static constexpr GLenum type = GL_INT_VEC3;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 3; };
    template<> struct var_to_glsl_type<vec4i>          { static constexpr GLenum type = GL_INT_VEC4;   static constexpr GLenum utype = GL_INT;   static constexpr GLint usize = 4; };
    template<> struct var_to_glsl_type<mat4>           { static constexpr GLenum type = GL_FLOAT_MAT4; };
    template<> struct var_to_glsl_type<texture_unit_t> { static constexpr GLenum type = GL_SAMPLER_2D; };


} // namespace detail

template<typename Var>
class attribute_variable_t
{
    GLuint loc_;

public:

    explicit attribute_variable_t(GLuint loc = -1) noexcept
        : loc_(loc)
    {
        
    }

    GLuint get_location() const noexcept
    {
        return loc_;
    }
};

template<typename Var>
class uniform_variable_slot_t
{
    GLint loc_;

    Var value_;

public:

    explicit uniform_variable_slot_t(GLint loc = -1)
        : loc_(loc)
    {
        
    }

    void set_value(const Var &value)
    {
        value_ = value;
    }

    void apply()
    {
        detail::set_uniform(loc_, value_);
    }
};

class uniform_buffer_slot_t
{
    GLuint bind_point_;

    RC<buffer_t> buffer_;

public:

    explicit uniform_buffer_slot_t(GLuint bind_point)
        : bind_point_(bind_point)
    {
        
    }

    void set_buffer(RC<buffer_t> buffer)
    {
        buffer_.swap(buffer);
    }

    void apply()
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bind_point_, buffer_->get_handle());
    }
};

class uniform_variable_manager_t : public misc::uncopyable_t
{
    class slot_cont_t
    {
    public:

        virtual ~slot_cont_t() = default;

        virtual void apply() = 0;
    };

    template<typename Var>
    class slot_cont_impl_t : public slot_cont_t
    {
    public:

        uniform_variable_slot_t<Var> slot;

        void apply() override
        {
            slot.apply();
        }
    };

    struct record_t
    {
        std::unique_ptr<slot_cont_t> slot;
        GLint loc;
        GLenum type;
    };

    mutable std::map<std::string, record_t, std::less<>> slots_;

public:

    struct uniform_var_info_t
    {
        GLint loc;
        GLenum type;
    };

    explicit uniform_variable_manager_t(
        const std::map<std::string, uniform_var_info_t> &slots)
    {
        for(auto &s : slots)
        {
            slots_.insert(
                std::make_pair(
                    s.first, record_t{ nullptr, s.second.loc, s.second.type }));
        }
    }

    uniform_variable_manager_t(uniform_variable_manager_t &&)            noexcept = default;
    uniform_variable_manager_t &operator=(uniform_variable_manager_t &&) noexcept = default;

    template<typename Var>
    uniform_variable_slot_t<Var> *get_slot(std::string_view name)
    {
        const auto it = slots_.find(name);
        if(it == slots_.end())
        {
            throw gl_exception_t(
                "unknown uniform variable: " + std::string(name));
        }

        if(!it->second.slot)
        {
            if(it->second.type != detail::var_to_glsl_type<Var>::type)
            {
                throw gl_exception_t(
                    "unmatched uniform variable type: " + std::string(name));
            }

            auto new_slot_cont = std::make_unique<slot_cont_impl_t<Var>>();
            new_slot_cont->slot = uniform_variable_slot_t<Var>(it->second.loc);

            auto ret = &new_slot_cont->slot;
            it->second.slot = std::move(new_slot_cont);
            return ret;
        }

        auto slot_cont =
            dynamic_cast<slot_cont_impl_t<Var> *>(it->second.slot.get());
        if(!slot_cont)
        {
            throw gl_exception_t(
                "unmatched uniform variable type: " + std::string(name));
        }

        return &slot_cont->slot;
    }

    void apply()
    {
        for(auto &s : slots_)
        {
            if(s.second.slot)
                s.second.slot->apply();
        }
    }
};

class uniform_buffer_manager_t : public misc::uncopyable_t
{
    std::map<std::string, uniform_buffer_slot_t, std::less<>> slots_;

public:

    struct uniform_buffer_info_t
    {
        GLuint bind_point;
    };

    explicit uniform_buffer_manager_t(
        const std::map<std::string, uniform_buffer_info_t> &slots)
    {
        for(auto &s : slots)
        {
            slots_.insert(std::make_pair(
                s.first, uniform_buffer_slot_t(s.second.bind_point)));
        }
    }

    uniform_buffer_manager_t(uniform_buffer_manager_t &&)            noexcept = default;
    uniform_buffer_manager_t &operator=(uniform_buffer_manager_t &&) noexcept = default;

    uniform_buffer_slot_t *get_slot(std::string_view name)
    {
        auto it = slots_.find(name);
        if(it == slots_.end())
        {
            throw gl_exception_t(
                "unknown uniform buffer: " + std::string(name));
        }
        return &it->second;
    }

    void apply()
    {
        for(auto &s : slots_)
            s.second.apply();
    }
};

class uniform_manager_t
{
    uniform_variable_manager_t var_mgr_;
    uniform_buffer_manager_t   buf_mgr_;

public:

    uniform_manager_t(
        uniform_variable_manager_t var,
        uniform_buffer_manager_t   buf) noexcept
        : var_mgr_(std::move(var)), buf_mgr_(std::move(buf))
    {
        
    }

    template<typename Var>
    uniform_variable_slot_t<Var> *get_variable(std::string_view name)
    {
        return var_mgr_.get_slot<Var>(name);
    }

    uniform_buffer_slot_t *get_buffer(std::string_view name)
    {
        return buf_mgr_.get_slot(name);
    }

    void apply()
    {
        var_mgr_.apply();
        buf_mgr_.apply();
    }
};

template<GLenum Stage>
class shader_stage_t : public misc::uncopyable_t
{
    friend class shader_t;

    GLuint handle_ = 0;

    void destroy()
    {
        if(handle_)
        {
            glDeleteShader(handle_);
            handle_ = 0;
        }
    }

public:

    shader_stage_t() = default;

    shader_stage_t(shader_stage_t<Stage> &&other) noexcept
        : shader_stage_t()
    {
        this->swap(other);
    }

    shader_stage_t<Stage> &operator=(shader_stage_t<Stage> &&other) noexcept
    {
        this->swap(other);
        return *this;
    }

    ~shader_stage_t()
    {
        destroy();
    }

    void swap(shader_stage_t<Stage> &other) noexcept
    {
        std::swap(handle_, other.handle_);
    }

    static shader_stage_t<Stage> from_string(std::string_view src)
    {
        shader_stage_t<Stage> ret;
        ret.load_string(src);
        return ret;
    }

    static shader_stage_t<Stage> from_file(const std::string &filename)
    {
        shader_stage_t<Stage> ret;
        ret.load_file(filename);
        return ret;
    }

    void load_string(std::string_view src)
    {
        destroy();

        handle_ = glCreateShader(Stage);
        if(!handle_)
            throw gl_exception_t("failed to create shader");
        misc::fixed_scope_guard_t shader_guard([&] { destroy(); });

        const char   *data = src.data();
        const GLsizei len  = static_cast<GLsizei>(src.length());
        glShaderSource(handle_, 1, &data, &len);
        glCompileShader(handle_);

        GLint result;
        glGetShaderiv(handle_, GL_COMPILE_STATUS, &result);
        if(result != GL_TRUE)
        {
            GLint log_len;
            glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> log_buf(log_len + 1);
            glGetShaderInfoLog(handle_, log_len + 1, nullptr, log_buf.data());
            throw gl_exception_t(log_buf.data());
        }

        shader_guard.dismiss();
    }

    void load_file(const std::string &filename)
    {
        const auto src = file::read_txt_file(filename);
        load_string(src);
    }
};

using vertex_shader_t   = shader_stage_t<GL_VERTEX_SHADER>;
using geometry_shader_t = shader_stage_t<GL_GEOMETRY_SHADER>;
using fragment_shader_t = shader_stage_t<GL_FRAGMENT_SHADER>;

class shader_t : public misc::uncopyable_t
{
    GLuint handle_;

    template<GLenum Stage>
    void attach(const shader_stage_t<Stage> &stage)
    {
        glAttachShader(handle_, stage.handle_);
    }

    template<GLenum Stage0, GLenum Stage1, GLenum...Stages>
    void attach(
        const shader_stage_t<Stage0> &stage0,
        const shader_stage_t<Stage1> &stage1,
        const shader_stage_t<Stages> &...stages)
    {
        this->attach(stage0);
        this->attach(stage1, stages...);
    }

    template<GLenum Stage>
    void detach(const shader_stage_t<Stage> &stage)
    {
        glDetachShader(handle_, stage.handle_);
    }

    template<GLenum Stage0, GLenum Stage1, GLenum...Stages>
    void detach(
        const shader_stage_t<Stage0> &stage0,
        const shader_stage_t<Stage1> &stage1,
        const shader_stage_t<Stages> &...stages)
    {
        this->attach(stage0);
        this->attach(stage1, stages...);
    }

public:

    shader_t()
        : handle_(0)
    {

    }

    template<GLenum...Stages>
    explicit shader_t(const shader_stage_t<Stages> &...stages)
        : shader_t()
    {
        handle_ = glCreateProgram();
        if(!handle_)
            throw gl_exception_t("failed to create program object");

        this->attach(stages...);

        std::vector<GLuint> attached_shaders(sizeof...(stages));
        misc::fixed_scope_guard_t guard([&]
        {
            GLsizei count;
            glGetAttachedShaders(
                handle_, sizeof...(stages),
                &count, attached_shaders.data());
            for(GLsizei i = 0; i < count; ++i)
                glDetachShader(handle_, attached_shaders[i]);
            glDeleteProgram(handle_);
        });

        glLinkProgram(handle_);

        GLint result;
        glGetProgramiv(handle_, GL_LINK_STATUS, &result);
        if(result != GL_TRUE)
        {
            GLint log_len;
            glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &log_len);
            std::vector<char> buf(log_len + 1);
            glGetProgramInfoLog(
                handle_, buf.size(), nullptr, buf.data());
            throw gl_exception_t(buf.data());
        }

        this->detach(stages...);
        guard.dismiss();
    }

    shader_t(shader_t &&other) noexcept
        : shader_t()
    {
        swap(other);
    }

    shader_t &operator=(shader_t &&other) noexcept
    {
        swap(other);
        return *this;
    }

    ~shader_t()
    {
        if(handle_)
            glDeleteProgram(handle_);
    }

    void swap(shader_t &other) noexcept
    {
        std::swap(handle_, other.handle_);
    }

    void bind() const noexcept
    {
        assert(handle_);
        glUseProgram(handle_);
    }

    void unbind() const noexcept
    {
        assert(handle_);
        AGZ_WHEN_DEBUG({
            GLint cur;
            glGetIntegerv(GL_CURRENT_PROGRAM, &cur);
            assert(cur == static_cast<GLint>(handle_));
        });
        glUseProgram(0);
    }

    template<typename Var>
    void set_uniform_unchecked(const char *name, const Var &val) const
    {
        const auto loc = glGetUniformLocation(handle_, name);
        if(loc < 0)
        {
            throw gl_exception_t(
                "unknown uniform variable: " + std::string(name));
        }
        detail::set_uniform(loc, val);
    }

    uniform_manager_t create_uniform_manager() const
    {
        std::map<std::string, uniform_variable_manager_t::uniform_var_info_t> vars;
        std::map<std::string, uniform_buffer_manager_t::uniform_buffer_info_t> bufs;

        // vars

        GLint num_vars;
        glGetProgramiv(handle_, GL_ACTIVE_UNIFORMS, &num_vars);

        GLint var_name_len;
        glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &var_name_len);

        for(GLint i = 0; i < num_vars; ++i)
        {
            std::vector<char> name_buf(var_name_len + 1, '\0');

            GLenum type; GLint size;
            glGetActiveUniform(
                handle_, i, name_buf.size(),
                nullptr, &size, &type, name_buf.data());

            const GLint loc = glGetUniformLocation(handle_, name_buf.data());

            vars.insert({
                std::string(name_buf.data()),
                {
                    loc, type
                }});
        }

        // bufs

        GLint num_bufs;
        glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_BLOCKS, &num_bufs);

        GLint buf_name_len;
        glGetProgramiv(
            handle_, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &buf_name_len);

        for(GLint i = 0; i < num_bufs; ++i)
        {
            GLsizei len;
            std::vector<char> name_buf(buf_name_len + 1, '\0');
            glGetActiveUniformBlockName(
                handle_, i, name_buf.size(), &len, name_buf.data());

            glUniformBlockBinding(handle_, i, i);

            bufs.insert({
                std::string(name_buf.data()),
                { static_cast<GLuint>(i) } });
        }

        return uniform_manager_t(
            uniform_variable_manager_t(vars),
            uniform_buffer_manager_t  (bufs));
    }

    template<typename Var>
    attribute_variable_t<Var> get_attribute_variable(const char *name)
    {
        assert(handle_);

        GLint loc = glGetAttribLocation(handle_, name);
        if(loc < 0)
            throw gl_exception_t(
                "invalid attrib variable name: " + std::string(name));

        const GLuint index = glGetProgramResourceIndex(
            handle_, GL_PROGRAM_INPUT, name);

        GLint size; GLenum type;
        glGetActiveAttrib(handle_, index, 0, nullptr, &size, &type, nullptr);
        if(type != detail::var_to_glsl_type<Var>::type)
            throw gl_exception_t(
                "unmatched attrib variable type of " + std::string(name));

        return attribute_variable_t<Var>(loc);
    }
};

AGZ_GL_END
