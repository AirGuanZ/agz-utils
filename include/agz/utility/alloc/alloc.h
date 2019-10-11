#pragma once

#include <cassert>
#include <new>
#include <type_traits>

#include "../system/platform.h"

#ifndef AGZ_CC_MSVC
#include <cstdlib>
#endif

namespace agz::alloc
{

namespace impl
{
    
    template<typename T, bool NeedDtor>
    struct call_destructor_aux
    {
        static void call(T &obj) noexcept
        {
            obj.~T();
        }
    };

    template<typename T>
    struct call_destructor_aux<T, false>
    {
        static void call(T&) noexcept
        {
            // do nothing
        }
    };

}

/**
 * @brief 析构一个对象
 */
template<typename T>
void call_destructor(T &obj) noexcept
{
    impl::call_destructor_aux<T, std::is_trivially_destructible_v<T>>::call(obj);
}

/**
 * @brief 析构一串对象，顺序自后往前
 */
template<typename T>
void call_destructor(T *arr, size_t num) noexcept
{
    assert(arr);
    for(size_t i = num; i > 0; --i)
        call_destructor(arr[i - 1]);
}

/**
 * @brief 申请一块按要求对齐的内存，align得是2的整数次幂
 * 
 * 返回结果必须由agz::alloc::aligned_free释放
 * 
 * 绝不会返回nullptr，申请失败时会抛std::bad_alloc
 */
inline void *aligned_alloc(size_t byte_size, size_t align)
{
#ifdef AGZ_CC_MSVC

    void *ret = _aligned_malloc(byte_size, align);

#elif defined(AGZ_CC_GCC)

    void *ret = aligned_alloc(byte_size, align);

#else

    void *ret = std::aligned_alloc(byte_size, align);

#endif

    if(!ret)
        throw std::bad_alloc();
    return ret;
}

/**
 * @brief 释放一块由agz::alloc::aligned_alloc申请的内存
 * 
 * 参数不能是nullptr
 */
inline void aligned_free(void *ptr)
{
    assert(ptr);

#ifdef AGZ_CC_MSVC

    _aligned_free(ptr);

#else

    std::free(ptr);

#endif
}

} // namespace agz::alloc
