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
 * @brief ����һ������
 */
template<typename T>
void call_destructor(T &obj) noexcept
{
    impl::call_destructor_aux<T, std::is_trivially_destructible_v<T>>::call(obj);
}

/**
 * @brief ����һ������˳���Ժ���ǰ
 */
template<typename T>
void call_destructor(T *arr, size_t num) noexcept
{
    assert(arr);
    for(size_t i = num; i > 0; --i)
        call_destructor(arr[i - 1]);
}

/**
 * @brief ����һ�鰴Ҫ�������ڴ棬align����2����������
 * 
 * ���ؽ��������agz::alloc::aligned_free�ͷ�
 * 
 * �����᷵��nullptr������ʧ��ʱ����std::bad_alloc
 */
inline void *aligned_alloc(size_t byte_size, size_t align)
{
#ifdef AGZ_CC_MSVC

    void *ret = _aligned_malloc(byte_size, align);

#else

    void *ret = std::aligned_alloc(byte_size, align);

#endif

    if(!ret)
        throw std::bad_alloc();
    return ret;
}

/**
 * @brief �ͷ�һ����agz::alloc::aligned_alloc������ڴ�
 * 
 * ����������nullptr
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
