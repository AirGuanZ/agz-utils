#pragma once

#include "./renderbuffer.h"
#include "./texture2d.h"

namespace agz::gl
{
    
/**
 * @brief 对gl framebuffer object的直接封装
 */
class framebuffer_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即申请一个gl framebuffer name
     */
    explicit framebuffer_t(bool init_handle = false)
    {
        if(init_handle)
            initialize_handle();
    }

    framebuffer_t(framebuffer_t &&move_from) noexcept
        : gl_object_t(move_from.handle_)
    {
        move_from.handle_ = 0;
    }

    framebuffer_t &operator=(framebuffer_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~framebuffer_t()
    {
        destroy();
    }

    /**
     * @brief 申请一个gl framebuffer name
     *
     * 不得在已经持有framebuffer name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateFramebuffers(1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create framebuffer object");
    }

    /**
     * @brief 若含有framebuffer name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteFramebuffers(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 将一个texture2d绑定到该framebuffer上
     */
    void attach(GLenum attachment, texture2d_t &tex)
    {
        assert(handle_);
        glNamedFramebufferTexture(handle_, attachment, tex.handle(), 0);
    }

    /**
     * @brief j将一个renderbuffer绑定到该framebuffer上
     */
    void attach(GLenum attachment, renderbuffer_t &buffer)
    {
        assert(handle_);
        glNamedFramebufferRenderbuffer(handle_, attachment, GL_RENDERBUFFER, buffer.handle());
    }

    /**
     * @brief 开始在该framebuffer上绘制
     */
    void bind() noexcept
    {
        assert(is_complete());
        glBindFramebuffer(GL_FRAMEBUFFER, handle_);
    }

    /**
     * @brief j结束在该framebuffer上的绘制
     */
    void unbind() noexcept
    {
        assert(is_complete());
        AGZ_WHEN_DEBUG({
            GLint cur;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &cur);
            assert(cur == GLint(handle_));
        });
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * @brief 检查该framebuffer是否已经可以用于绘制
     */
    bool is_complete() const noexcept
    {
        assert(handle_);
        return glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER)
            == GL_FRAMEBUFFER_COMPLETE;
    }
};

} // namespace agz::gl
