#pragma once

#include <cassert>

#include "./common.h"

namespace agz::gl
{
    
/**
 * @brief 对opengl buffer object的封装
 * 
 * 一般不直接拿来用，而是使用基于它实现的各类细化的buffer类型
 */
class buffer_t : public gl_object_t
{
public:

    /**
     * @param init_handle 是否立即申请一个gl buffer name
     */
    explicit buffer_t(bool init_handle = false)
        : gl_object_t(0)
    {
        if(init_handle)
            initialize_handle();
    }

    buffer_t(buffer_t &&move_from) noexcept
        : gl_object_t(move_from.handle())
    {
        move_from.handle_ = 0;
    }

    buffer_t &operator=(buffer_t &&move_from) noexcept
    {
        destroy();
        std::swap(handle_, move_from.handle_);
        return *this;
    }

    ~buffer_t()
    {
        destroy();
    }

    /**
     * @brief 申请一个gl buffer name
     * 
     * 不得在已经持有buffer name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        assert(!handle_);
        glCreateBuffers(1, &handle_);
        if(!handle_)
            throw opengl_exception_t("failed to create opengl buffer");
    }

    /**
     * @brief 若含有buffer name，将其标记为删除并释放所有权
     */
    void destroy()
    {
        if(handle_)
        {
            glDeleteBuffers(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 初始化buffer内部的数据
     * 
     * 若已有其他数据，则释放之并重新申请
     * 
     * @param data 初始化数据指针
     * @param byte_size 初始化数据的字节数
     * @param usage GL_{STREAM|STATIC|DYNAMIC}_{DRAW|READ|COPY}
     */
    void reinitialize_data(
        const void *data, size_t byte_size, GLenum usage) noexcept
    {
        assert(handle_);
        glNamedBufferData(handle_, GLsizeiptr(byte_size), data, usage);
    }

    /**
     * @brief 设置buffer的部分内容
     * 
     * @param subdata 待写入buffer的数据
     * @param byte_offset 要写入的数据距buffer首字节的距离
     * @param byte_size 要写入的数据长度
     */
    void set_data(
        const void *subdata, size_t byte_offset, size_t byte_size) noexcept
    {
        assert(handle_);
        glNamedBufferSubData(
            handle_, GLintptr(byte_offset), GLsizeiptr(byte_size), subdata);
    }
};

/**
 * @brief 对opengl vertex buffer的封装
 * 
 * @tparma Var 单个顶点的数据类型，可以是vec3这样的基本元素，也可以是更复杂的struct
 */
template<typename Var>
class vertex_buffer_t : public buffer_t
{
    size_t vertex_count_;

public:

    /**
     * @param init_handle 是否立即创建一个gl buffer name
     */
    explicit vertex_buffer_t(bool init_handle = false) noexcept
        : buffer_t(init_handle), vertex_count_(0)
    {
        
    }

    /**
     * @brief 立即创建一个gl buffer name并用给定数据初始化之
     * 
     * @param data 用于初始化的数据指针
     * @param vertex_count 数据所包含的顶点数
     * @param usage GL_{STREAM|STATIC|DYNAMIC}_{DRAW|READ|COPY}
     */
    vertex_buffer_t(const Var *data, size_t vertex_count, GLenum usage)
        : buffer_t(true)
    {
        reinitialize_data(data, vertex_count, usage);
    }

    vertex_buffer_t(vertex_buffer_t<Var> &&move_from) noexcept
        : buffer_t(std::move(static_cast<buffer_t&>(move_from))),
          vertex_count_(move_from.vertex_count_)
    {
        move_from.vertex_count_ = 0;
    }

    vertex_buffer_t<Var> &operator=(vertex_buffer_t<Var> &&move_from) noexcept
    {
        static_cast<buffer_t&>(*this) =
            std::move(static_cast<buffer_t&>(move_from));
        vertex_count_ = move_from.vertex_count_;
        move_from.vertex_count_ = 0;
        return *this;
    }

    ~vertex_buffer_t() = default;

    /**
     * @brief 申请一个gl buffer name
     *
     * 不得在已经持有buffer name所有权的情况下再次调用
     */
    void initialize_handle()
    {
        buffer_t::initialize_handle();
    }

    /**
     * @brief 若含有buffer name，将其标记为删除并释放所有权
     */
    void destroy() noexcept
    {
        buffer_t::destroy();
        vertex_count_ = 0;
    }

    /**
     * @brief 初始化buffer内的数据
     * 
     * 若已有其他数据，释放之并重新申请空间
     *
     * @param data 用于初始化的数据指针
     * @param vertex_count 数据所包含的顶点数
     * @param usage GL_{STREAM|STATIC|DYNAMIC}_{DRAW|READ|COPY}
     */
    void reinitialize_data(const Var *data, size_t vertex_count, GLenum usage)
    {
        assert(vertex_count);
        size_t byte_size = sizeof(Var) * vertex_count;
        buffer_t::reinitialize_data(data, byte_size, usage);
        vertex_count_ = vertex_count;
    }

    /**
     * @brief 设置buffer中的部分数据
     * 
     * @param subdata 待写入buffer的数据
     * @param vertex_offset 要设置的内容距buffer首元素有多少个顶点
     * @param vertex_count 要写入多少个顶点数据
     */
    void set_data(const Var *subdata, size_t vertex_offset, size_t vertex_count)
    {
        assert(vertex_count);
        size_t byte_offset = sizeof(Var) * vertex_offset;
        size_t byte_size   = sizeof(Var) * vertex_count;
        buffer_t::set_data(subdata, byte_offset, byte_size);
    }

    /**
     * @brief 取得buffer中的顶点数量
     */
    size_t vertex_count() const noexcept
    {
        return vertex_count_;
    }
};

/**
 * @brief 对适用于std140 layout的uniform buffer object的封装
 * 
 * @tparam Block 具有uniform block相对应layout的C++类型
 */
template<typename Block>
class std140_uniform_block_buffer_t : public buffer_t
{
public:

    /**
     * @param init_handle 是否立即创建一个gl buffer name
     */
    explicit std140_uniform_block_buffer_t(bool init_handle = false)
        : buffer_t(init_handle)
    {
        
    }

    /**
     * @brief 立即创建一个gl buffer name并用给定数据初始化之
     *
     * @param data 用于初始化的数据指针
     * @param usage GL_{STREAM|STATIC|DYNAMIC}_{DRAW|READ|COPY}
     */
    std140_uniform_block_buffer_t(const Block *data, GLenum usage)
        : std140_uniform_block_buffer_t(true)
    {
        reinitialize_data(data, usage);
    }

    std140_uniform_block_buffer_t(
        std140_uniform_block_buffer_t<Block> &&move_from) noexcept
        : buffer_t(std::move(static_cast<buffer_t&>(move_from)))
    {
        
    }

    std140_uniform_block_buffer_t<Block> &operator=(
        std140_uniform_block_buffer_t<Block> &&move_from) noexcept
    {
        static_cast<buffer_t&>(*this) =
            std::move(static_cast<buffer_t&>(move_from));
        return *this;
    }

    ~std140_uniform_block_buffer_t() = default;

    /**
     * @brief 申请一个gl buffer name
     *
     * 不得在已经持有buffer name所有权的情况下再次调用
     */
    void initialize_handle() noexcept
    {
        buffer_t::initialize_handle();
    }

    /**
     * @brief 若含有buffer name，将其标记为删除并释放所有权
     */
    void destroy() noexcept
    {
        buffer_t::destroy();
    }

    /**
     * @brief 初始化buffer内的数据
     *
     * 若已有其他数据，释放之并重新申请空间
     *
     * @param data 用于初始化的数据指针
     * @param usage GL_{STREAM|STATIC|DYNAMIC}_{DRAW|READ|COPY}
     */
    void reinitialize_data(const Block *data, GLenum usage) noexcept
    {
        buffer_t::reinitialize_data(data, sizeof(Block), usage);
    }

    /**
     * @brief 设置buffer中的部分数据
     *
     * @param subdata 待写入buffer的数据
     * @param byte_offset 要设置的内容距buffer首元素有多少个字节
     * @param byte_size 要写入多少个字节数据
     */
    void set_data(
        const void *subdata, size_t byte_offset, size_t byte_size) noexcept
    {
        buffer_t::set_data(subdata, byte_offset, byte_size);
    }

    /**
     * @brief 设置整个buffer中的数据
     */
    void set_data(const Block *data) noexcept
    {
        buffer_t::set_data(data, 0, sizeof(Block));
    }

    /**
     * @brief 设置buffer中特定成员的值
     * 
     * @param data 待写入buffer的数据
     * @param mem 要设置的成员的指针
     */
    template<typename M>
    void set_data(const M *data, M (Block::*mem)) noexcept
    {
        buffer_t::set_data(data, byte_offset(mem), sizeof(M));
    }

    /**
     * @brief 绑定到指定的uniform buffer binding point
     */
    void bind(GLuint binding_point) const noexcept
    {
        assert(handle_);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, handle_);
    }
};

} // namespace agz::gl
