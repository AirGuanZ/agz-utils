#pragma once

#include "./mem_arena.h"

namespace agz::alloc
{

class object_releaser_t
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

        explicit obj_descructor_impl_t(T *ptr) noexcept: ptr(ptr) { }

        void destruct() noexcept override { ptr->~T(); }
    };

    mem_arena_t &mem_arena_;
    
    obj_desctructor_t *destructor_entry_;

    template<typename T>
    void add_destructor(T *obj);

public:

    explicit object_releaser_t(mem_arena_t &mem_arena);

    explicit object_releaser_t(memory_resource_arena_t &mem_pool);

    ~object_releaser_t();

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
    void destroy();
};


template<typename T>
void object_releaser_t::add_destructor(T *obj)
{
    if constexpr(std::is_trivially_destructible_v<T>)
        return;

    void *destructor_mem = mem_arena_.alloc(
        sizeof(obj_descructor_impl_t<T>), alignof(obj_descructor_impl_t<T>));
    auto destructor = new(destructor_mem) obj_descructor_impl_t<T>(obj);

    destructor->next = destructor_entry_;
    destructor_entry_ = destructor;
}

inline object_releaser_t::object_releaser_t(mem_arena_t &mem_arena)
    : mem_arena_(mem_arena), destructor_entry_(nullptr)
{
    
}

inline object_releaser_t::object_releaser_t(memory_resource_arena_t &mem_pool)
    : object_releaser_t(mem_pool.get_arena())
{
    
}

inline object_releaser_t::~object_releaser_t()
{
    destroy();
}

template<typename T, typename...Args>
T *object_releaser_t::create(Args&&...args)
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
T *object_releaser_t::create_nodestruct(Args &&... args)
{
    void *obj_mem = mem_arena_.alloc(sizeof(T), alignof(T));
    return new(obj_mem) T(std::forward<Args>(args)...);
}

inline void object_releaser_t::destroy()
{
    for(obj_desctructor_t *d = destructor_entry_, *nd; d; d = nd)
    {
        nd = d->next;
        d->destruct();
        // d->~obj_destructor_t();
    }

    destructor_entry_ = nullptr;
}

} // namespace agz::alloc
