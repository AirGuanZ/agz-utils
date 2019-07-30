#pragma once

#include <type_traits>

namespace agz::misc
{

/**
 * @brief 表示一组连续放置的相同类型的对象
 */
template<typename T>
class span
{
    T *elems_;
    size_t size_;

public:

    using value_t       = T;
    using const_value_t = std::add_const_t<T>;

    span() noexcept
        : elems_(nullptr), size_(0)
    {

    }

    span(T *elems, size_t size) noexcept
        : elems_(elems), size_(size)
    {
        assert(!size || elems);
    }

    size_t size() const noexcept
    {
        return size_;
    }

    size_t length() const noexcept
    {
        return size_;
    }

    bool empty() const noexcept
    {
        return !size_;
    }

    value_t &operator[](size_t idx) noexcept
    {
        assert(elems_ && idx < size_);
        return elems_[idx];
    }

    const_value_t &operator[](size_t idx) const noexcept
    {
        assert(elems_ && idx < size_);
        return elems_[idx];
    }

    auto begin() noexcept
    {
        return elems_;
    }

    auto end() noexcept
    {
        return elems_ + size_;
    }

    const_value_t *begin() const noexcept
    {
        return elems_;
    }

    const_value_t *end() const noexcept
    {
        return elems_ + size_;
    }

    auto *data() noexcept
    {
        return elems_;
    }

    const_value_t *data() const noexcept
    {
        return elems_;
    }

    auto &front() noexcept
    {
        assert(!empty());
        return *elems_;
    }

    auto &back() noexcept
    {
        assert(!empty());
        return *(elems_ + size_ - 1);
    }

    const_value_t &front() const noexcept
    {
        assert(!empty());
        return *elems_;
    }

    const_value_t &back() const noexcept
    {
        assert(!empty());
        return *(elems_ + size_ - 1);
    }
};

} // namespace agz::misc

