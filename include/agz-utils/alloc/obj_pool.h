#pragma once

#include <vector>

#include "../misc/uncopyable.h"
#include "./alloc.h"

namespace agz::alloc
{

/**
 * @brief �̶����Ͷ���أ�������ͷŶ���Ҫ�ֶ�����
 *
 * ��������ʱ�ѷ��������������������ᱻ�Զ����ã���ֻ�Ǹ��ȽϿ��new/delete����
 *
 * �̲߳���ȫ
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
     * @brief �ø������캯����������һ������
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
     * @brief �������ͷ�һ���ɸó��Ӵ����Ķ���
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
