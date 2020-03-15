#pragma once

#include <type_traits>

#ifdef __GNUC__
#include <endian.h>
#endif

#include "../system/platform.h"

namespace agz::misc
{

enum class endian_type
{
    little = 0,
    big = 1,

#if defined(__GNUC__)
    #if __BYTE_ORDER == __BIG_ENDIAN
        local = big
    #elif __BYTE_ORDER == __LITTLE_ENDIAN
        local = little
    #else
        #error "platform with unknown endian"
    #endif
#elif defined(_MSC_VER)
    #if defined(AGZ_OS_WIN32)
        local = little
    #else
        #error "platform with unknown endian"
    #endif
#else
    #error "platform with unknown endian"
#endif
};

template<endian_type From, endian_type To, typename T>
T convert_endian(T value) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);

    if constexpr(From == To)
        return value;

    if constexpr(sizeof(T) == 1)
        return value;

    T result;
    unsigned char *to_ptr   = reinterpret_cast<unsigned char*>(&result);
    unsigned char *from_ptr = reinterpret_cast<unsigned char*>(&value)
                            + sizeof(value) - 1;

    if constexpr(sizeof(T) == 2)
    {
        to_ptr[0] = from_ptr[1];
        to_ptr[1] = from_ptr[0];
        return result;
    }

    if constexpr(sizeof(T) == 4)
    {
        to_ptr[0] = from_ptr[3];
        to_ptr[1] = from_ptr[2];
        to_ptr[2] = from_ptr[1];
        to_ptr[3] = from_ptr[0];
        return result;
    }

    if constexpr(sizeof(T) == 8)
    {
        to_ptr[0] = from_ptr[7];
        to_ptr[1] = from_ptr[6];
        to_ptr[2] = from_ptr[5];
        to_ptr[3] = from_ptr[4];
        to_ptr[4] = from_ptr[3];
        to_ptr[5] = from_ptr[2];
        to_ptr[6] = from_ptr[1];
        to_ptr[7] = from_ptr[0];
        return result;
    }
    else
    {
        for(size_t byte_idx = 0; byte_idx < sizeof(value); ++byte_idx)
            *to_ptr++ = *from_ptr--;
        return result;
    }
}

template<endian_type To, typename T>
T local_endian_to(T value) noexcept
{
    return convert_endian<endian_type::local, To, T>(value);
}

template<endian_type From, typename T>
T to_local_endian(T value) noexcept
{
    return convert_endian<From, endian_type::local, T>(value);
}

} // namespace agz::misc
