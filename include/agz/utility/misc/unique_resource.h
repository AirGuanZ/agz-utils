#pragma once

#include <utility>

namespace agz::misc
{

template<typename R, typename D>
class unique_resource_t
{
    R resource_;
    D deleter_;

    bool execute_ = false;

    unique_resource_t(const unique_resource_t &)            = delete;
    unique_resource_t &operator=(const unique_resource_t &) = delete;

public:

    unique_resource_t() = default;

    explicit unique_resource_t(R &&resource, D &&deleter) noexcept
        : resource_(std::move(resource)),
          deleter_(std::move(deleter)),
          execute_(true)
    {
        
    }

    unique_resource_t(unique_resource_t &&other) noexcept
        : unique_resource_t()
    {
        this->swap(other);
    }

    unique_resource_t &operator=(unique_resource_t &&other) noexcept
    {
        this->swap(other);
        return *this;
    }

    ~unique_resource_t()
    {
        if(execute_)
            deleter_(resource_);
    }

    void swap(unique_resource_t &other) noexcept
    {
        std::swap(resource_, other.resource_);
        std::swap(deleter_,  other.deleter_);
        std::swap(execute_,  other.execute_);
    }

    void reset()
    {
        if(execute_)
            execute_ = false;
        deleter_(resource_);
    }

    const R &get() const noexcept
    {
        return resource_;
    }

    operator const R &() const noexcept
    {
        return resource_;
    }

    const auto operator->() const noexcept
    {
        if constexpr(std::is_pointer_v<R>)
            return resource_;
        else
            return &resource_;
    }

    auto operator->() noexcept
    {
        if constexpr(std::is_pointer_v<R>)
            return resource_;
        else
            return &resource_;
    }

    std::add_lvalue_reference_t<
        std::remove_pointer_t<R>>
    operator*() const {
        return *resource_;
    }
};

} // namespace agz::misc
