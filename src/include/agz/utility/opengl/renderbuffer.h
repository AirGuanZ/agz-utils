#pragma once

#include "./common.h"

namespace agz::gl
{
    
/**
 * @brief 对gl renderbuffer object的直接封装
 */
class renderbuffer_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即申请一个gl buffer name
     */
    explicit renderbuffer_t(bool init_handle = false)
    {
        if(init_handle)
            initialize_handle();
    }

    renderbuffer_t(renderbuffer_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    renderbuffer_t &operator=(renderbuffer_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~renderbuffer_t()
    {
        destroy();
    }

    /**
     * @brief 申请一个gl renderbuffer name
     *
     * 不得在已经持有renderbuffer name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateRenderbuffers(1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create renderbuffer object");
    }

    /**
     * @brief 若含有renderbuffer name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteRenderbuffers(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 设置该renderbuffer的大小和存储格式
     */
    void set_format(GLenum internal_format, GLsizei width, GLsizei height)
    {
        assert(handle_);
        glNamedRenderbufferStorage(handle_, internal_format, width, height);
    }
};

} // namespace agz::gl
