#pragma once

#include <type_traits>

#include "../common/common.h"
#include "../misc/scope_guard.h"
#include "../misc/uncopyable.h"
#include "./mem_arena.h"
#include "./releaser.h"

namespace agz::alloc
{

/**
 * @brief 固定类型对象的快速分配器，多次分配，一次释放
 *
 * 线程不安全
 */
template<typename T>
class fixed_obj_arena_t : public misc::uncopyable_t
{
    struct chunk_t
    {
        chunk_t *next_chunk;
    };

    chunk_t *chunk_entry_;

    T *data_top_;
    size_t remain_count_;

    size_t chunk_obj_count_;
    size_t chunk_start_to_ptr_offset_;
    size_t chunk_total_bytes_;

    void alloc_new_chunk();

    void free_chunk(chunk_t *ptr, size_t allocated_count);

public:

    /** 用起来这么简单的东西还需要参数注释吗 = = */
    explicit fixed_obj_arena_t(size_t chunk_obj_count = 1024);

    ~fixed_obj_arena_t();

    template<typename...Args>
    T *create(Args &&...args);

    void release();
};

/**
 * @brief 对象arena，多次创建，一次析构、释放
 *
 * 基于mem_pool_t实现
 *
 * 线程不安全
 */
class obj_arena_t : public misc::uncopyable_t
{
    struct obj_desctructor_t
    {
        obj_desctructor_t *next = nullptr;

        virtual void destruct() noexcept = 0;

        virtual ~obj_desctructor_t() = default;
    };

    template<typename T>
    struct obj_descructor_impl_t : obj_desctructor_t
    {
        T *ptr = nullptr;

        explicit obj_descructor_impl_t(T *ptr) noexcept : ptr(ptr) { }

        void destruct() noexcept override { ptr->~T(); }
    };

    mem_arena_t mem_arena_;

    obj_desctructor_t *destructor_entry_;

    template<typename T>
    void add_destructor(T *obj);

public:

    /**
     * @param mem_pool_chunk_size 每次预分配的字节数
     * @param direct_alloc_threshold alloc需要的空间超过此值时，将直接使用malloc
     */
    explicit obj_arena_t(
        size_t mem_pool_chunk_size = 4096, size_t direct_alloc_threshold = 2048);

    ~obj_arena_t();

    /**
     * @brief 创建指定类型的对象，参数为构造函数参数
     */
    template<typename T, typename...Args>
    T *create(Args &&...args);

    /**
     * @brief 创建指定类型的对象，但释放时不进行析构
     */
    template<typename T, typename...Args>
    T *create_nodestruct(Args &&...args);

    /**
     * @brief 析构并销毁所有之前创建的对象
     */
    void release();

    /**
     * @brief 已创建的对象总共使用了多少字节的堆内存
     */
    size_t used_bytes() const noexcept;
};

template<typename T>
void fixed_obj_arena_t<T>::alloc_new_chunk()
{
    assert(!remain_count_);

    char *new_chunk_start = reinterpret_cast<char*>(aligned_alloc(
        chunk_total_bytes_, alignof(T)));

    chunk_t *ptr = reinterpret_cast<chunk_t *>(
        new_chunk_start + chunk_start_to_ptr_offset_);
    ptr->next_chunk = chunk_entry_;

    chunk_entry_  = ptr;
    data_top_     = reinterpret_cast<T*>(new_chunk_start);
    remain_count_ = chunk_obj_count_;
}

template<typename T>
void fixed_obj_arena_t<T>::free_chunk(chunk_t *ptr, size_t allocated_count)
{
    auto raw_chunk = reinterpret_cast<char *>(ptr) - chunk_start_to_ptr_offset_;
    if constexpr(!std::is_trivially_destructible_v<T>)
    {
        auto data = reinterpret_cast<T *>(raw_chunk);
        std::destroy(data, data + allocated_count);
    }
    ::agz::alloc::aligned_free(raw_chunk);
}

template<typename T>
fixed_obj_arena_t<T>::fixed_obj_arena_t(size_t chunk_obj_count)
{
    chunk_entry_  = nullptr;
    data_top_     = nullptr;
    remain_count_ = 0;

    const size_t data_size = sizeof(T) * chunk_obj_count;

    chunk_obj_count_           = chunk_obj_count;
    chunk_start_to_ptr_offset_ = upalign_to(data_size, alignof(chunk_t));
    chunk_total_bytes_         = chunk_start_to_ptr_offset_ + sizeof(chunk_t);
}

template<typename T>
fixed_obj_arena_t<T>::~fixed_obj_arena_t()
{
    release();
}

template<typename T>
template<typename ... Args>
T *fixed_obj_arena_t<T>::create(Args &&... args)
{
    if(!remain_count_)
        alloc_new_chunk();
    assert(remain_count_);

    T *ret = new(data_top_) T(std::forward<Args>(args)...);
    ++data_top_;
    --remain_count_;

    return ret;
}

template<typename T>
void fixed_obj_arena_t<T>::release()
{
    if(!chunk_entry_)
        return;

    chunk_t *tchk = chunk_entry_->next_chunk;
    this->free_chunk(chunk_entry_, chunk_obj_count_ - remain_count_);

    for(chunk_t *chunk = tchk; chunk; chunk = tchk)
    {
        tchk = chunk->next_chunk;
        this->free_chunk(chunk, chunk_obj_count_);
    }

    chunk_entry_  = nullptr;
    data_top_     = nullptr;
    remain_count_ = 0;
}

template<typename T>
void obj_arena_t::add_destructor(T *obj)
{
    if constexpr(std::is_trivially_destructible_v<T>)
        return;

    void *destructor_mem = mem_arena_.alloc(
        sizeof(obj_descructor_impl_t<T>), alignof(obj_descructor_impl_t<T>));
    auto destructor = new(destructor_mem) obj_descructor_impl_t<T>(obj);

    destructor->next = destructor_entry_;
    destructor_entry_ = destructor;
}

inline obj_arena_t::obj_arena_t(
    size_t mem_pool_chunk_size, size_t direct_alloc_threshold)
    : mem_arena_(mem_pool_chunk_size, direct_alloc_threshold),
      destructor_entry_(nullptr)
{
    
}

inline obj_arena_t::~obj_arena_t()
{
    release();
}

template<typename T, typename...Args>
T *obj_arena_t::create(Args&&...args)
{
    void *obj_mem = mem_arena_.alloc(sizeof(T), alignof(T));
    T *obj = new(obj_mem) T(std::forward<Args>(args)...);
    
    try
    {
        this->add_destructor(obj);
    }
    catch(...)
    {
        obj->~T();
        throw;
    }

    return obj;
}

template<typename T, typename ... Args>
T *obj_arena_t::create_nodestruct(Args &&... args)
{
    void *obj_mem = mem_arena_.alloc(sizeof(T), alignof(T));
    return new(obj_mem) T(std::forward<Args>(args)...);
}

inline void obj_arena_t::release()
{
    for(obj_desctructor_t *d = destructor_entry_, *nd; d; d = nd)
    {
        nd = d->next;
        d->destruct();
        // d->~obj_destructor_t();
    }

    destructor_entry_ = nullptr;
    mem_arena_.free();
}

inline size_t obj_arena_t::used_bytes() const noexcept
{
    return mem_arena_.used_bytes();
}

} // namespace agz::alloc
