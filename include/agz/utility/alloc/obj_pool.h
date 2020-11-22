#pragma once

#include <vector>

#include "../misc/uncopyable.h"
#include "./alloc.h"

namespace agz::alloc
{

/**
 * @brief 固定类型对象池，分配和释放都需要手动进行
 *
 * 线程不安全
 */
template<typename T>
class obj_pool_t : public misc::uncopyable_t
{
    static constexpr size_t static_max(size_t a, size_t b) noexcept
    {
        return a > b ? a : b;
    }

    static constexpr size_t STORAGE_ALIGN =
        static_max(alignof(T), alignof(void *));

    static constexpr size_t STORAGE_RAW_SIZE =
        static_max(sizeof(T), sizeof(void *));

    static constexpr size_t STORAGE_SIZE =
        (STORAGE_RAW_SIZE + STORAGE_ALIGN - 1) / STORAGE_ALIGN * STORAGE_ALIGN;

    static_assert(STORAGE_SIZE > STORAGE_ALIGN);
    static_assert(STORAGE_SIZE % STORAGE_ALIGN == 0);

    std::vector<char *> chunks_;

    size_t chunk_size_;
    char *freelist_;

    void alloc_chunk()
    {
        char *new_chunk = static_cast<char *>(
            ::agz::alloc::aligned_alloc(chunk_size_, STORAGE_ALIGN));
        char *chunk_end = new_chunk + chunk_size_;

        for(char *entry = new_chunk; entry < chunk_end; entry += STORAGE_SIZE)
        {
            *reinterpret_cast<char **>(entry) = freelist_;
            freelist_ = entry;
        }

        chunks_.push_back(new_chunk);
    }

public:

    explicit obj_pool_t(size_t objs_per_chunk) noexcept
        : chunk_size_(STORAGE_SIZE * objs_per_chunk), freelist_(nullptr)
    {
        
    }

    template<typename...Args>
    T *create(Args &&...args)
    {
        if(!freelist_)
            alloc_chunk();
        assert(freelist_);

        T *obj = reinterpret_cast<T *>(freelist_);
        char *new_freelist = *reinterpret_cast<char **>(freelist_);

        try
        {

            (void)new(obj) T(std::forward<Args>(args)...);
        }
        catch(...)
        {
            *reinterpret_cast<char **>(freelist_) = new_freelist;
            throw;
        }

        freelist_ = new_freelist;

        return obj;
    }

    void destroy(T *obj) noexcept
    {
        assert(obj);
        obj->~T();

        char *new_freelist = reinterpret_cast<char *>(obj);
        *reinterpret_cast<char **>(new_freelist) = freelist_;
        freelist_ = new_freelist;
    }
};

} // namespace agz::alloc
