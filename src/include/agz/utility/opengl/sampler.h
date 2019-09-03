#pragma once

#include "./common.h"

namespace agz::gl
{
    
/**
 * @brief 对gl sampler object的直接封装
 */
class sampler_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即申请一个gl sampler name
     */
    explicit sampler_t(bool init_handle = false)
    {
        if(init_handle)
            initialize_handle();
    }

    sampler_t(sampler_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    sampler_t &operator=(sampler_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~sampler_t()
    {
        destroy();
    }

    /**
     * @brief 申请一个gl sampler name
     *
     * 不得在已经持有sampler name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateSamplers(1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create sampler object");
    }

    /**
     * @brief 若含有sampler name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteSamplers(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 设置sampler参数
     */
    void set_param(GLenum param_name, GLint param_value)
    {
        assert(handle_);
        glSamplerParameteri(handle_, param_name, param_value);
    }

    /**
     * @brief 将该sampler绑定到指定texture unit
     */
    void bind(GLuint texture_unit) const
    {
        assert(handle_);
        glBindSampler(texture_unit, handle_);
    }

    /**
     * @brief 将该sampler从指定texture unit解绑
     */
    void unbind(GLuint texture_uint) const
    {
        assert(handle_);
        glBindSampler(texture_uint, 0);
    }
};

} // namespace agz::gl
