#pragma once

#include "../misc/uncopyable.h"

namespace agz::alloc
{
    
/**
 * @brief 小对象快速分配器
 */
class arena_t : public misc::uncopyable_t
{
    struct chunk_head_t
    {
        chunk_head_t *next;
    };

    static_assert(alignof(chunk_head_t) == alignof(chunk_head_t*));

    class destructor_t
    {
        destructor_t *next_;

    public:

        explicit destructor_t(destructor_t *next) noexcept
            : next_(next)
        {
            
        }

        virtual ~destructor_t() noexcept
        {
            if(next_)
                next_->~destructor_t();
        }
    };

    chunk_head_t *chunk_entry_;
    char *current_chunk_top_;
    size_t current_chunk_rest_;

    destructor_t *destructor_entry_;

    const size_t chunk_size_;

    size_t used_bytes_;
    size_t occupied_bytes_;

    void alloc_chunk();

public:

    /**
     * @param chunk_size 区块字节大小
     * 
     * 区块是分配器一次从堆中分配的最小单位
     */
    explicit arena_t(size_t chunk_size = 1024) noexcept;

    ~arena_t();

    /**
     * @brief 已被对象占据的字节数
     */
    size_t used_bytes() const noexcept;

    /**
     * @brief 整个内存池已消耗的字节数
     */
    size_t occupied_bytes() const noexcept;

    /**
     * @brief 清空所有已分配对象，其析构函数按构造的逆序调用
     */
    void clear();

    /**
     * @brief 分配用于n个T类型对象的存储区域
     * 
     * 若需要内存块过大，直接从堆分配；否则从内存池内的chunk中分配。
     * 
     * 分配器会负责管理这块内存的生命周期，但不负责上面任何对象的析构
     */
    template<typename T>
    char *alloc(size_t n);

    /**
     * @brief 创建一个指定类型的对象
     */
    template<typename T, typename...Args>
    T *create(Args&&...args);

    /**
     * @brief 创建一个指定类型的对象数组
     */
    template<typename T, typename...Args>
    T *create_array(size_t n, Args&&...args);
};

} // namespace agz::alloc

#include "impl/arena.inl"
