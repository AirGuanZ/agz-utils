#pragma once

#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>

#include "../alloc.h"

namespace agz::alloc
{
    
inline void arena_t::alloc_chunk()
{
    char *new_chunk = static_cast<char*>(std::malloc(chunk_size_));
    if(!new_chunk)
        throw std::bad_alloc();

    auto head = reinterpret_cast<chunk_head_t*>(new_chunk);
    head->next = chunk_entry_;
    chunk_entry_ = head;

    current_chunk_top_  = new_chunk + sizeof(chunk_head_t);
    current_chunk_rest_ = chunk_size_ - sizeof(chunk_head_t);

    occupied_bytes_ += chunk_size_;
}

inline arena_t::arena_t(size_t chunk_size) noexcept
    : chunk_entry_(nullptr), current_chunk_top_(nullptr), current_chunk_rest_(0),
      destructor_entry_(nullptr),
      chunk_size_(chunk_size),
      used_bytes_(0), occupied_bytes_(0)
{
    assert(chunk_size >= 64);
}

inline arena_t::~arena_t()
{
    clear();
}

inline size_t arena_t::used_bytes() const noexcept
{
    return used_bytes_;
}

inline size_t arena_t::occupied_bytes() const noexcept
{
    return occupied_bytes_;
}

inline void arena_t::clear()
{
    if(destructor_entry_)
        destructor_entry_->~destructor_t();

    for(chunk_head_t *chk = chunk_entry_, *next; chk; chk = next)
    {
        next = chk->next;
        std::free(chk);
    }

    chunk_entry_        = nullptr;
    current_chunk_top_  = nullptr;
    current_chunk_rest_ = 0;

    destructor_entry_ = nullptr;

    used_bytes_     = 0;
    occupied_bytes_ = 0;
}

template<typename T>
char *arena_t::alloc(size_t n)
{
    class free_destructor_t : public destructor_t
    {
        T *ptr_;
        size_t n_;

    public:

        free_destructor_t(destructor_t *next, T *ptr, size_t n) noexcept
            : destructor_t(next), ptr_(ptr), n_(n)
        {
            
        }

        ~free_destructor_t()
        {
            std::allocator<T>().deallocate(ptr_, n);
        }
    };

    size_t byte_size = sizeof(T) * n;

    // 对过大的byte_size，直接std::allocate走起
    if(byte_size + (sizeof(chunk_head_t) + alignof(T)) > chunk_size_)
    {
        auto ptr = std::allocator<T>().allocate(n);
        free_destructor_t *destructor;
        try
        {
            destructor = alloc<free_destructor_t>(1);
        }
        catch(...)
        {
            std::allocator<T>().deallocate(ptr, n);
            throw;
        }
        new(destructor) free_destructor_t(destructor_entry_, ptr, n);
        destructor_entry_ = destructor;

        used_bytes_     += byte_size;
        occupied_bytes_ += byte_size;

        return ptr;
    }

    constexpr size_t ALIGN = alignof(T);
    constexpr size_t ALIGN_MASK = ~(ALIGN - 1);

    size_t top = reinterpret_cast<size_t>(current_chunk_top_);
    size_t aligned_top = (top + (ALIGN - 1)) & ALIGN_MASK;

    auto aligned_chunk_top = reinterpret_cast<char*>(aligned_top);
    size_t padding_bytes   = aligned_chunk_top - current_chunk_top_;
    size_t full_byte_size  = padding_bytes + byte_size;
    if(full_byte_size > current_chunk_rest_)
    {
        alloc_chunk();
        return alloc<T>(n);
    }

    current_chunk_top_ = aligned_chunk_top + byte_size;
    current_chunk_rest_ -= full_byte_size;
    used_bytes_ += full_byte_size;

    return aligned_chunk_top;
}

template<typename T, typename...Args>
T *arena_t::create(Args&&...args)
{
    class single_destructor_t : public destructor_t
    {
        T *ptr_;

    public:

        single_destructor_t(destructor_t *next, T *ptr) noexcept
            : destructor_t(next), ptr_(ptr)
        {
            
        }

        ~single_destructor_t()
        {
            call_destructor(*ptr_);
        }
    };

    auto ret = alloc<T>(1);
    new(ret) T(std::forward<Args>(args)...);

    try
    {
        auto dtor = alloc<single_destructor_t>(1);
        new(dtor) single_destructor_t(destructor_entry_, ret);
        destructor_entry_ = dtor;
    }
    catch(...)
    {
        call_destructor(*ret);
        throw;
    }

    return ret;
}

template<typename T, typename ... Args>
T *arena_t::create_array(size_t n, Args&&...args)
{
    class array_destructor_t : public destructor_t
    {
        T *ptr_;
        size_t n_;

    public:

        array_destructor_t(destructor_t *next, T *ptr, size_t n) noexcept
            : destructor_t(next), ptr_(ptr), n_(n)
        {

        }

        ~array_destructor_t()
        {
            call_destructor(ptr_, n);
        }
    };

    auto ret = alloc<T>(n);

    size_t constructed_count = 0;
    try
    {
        for(size_t i = 0; i != n; ++i)
        {
            new(ret + i) T(static_cast<Args&>(args)...);
            constructed_count = i;
        }
    }
    catch(...)
    {
        call_destructor(ret, constructed_count);
    }

    try
    {
        auto dtor = alloc<array_destructor_t>(1);
        new(dtor) array_destructor_t(destructor_entry_, ret, n);
        destructor_entry_ = dtor;
    }
    catch(...)
    {
        call_destructor(*ret);
        throw;
    }

    return ret;
}

} // namespace agz::alloc
