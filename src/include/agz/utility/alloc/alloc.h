#pragma once

#include <cassert>
#include <type_traits>

namespace agz::alloc
{

namespace alloc_impl
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
 * @brief 手动析构一个对象
 */
template<typename T>
void call_destructor(T &obj) noexcept
{
    alloc_impl::call_destructor_aux<T, std::is_trivially_destructible_v<T>>::call(obj);
}

/**
 * @brief 手动析构一串对象，顺序自后往前
 */
template<typename T>
void call_destructor(T *arr, size_t num) noexcept
{
    assert(arr);
    for(size_t i = num; i > 0; --i)
        call_destructor(arr[i - 1]);
}

} // namespace agz::alloc
