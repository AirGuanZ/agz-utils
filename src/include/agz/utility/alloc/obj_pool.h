#pragma once

#include <cassert>
#include <vector>

#include "../misc/scope_guard.h"
#include "../misc/uncopyable.h"
#include "./alloc.h"
#include "./releaser.h"

namespace agz::alloc
{

/**
 * @brief 对象池，可快速创建大量小对象
 * 
 * IMPROVE: 现在先拿releaser用者，空了再实现之
 */
template<typename T>
class obj_pool_t : public misc::uncopyable_t
{
    releaser_t releaser_;

public:

    template<typename...Args>
    T *create(Args&&...args)
    {
        return releaser_.create<T>(std::forward<Args>(args)...);
    }

    void free_all_memory()
    {
        releaser_.release();
    }
};
    
/*template<typename T>
class obj_pool_t : public misc::uncopyable_t
{
    // 编译时求最大值
    template<typename F>
    static constexpr F static_max(F lhs, F rhs)
    {
        return lhs > rhs ? lhs : rhs;
    }

    // 求不小于original_size的值中最小的能被align整除的那个
    // 即min_{x}\{ x >= original_size and x mod align = 0 \}
    static constexpr size_t align_to(size_t original_size, size_t align)
    {
        return original_size % align ? (original_size + align - original_size % align) : original_size;
    }

    // 用来保证所持有的chunk的内存最后一定被释放
    struct chunk_holder_t
    {
        void *chunk_ptr = nullptr;

        ~chunk_holder_t()
        {
            assert(chunk_ptr);
            aligned_free(chunk_ptr);
        }
    };

    struct node
    {
        node *next;
    };

    // 要申请的基本元素的对齐值
    static constexpr size_t BLOCK_ALIGN = static_max(alignof(T), alignof(node));

    // 为每个对象准备的内存区域block的布局如下：
    //   首先是按align_v对齐的一块大小为sizeof(T)的区域
    //   然后是一小段padding，要保证padding后的地址符合alignof(void*)
    //   然后跟一个node，用来作为侵入式对象链表的节点，指向下一个节点
    //   最后这块内存区域的大小必须能被align_v整除，因为它相邻的下一片区域会被用作下一个对象
    //   如果T满足trivially destructible，那么就不要padding和obj node部分了

    // obj node距block开头的字节数
    static constexpr size_t NODE_OFFSET = align_to(static_max(sizeof(T), sizeof(node)), alignof(node));

    // block大小
    static constexpr size_t BLOCK_SIZE = align_to(NODE_OFFSET + sizeof(node), BLOCK_ALIGN);

    static T *block_to_obj(void *block)
    {
        return static_cast<T*>(block);
    }

    static node *block_to_obj_node(void *block)
    {
        return reinterpret_cast<node*>(static_cast<char*>(block) + NODE_OFFSET);
    }
    
    static void *obj_node_to_block(node *node)
    {
        return static_cast<char*>(node) - NODE_OFFSET;
    }

    static node *block_to_free_node(void *block)
    {
        return static_cast<node*>(block);
    }

    std::vector<chunk_holder_t> chunks_;
    node *free_entry_;
    node *obj_entry_;

    size_t chunk_byte_size_;

    void new_chunk()
    {
        void *new_chunk = aligned_alloc(chunk_byte_size_, BLOCK_ALIGN);
        misc::scope_guard_t guard([=] { aligned_free(new_chunk); });

        chunks_.push_back({ new_chunk });
        guard.dismiss();

        size_t used_bytes = 0;
        void *block = new_chunk;
        while(used_bytes < chunk_byte_size_)
        {
            node *free_node = block_to_free_node(block);
            free_node->next = free_entry_;
            free_entry_ = free_node;

            used_bytes += BLOCK_SIZE;
            block = static_cast<char*>(block) + BLOCK_SIZE;
        }
    }

    void *alloc_block()
    {
        if(!free_entry_)
            new_chunk();
        assert(free_entry_);

        void *block = free_entry_;
        free_entry_ = free_entry_->next;

        return block;
    }

    void free_block(void *block)
    {
        node *node = block_to_free_node(block);
        node->next = free_entry_;
        free_entry_ = node;
    }

public:

    explicit obj_pool_t(size_t chunk_obj_count = 128)
        : free_entry_(nullptr), obj_entry_(nullptr)
    {
        chunk_byte_size_ = chunk_obj_count * BLOCK_SIZE;
    }

    ~obj_pool_t()
    {
        free_all_memory();
    }

    template<typename...Args>
    T *create(Args&&...args)
    {
        void *block = alloc_block();
        misc::scope_guard_t guard([=] { free_block(block); });

        T *obj = block_to_obj(block);
        new(obj) T(std::forward<Args>(args)...);
        guard.dismiss();

        node *obj_node = block_to_obj_node(block);
        obj_node->next = obj_entry_;
        obj_entry_ = obj_node;

        return obj;
    }

    void destroy_all_objs()
    {
        for(node *obj_node = obj_entry_, *next; obj_node; obj_node = next)
        {
            next = obj_node->next;
            void *block = obj_node_to_block(obj_node);
            {
                T *obj = block_to_obj(block);
                call_destructor(*obj);
            }
            {
                node *free_node = block_to_free_node(block);
                free_node->next = free_entry_;
                free_entry_ = free_node;
            }
        }
        obj_entry_ = nullptr;
    }

    void free_all_memory()
    {
        chunks_.clear();
        free_entry_ = nullptr;
        obj_entry_ = nullptr;
    }
};*/

} // namespace agz::alloc
