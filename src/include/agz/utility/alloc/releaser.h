#pragma once

#include <cassert>
#include <forward_list>
#include <memory>
#include <vector>

#include "../misc/scope_guard.h"
#include "../misc/uncopyable.h"

namespace agz::alloc
{
    
/**
 * @brief 用于统一管理对象生命周期
 * 
 * 可以将一个已有对象及其释放方式托管给releaset_t的实例，也可以让一个releaser_t实例创建并持有对象或对象数组
 */
class releaser_t : public misc::uncopyable_t
{
    struct destructor_t : uncopyable_t
    {
        virtual ~destructor_t() = default;
    };

    std::forward_list<destructor_t*> dtors_;

    template<typename T, typename D>
    struct deleter_caller_t : destructor_t
    {
        T *ptr;
        explicit deleter_caller_t(T *ptr) noexcept;
        ~deleter_caller_t();
    };

public:

    releaser_t() = default;

    releaser_t(releaser_t &&move_from)            noexcept;
    releaser_t &operator=(releaser_t &&move_from) noexcept;

    /** @brief 销毁时自动释放全部持有的对象 */
    ~releaser_t();

    /**
     * @brief 托管一个已有对象
     * 
     * @tparam D 删除器，默认使用std::default_delete<T>
     */
    template<typename T, typename D = std::default_delete<T>>
    void add(T *ptr);

    /**
     * @brief 托管一个已有对象数组
     * 
     * @tparam D 删除器，默认使用std::default_delete<T[]>
     */
    template<typename T, typename D = std::default_delete<T[]>>
    void add_array(T *ptr);

    /**
     * @brief 创建指定类型的对象并自动托管
     */
    template<typename T, typename...Args>
    T *create(Args&&...args);

    /**
     * @brief 创建指定类型的对象数组并自动托管
     * 
     * @param n 对象数量
     * @param args 构造函数参数，参数转发时右值会被转为左值引用
     */
    template<typename T, typename...Args>
    T *create_array(size_t n, Args&&...args);

    /**
     * @brief 释放全部持有的对象
     */
    void release();
};

template<typename T, typename D>
releaser_t::deleter_caller_t<T, D>::deleter_caller_t(T *ptr) noexcept
    : ptr(ptr)
{
    assert(ptr);
}

template<typename T, typename D>
releaser_t::deleter_caller_t<T, D>::~deleter_caller_t()
{
    D()(ptr);
}

inline releaser_t::releaser_t(releaser_t &&move_from) noexcept
    : dtors_(std::move(move_from.dtors_))
{
    
}

inline releaser_t &releaser_t::operator=(releaser_t &&move_from) noexcept
{
    dtors_ = std::move(move_from.dtors_);
    return *this;
}

inline releaser_t::~releaser_t()
{
    release();
}

template<typename T, typename D>
void releaser_t::add(T *ptr)
{
    dtors_.push_front(nullptr);
    destructor_t *dtor;
    try
    {
        dtor = new deleter_caller_t<T, D>(ptr);
    }
    catch(...)
    {
        dtors_.pop_front();
        throw;
    }
    dtors_.front() = dtor;
}

template<typename T, typename D>
void releaser_t::add_array(T* ptr)
{
    add<T, D>(ptr);
}

template<typename T, typename...Args>
T *releaser_t::create(Args&&...args)
{
    auto ret = new T(std::forward<Args>(args)...);
    misc::scope_guard_t guard([=] { delete ret; });

    add(ret);

    guard.dismiss();
    return ret;
}

template<typename T, typename...Args>
T *releaser_t::create_array(size_t n, Args&&...args)
{
    struct vector_holder_t : destructor_t
    {
        std::vector<T> vec;
    };

    assert(n);

    std::vector<T> ret;
    ret.reserve(n);
    for(size_t i = 0; i < n; ++i)
        ret.emplace_back(static_cast<Args&>(args)...);

    dtors_.push_front(nullptr);
    vector_holder_t *dtor;
    try
    {
        dtor = new vector_holder_t;
        dtor->vec = std::move(ret);
    }
    catch(...)
    {
        dtors_.pop_front();
        throw;
    }
    dtors_.front() = dtor;

    return dtor->vec.data();
}

inline void releaser_t::release()
{
    for(auto dtor : dtors_)
    {
        assert(dtor);
        delete dtor;
    }
    dtors_.clear();
}

using default_arena_t = releaser_t;

} // namespace agz::alloc
