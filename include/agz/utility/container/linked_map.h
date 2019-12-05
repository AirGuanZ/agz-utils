#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <optional>

namespace agz::container
{

/**
 * @brief 由链表穿线的map，主要用于快速实现LRU
 */
template<typename TKey, typename TValue>
class linked_map_t
{
    struct map_node_t;

    struct list_node_t
    {
        typename std::map<TKey, map_node_t>::iterator map_iterator;
        TValue value;
    };

    struct map_node_t
    {
        typename std::list<list_node_t>::iterator list_iterator;
    };

    std::list<list_node_t> list_;
    std::map<TKey, map_node_t> map_;

public:

    using key_t   = TKey;
    using value_t = TValue;

    using self_t = linked_map_t<key_t, value_t>;

    linked_map_t() = default;

    linked_map_t(const self_t &)            = default;
    linked_map_t &operator=(const self_t &) = default;

    linked_map_t(self_t &&)            noexcept = default;
    linked_map_t &operator=(self_t &&) noexcept = default;

    /**
     * @brief 容器是否为空
     */
    bool empty() const noexcept
    {
        return list_.empty();
    }

    /**
     * @brief 容器中的元素数量
     */
    size_t size() const noexcept
    {
        return list_.size();
    }

    /**
     * @brief 查找并移除具有指定key的元素
     */
    std::optional<value_t> find_and_erase(const key_t &key)
    {
        auto it = map_.find(key);
        if(it == map_.end())
            return std::nullopt;

        auto ret = std::make_optional(std::move(it->second.list_iterator->value));
        list_.erase(it->second.list_iterator);
        map_.erase(it);
        return ret;
    }

    /**
     * @brief 查找具有指定key的元素，成功时返回其指针，失败时返回nullptr
     */
    value_t *find(const key_t &key)
    {
        auto it = map_.find(key);
        return it != map_.end() ? &it->second.list_iterator->value : nullptr;
    }

    /**
     * @brief 查找具有指定key的元素，成功时返回其指针，失败时返回nullptr
     */
    const value_t *find(const key_t &key) const
    {
        auto it = map_.find(key);
        return it != map_.end() ? &it->second.list_iterator->value : nullptr;
    }

    /**
     * @brief 返回链表的最后一个元素
     */
    value_t &back() noexcept
    {
        return list_.back().value;
    }

    /**
     * @brief 返回链表的最后一个元素
     */
    const value_t &back() const noexcept
    {
        return list_.back().value;
    }

    /**
     * @brief 删除位于链表末端的元素
     */
    void pop_back()
    {
        auto it = list_.back().map_iterator;
        list_.erase(it->second.list_iterator);
        map_.erase(it);
    }

    /**
     * @brief 返回链表的首元素
     */
    value_t &front() noexcept
    {
        return list_.front.value;
    }

    /**
     * @brief 返回链表首元素
     */
    const value_t &front() const noexcept
    {
        return list_.front.value;
    }

    /**
     * @brief 在链表头部插入一个元素
     */
    void push_front(const key_t &key, value_t &&value)
    {
        list_node_t list_node;
        list_node.value = std::move(value);
        list_.push_front(std::move(list_node));
        auto it_result = map_.insert(std::make_pair(key, map_node_t{ list_.begin() }));
        list_.front().map_iterator = it_result.first;
    }

    /**
     * @brief 在链表头部插入一个元素
     */
    void push_front(const key_t &key, const value_t &value)
    {
        this->push_front(key, value_t(value));
    }

    /**
     * @brief 遍历所有元素
     */
    template<typename Func>
    void for_each(Func &&func)
    {
        for(auto &node : list_)
            func(node.value);
    }

    /**
     * @brief 清空所有元素
     */
    void clear()
    {
        list_.clear();
        map_.clear();
    }

    /**
     * @brief 是否存在具有指定key的元素
     */
    bool exists(const key_t &key) const
    {
        return map_.find(key) != map_.end();
    }

    /**
     * @brief 利用map中的key对链表进行排序
     */
    template<typename Comp>
    void sort_list_by_key(Comp &&comp)
    {
        list_.sort([&](const list_node_t &lhs, const list_node_t &rhs)
        {
            return comp(lhs.map_iterator->first, lhs.map_iterator->first);
        });
        for(auto it = list_.begin(); it != list_.end(); ++it)
            it->map_iterator->second.list_iterator = it;
    }
};

} // namespace agz::container
