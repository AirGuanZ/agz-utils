#pragma once

#include <functional>

namespace agz::misc
{
    
inline size_t hash_combine(size_t a, size_t b)
{
    return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
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

template<typename A, typename...Others>
size_t hash(const A &a, const Others&...others)
{
    return hash_impl::hash(std::hash<A>()(a), others...);
}

} // namespace agz::misc
