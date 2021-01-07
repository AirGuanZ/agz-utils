#pragma once

#include <functional>

#ifndef __CUDACC__

namespace agz {
namespace misc {

    
inline size_t hash_combine(size_t a, size_t b)
{
    if constexpr(sizeof(size_t) == 4)
        return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
    else
        return a ^ (b + 0x9e3779b97f4a7c15 + (a << 6) + (a >> 2));
}

namespace hash_impl
{

    inline size_t hash(size_t h)
    {
        return h;
    }

    template<typename A, typename...Others>
    size_t hash(size_t h, const A &a, const Others&...others)
    {
        h = ::agz::misc::hash_combine(h, std::hash<A>()(a));
        return hash(h, others...);
    }

} // namespace hash_impl

/**
 * @brief 将一个或多个对象hash到一个size_t上
 * 
 * 要求输入参数都可以用std::hash求其hash值
 */
template<typename A, typename...Others>
size_t hash(const A &a, const Others&...others)
{
    return hash_impl::hash(std::hash<A>()(a), others...);
}

} } // namespace agz::misc

#endif // #ifndef __CUDACC__
