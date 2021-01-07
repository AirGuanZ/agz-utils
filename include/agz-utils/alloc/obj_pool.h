#pragma once

#include <vector>

#include "../misc/uncopyable.h"
#include "./alloc.h"

namespace agz::alloc
{

/**
 * @brief 固定类型对象池，分配和释放都需要手动进行
 *
 * 池子销毁时已分配对象的析构函数并不会被自动调用，这只是个比较快的new/delete而已
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

    ~obj_pool_t()
    {
        for(auto c : chunks_)
            ::agz::alloc::aligned_free(c);
    }

    obj_pool_t(obj_pool_t &&other) noexcept
        : obj_pool_t(1)
    {
        this->swap(other);
    }

    obj_pool_t &operator=(obj_pool_t &&other) noexcept
    {
        this->swap(other);
        return *this;
    }

    void swap(obj_pool_t &other) noexcept
    {
        chunks_.swap(other.chunks_);
        std::swap(freelist_, other.freelist_);
        std::swap(chunk_size_, other.chunk_size_);
    }

    /**
     * @brief 用给定构造函数参数创建一个对象
     */
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

    /**
     * @brief 析构并释放一个由该池子创建的对象
     */
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
