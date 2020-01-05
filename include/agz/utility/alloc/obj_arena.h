#pragma once

#include <type_traits>

#include "../misc/scope_guard.h"
#include "../misc/uncopyable.h"
#include "./mem_arena.h"
#include "./releaser.h"

namespace agz::alloc
{

/**
 * @brief 对象arena，多次创建，一次析构、释放
 *
 * 基于mem_pool_t实现
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
    explicit obj_arena_t(size_t mem_pool_chunk_size = 4096, size_t direct_alloc_threshold = 2048);

    ~obj_arena_t();

    /**
     * @brief 创建指定类型的对象，参数为构造函数参数
     */
    template<typename T, typename...Args>
    T *create(Args&&...args);

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
void obj_arena_t::add_destructor(T *obj)
{
    if constexpr(std::is_trivially_destructible_v<T>)
        return;

    void *destructor_mem = mem_arena_.alloc(sizeof(obj_descructor_impl_t<T>), alignof(obj_descructor_impl_t<T>));
    auto destructor = new(destructor_mem) obj_descructor_impl_t<T>(obj);

    destructor->next = destructor_entry_;
    destructor_entry_ = destructor;
}

inline obj_arena_t::obj_arena_t(size_t mem_pool_chunk_size, size_t direct_alloc_threshold)
    : mem_arena_(mem_pool_chunk_size, direct_alloc_threshold), destructor_entry_(nullptr)
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
    misc::scope_guard_t obj_guard([&] { obj->~T(); });

    this->add_destructor(obj);

    obj_guard.dismiss();
    return obj;
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
