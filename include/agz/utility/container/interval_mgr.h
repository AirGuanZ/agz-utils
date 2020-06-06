#pragma once

#include <cassert>
#include <optional>
#include <map>

#include "../misc/uncopyable.h"

namespace agz::container
{

/**
 * @brief 用于管理一组由不相交区间描述的资源
 *
 * 可以在其中分配和释放子区间
 */
template<typename I = int>
class interval_mgr_t : public misc::uncopyable_t
{
public:

    using Isize = std::make_unsigned_t<I>;

    interval_mgr_t() = default;

    interval_mgr_t(interval_mgr_t<I> &&other) noexcept = default;

    interval_mgr_t<I> &operator=(interval_mgr_t<I> &&other) noexcept = default;

    void swap(interval_mgr_t<I> &other) noexcept;

    /**
     * @brief 释放一个过去分配的子区间，或添加一个全新的区间
     *
     * 要求[beg, end)与其他任何一个区间都不相交
     */
    void free(I beg, I end);

    /**
     * @brief 分配一个制定大小的子区间
     *
     * 成功时返回区间起始位置，失败时返回std::nullopt
     */
    std::optional<I> alloc(Isize size);

private:
    
    struct Block;

    using start_to_block_t = std::map<I, Block>;
    using size_to_start_t  = std::multimap<
        Isize, typename start_to_block_t::iterator>;

    struct Block
    {
        Isize size;
        typename size_to_start_t::iterator size_to_start_it;
    };

    start_to_block_t start_to_block_;
    size_to_start_t  size_to_start_;

    void remove_block(typename start_to_block_t::iterator it);
};

template<typename I>
void interval_mgr_t<I>::swap(interval_mgr_t<I> &other) noexcept
{
    std::swap(start_to_block_, other.start_to_block_);
    std::swap(size_to_start_, other.size_to_start_);
}

template<typename I>
void interval_mgr_t<I>::free(I beg, I end)
{
    // merge new block with successor/predecessor

    auto next_block_it = start_to_block_.upper_bound(beg);
    auto last_block_it = next_block_it;
    if(last_block_it != start_to_block_.end())
    {
        if(last_block_it == start_to_block_.begin())
            last_block_it = start_to_block_.end();
        else
            --last_block_it;
    }

    if(next_block_it != start_to_block_.end() && next_block_it->first == end)
    {
        end += next_block_it->second.size;
        remove_block(next_block_it);
    }

    if(last_block_it != start_to_block_.end() &&
        (last_block_it->first + last_block_it->second.size == beg))
    {
        beg = last_block_it->first;
        remove_block(last_block_it);
    }

    // insert the new free block

    const Isize size = end - beg;
    auto [block_it, block_inserted] = start_to_block_.insert({ beg, Block{} });
    assert(block_inserted);

    auto size_it = size_to_start_.insert({ size, block_it });
    block_it->second.size             = size;
    block_it->second.size_to_start_it = size_it;
}

template<typename I>
std::optional<I> interval_mgr_t<I>::alloc(Isize size)
{
    auto size_it = size_to_start_.lower_bound(size);
    if(size_it == size_to_start_.end())
        return std::nullopt;
    auto block_it = size_it->second;

    const I     ret      = block_it->first;
    const I     restBeg  = ret + size;
    const Isize restSize = block_it->second.size - size;

    this->remove_block(block_it);

    if(restSize > 0)
        this->free(restBeg, restBeg + restSize);

    return ret;
}

template<typename I>
void interval_mgr_t<I>::remove_block(typename start_to_block_t::iterator it)
{
    auto size_it = it->second.size_to_start_it;
    start_to_block_.erase(it);
    size_to_start_.erase(size_it);
}

} // namespace agz::container
