#pragma once

#include <cassert>
#include <cstdlib>

#include "../misc/uncopyable.h"

namespace agz::alloc
{

/**
 * @brief 内存arena：多次分配，一次释放
 *
 * 内部进行预分配，提高分配速度；对过大的分配请求，则绕过预分配，直接走malloc
 */
class mem_arena_t : public misc::uncopyable_t
{
    struct chunk_t
    {
        chunk_t *next_chunk;
    };

    chunk_t *chunk_entry_;

    char *data_top_;
    size_t unused_bytes_;

    const size_t chunk_byte_size_;
    const size_t direct_alloc_threshold_;

    size_t total_chunk_bytes_;

    void alloc_new_chunk();

    void *alloc_direct_chunk(size_t bytes, size_t align);

public:

    /**
     * @param chunk_byte_size 每次预分配的字节数
     * @param direct_alloc_threshold alloc需要的空间超过此值时，将直接使用malloc
     */
    explicit mem_arena_t(size_t chunk_byte_size = 4096, size_t direct_alloc_threshold = 2048);

    ~mem_arena_t();

    /**
     * @param bytes 待分配的字节数
     * @param align 对齐要求，以字节为单位
     *
     * @exception 分配失败时抛std::bad_alloc，不会返回nullptr
     */
    void *alloc(size_t bytes, size_t align);

    /**
     * @brief 释放所有已分配的内存
     */
    void free();

    /**
     * @brief 总共使用了多少字节的堆内存
     *
     * 预分配但还未被使用的不计入其中
     */
    size_t used_bytes() const noexcept;
};

inline void mem_arena_t::alloc_new_chunk()
{
    char *new_chunk_datazone = static_cast<char*>(std::malloc(chunk_byte_size_));
    if(!new_chunk_datazone)
        throw std::bad_alloc();

    chunk_t *new_chunk = reinterpret_cast<chunk_t*>(new_chunk_datazone);
    new_chunk->next_chunk = chunk_entry_;
    chunk_entry_ = new_chunk;

    total_chunk_bytes_ += chunk_byte_size_;

    data_top_ = new_chunk_datazone + sizeof(chunk_t);
    unused_bytes_ = chunk_byte_size_ - sizeof(chunk_t);
}

inline void *mem_arena_t::alloc_direct_chunk(size_t bytes, size_t align)
{
    size_t total_bytes = sizeof(chunk_t) + bytes + align;
    char *new_chunk_datazone = static_cast<char*>(std::malloc(total_bytes));
    if(!new_chunk_datazone)
        throw std::bad_alloc();

    chunk_t *new_chunk = reinterpret_cast<chunk_t*>(new_chunk_datazone);
    if(chunk_entry_)
    {
        new_chunk->next_chunk = chunk_entry_->next_chunk;
        chunk_entry_->next_chunk = new_chunk;
    }
    else
    {
        assert(!data_top_ && !unused_bytes_);
        chunk_entry_ = new_chunk;
    }

    total_chunk_bytes_ += total_bytes;

    char *new_chunk_data_top = new_chunk_datazone + sizeof(chunk_t);
    size_t align_pad_bytes = reinterpret_cast<size_t>(new_chunk_data_top) % align;
    assert(sizeof(chunk_t) + align_pad_bytes + bytes <= total_bytes);

    return new_chunk_data_top + align_pad_bytes;
}

inline mem_arena_t::mem_arena_t(size_t chunk_byte_size, size_t direct_alloc_threshold)
    : chunk_byte_size_(chunk_byte_size), direct_alloc_threshold_(direct_alloc_threshold)
{
    chunk_entry_  = nullptr;
    data_top_     = nullptr;
    unused_bytes_ = 0;

    total_chunk_bytes_ = 0;
}

inline mem_arena_t::~mem_arena_t()
{
    free();
}

inline void *mem_arena_t::alloc(size_t bytes, size_t align)
{
    size_t align_pad_bytes = reinterpret_cast<size_t>(data_top_) % align;
    size_t total_bytes = bytes + align_pad_bytes;
    if(total_bytes > direct_alloc_threshold_)
        return alloc_direct_chunk(bytes, align);

    if(total_bytes > unused_bytes_)
    {
        alloc_new_chunk();
        return alloc(bytes, align);
    }

    char *ret = data_top_ + align_pad_bytes;
    data_top_ = ret + bytes;
    unused_bytes_ -= total_bytes;

    return ret;
}

inline void mem_arena_t::free()
{
    for(chunk_t *c = chunk_entry_, *nc; c; c = nc)
    {
        nc = c->next_chunk;
        std::free(c);
    }

    chunk_entry_  = nullptr;
    data_top_     = nullptr;
    unused_bytes_ = 0;

    total_chunk_bytes_ = 0;
}

inline size_t mem_arena_t::used_bytes() const noexcept
{
    return total_chunk_bytes_ - unused_bytes_;
}

} // namespace agz::alloc
