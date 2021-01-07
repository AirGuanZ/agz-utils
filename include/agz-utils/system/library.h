#pragma once

#include <cassert>
#include <stdexcept>
#include <string_view>

#include "../misc/uncopyable.h"

namespace agz::sys
{
 
/**
 * @brief 共享库native handle
 */
struct shared_lib_handle_t;

/***
 * @brief 动态加载一个共享库，失败时返回nullptr
 * 
 * err_msg用于输出错误消息，默认为nullptr
 */
shared_lib_handle_t *load_lib(
    const std::string &filename, std::string *err_msg = nullptr);

/**
 * @brief 释放一个共享库
 * 
 * assert(handle != nullptr)
 */
void destroy_lib(shared_lib_handle_t *handle);

using void_func_ptr = void(*)();

/**
 * @brief 从一个共享库中取得具有指定名字的函数地址
 * 
 * 失败时返回nullptr
 * 
 * err_msg用于输出错误消息，默认为nullptr
 */
void_func_ptr get_lib_process(
    shared_lib_handle_t *handle, const char *proc_name,
    std::string *err_msg = nullptr);

/**
 * @brief shared_lib_t相关异常
 */
class shared_lib_exception_t : public std::runtime_error
{
public:

    using runtime_error::runtime_error;
};

/**
 * @brief 对共享库的简单封装，内部使用引用计数管理native lib handle
 */
class shared_lib_t : public misc::uncopyable_t
{
    shared_lib_handle_t *lib_;

public:

    shared_lib_t() noexcept
        : lib_(nullptr)
    {

    }

    /**
     * @brief 构造时完成共享库加载
     * 
     * @exception shared_lib_exception_t 加载失败时抛出
     */
    explicit shared_lib_t(const std::string &filename)
    {
        std::string err;
        lib_ = load_lib(filename, &err);
        if(!lib_)
            throw shared_lib_exception_t(err);
    }

    shared_lib_t(shared_lib_t &&move_from) noexcept
        : shared_lib_t()
    {
        swap(move_from);
    }

    shared_lib_t &operator=(shared_lib_t &&move_from) noexcept
    {
        free();
        swap(move_from);
        return *this;
    }

    ~shared_lib_t()
    {
        free();
    }

    /**
     * @brief 和另一个共享库对象交换内容
     */
    void swap(shared_lib_t &swap_with) noexcept
    {
        auto t         = swap_with.lib_;
        swap_with.lib_ = lib_;
        lib_           = t;
    }

    /**
     * @brief 加载共享库
     * 
     * @exception shared_lib_exception_t 加载失败时抛出
     */
    void load(const std::string &filename)
    {
        free();
        std::string err;
        lib_ = load_lib(filename, &err);
        if(!lib_)
            throw shared_lib_exception_t(err);
    }

    /**
     * @brief 释放共享库
     * 
     * 若available() == false，则什么也不做
     */
    void free()
    {
        if(lib_)
            destroy_lib(lib_);
    }

    /**
     * @brief 是否包含了一个成功加载的共享库
     */
    bool available() const noexcept
    {
        return lib_ != nullptr;
    }

    /**
     * @brief 取得共享库中具有指定函数签名的函数地址
     * 
     * assert(available())
     * 
     * @exception shared_lib_exception_t 取得失败时抛出
     */
    template<typename T>
    T get_proc(const std::string &proc_name) const
    {
        assert(available());
        std::string err;
        auto ret = reinterpret_cast<T>(
            get_lib_process(lib_, proc_name.c_str(), &err));
        if(!ret)
            throw shared_lib_exception_t(err);
        return ret;
    }
};

} // namespace agz::sys
