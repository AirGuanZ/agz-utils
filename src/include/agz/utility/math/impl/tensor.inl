#pragma once

#include <cassert>
#include <utility>

#include "../../alloc/alloc.h"
#include "../../misc/scope_guard.h"

AGZM_BEGIN

namespace tensor_impl
{

    template<int D>
    tvec<int, D> next_index(const tvec<int, D> &shape, const tvec<int, D> &index)
    {
        auto ret = index;
        for(int i = D - 1; i >= 1; --i)
        {
            if(++ret[i] < shape[i])
                return ret;
            ret[i] = 0;
        }
        ++ret[0];
        return ret;
    }

    template<int D>
    struct to_linear_index_aux
    {
        static int eval(const tvec<int, D> &shape, const tvec<int, D> &index) noexcept
        {
            int ret = 0, base = 1;
            for(int i = D - 1; i >= 0; --i)
            {
                ret += index[i] * base;
                base *= shape[i];
            }
            return ret;
        }
    };

    template<>
    struct to_linear_index_aux<1>
    {
        static int eval(const tvec<int, 1> &, const tvec<int, 1> &index) noexcept
        {
            return index[0];
        }
    };

    template<>
    struct to_linear_index_aux<2>
    {
        static int eval(const tvec<int, 2> &shape, const tvec<int, 2> &index) noexcept
        {
            return index[0] * shape[1] + index[1];
        }
    };

    template<>
    struct to_linear_index_aux<3>
    {
        static int eval(const tvec<int, 3> &shape, const tvec<int, 3> &index) noexcept
        {
            int s2 = shape[2], s1 = shape[1] * shape[2];
            return index[0] * s1 + index[1] * s2 + index[2];
        }
    };

} // namespace tensor_impl

template<typename P, int D>
template<typename F>
tensor_t<P, D>::tensor_t(from_indexed_func_t, const vec<int, D> &shape, F &&func)
    : shape_(shape), elem_count_(shape.product())
{
    assert(elem_count_ > 0);

    int constructed_count = 0;
    P *data = std::allocator<P>().allocate(elem_count_);
    misc::scope_guard_t guard([&]
    {
        for(int i = 0; i != constructed_count; ++i)
            alloc::call_destructor(data[i]);
        std::allocator<P>().deallocate(data, elem_count_);
    });

    index_t index(0);
    for(int i = 0; i != elem_count_; ++i, ++constructed_count)
    {
        const index_t &index_view = index;
        new(&data[i]) P(func(index_view));
        index = tensor_impl::next_index(index);
    }

    guard.dismiss();
}

template<typename P, int D>
template<typename F>
tensor_t<P, D>::tensor_t(from_linear_indexed_func_t, const vec<int, D> &shape, F &&func)
    : shape_(shape), elem_count_(shape.product())
{
    assert(elem_count_ > 0);

    int constructed_count = 0;
    P *data = std::allocator<P>().allocate(elem_count_);
    misc::scope_guard_t guard([&]
    {
        for(int i = 0; i != constructed_count; ++i)
            alloc::call_destructor(data[i]);
        std::allocator<P>().deallocate(data, elem_count_);
    });

    for(int i = 0; i != elem_count_; ++i, ++constructed_count)
        new(&data[i]) P(func(i));

    guard.dismiss();
}

template<typename P, int D>
tensor_t<P, D>::tensor_t()
    : data_(nullptr), shape_(0), elem_count_(0)
{

}

template<typename P, int D>
tensor_t<P, D>::tensor_t(const index_t &shape, uninitialized_t)
{
    P *data = std::allocator<P>().allocate(elem_count_);
    data_.reset(data);
}

template<typename P, int D>
tensor_t<P, D>::tensor_t(const index_t &shape, const P &init_value)
    : tensor_t(from_linear_indexed_func_t{ }, shape,
        [&](int i) { return init_value; })
{

}

template<typename P, int D>
template<typename F>
typename tensor_t<P, D>::self_t tensor_t<P, D>::from_indexed_fn(const index_t &shape, F &&func)
{
    return self_t(from_indexed_func_t{ }, shape, std::forward<F>(func));
}

template<typename P, int D>
template<typename F>
typename tensor_t<P, D>::self_t tensor_t<P, D>::from_lineared_indexed_fn(const index_t &shape, F &&func)
{
    return self_t(from_linear_indexed_func_t{ }, shape, std::forward<F>(func));
}

template<typename P, int D>
typename tensor_t<P, D>::self_t tensor_t<P, D>::from_array(const index_t &shape, const P *data)
{
    return from_lineared_indexed_fn(shape, [&](int i)
    {
        return data[i];
    });
}

template<typename P, int D>
tensor_t<P, D>::tensor_t(const self_t &copy_from)
{
    if(copy_from.is_available())
    {
        *this = tensor_t(from_linear_indexed_func_t{ }, copy_from.shape_,
            [&](int i) { return copy_from.data_[i]; });
    }
}

template<typename P, int D>
tensor_t<P, D>::tensor_t(self_t &&move_from) noexcept
{
    swap(move_from);
}

template<typename P, int D>
tensor_t<P, D> &tensor_t<P, D>::operator=(const self_t &copy_from)
{
    self_t t(copy_from);
    swap(t);
    return *this;
}

template<typename P, int D>
tensor_t<P, D> &tensor_t<P, D>::operator=(self_t &&move_from) noexcept
{
    swap(move_from);
    return *this;
}

template<typename P, int D>
void tensor_t<P, D>::initialize(const index_t &shape, const P &init_value)
{
    self_t t(shape, init_value);
    swap(t);
}

template<typename P, int D>
bool tensor_t<P, D>::is_available() const noexcept
{
    return data_ != nullptr;
}

template<typename P, int D>
void tensor_t<P, D>::destroy()
{
    data_ = nullptr;
    shape_ = index_t(0);
    elem_count_ = 0;
}

template<typename P, int D>
const typename tensor_t<P, D>::index_t& tensor_t<P, D>::shape() const noexcept
{
    return shape_;
}

template<typename P, int D>
int tensor_t<P, D>::elem_count() const noexcept
{
    return elem_count_;
}

template<typename P, int D>
P &tensor_t<P, D>::at(const index_t &index) noexcept
{
    assert(is_available());
    int linear_index = tensor_impl::to_linear_index_aux<D>::eval(shape_, index);
    return data_[linear_index];
}

template<typename P, int D>
const P &tensor_t<P, D>::at(const index_t &index) const noexcept
{
    assert(is_available());
    int linear_index = tensor_impl::to_linear_index_aux<D>::eval(shape_, index);
    return data_[linear_index];
}

template<typename P, int D>
template<typename...Args, typename>
P &tensor_t<P, D>::operator()(Args...indices) noexcept
{
    return at({ int(indices)... });
}

template<typename P, int D>
template<typename...Args, typename>
const P &tensor_t<P, D>::operator()(Args...indices) const noexcept
{
    return at({ int(indices)... });
}

template<typename P, int D>
P &tensor_t<P, D>::at(int index) noexcept
{
    assert(is_available() && index < elem_count_);
    return data_[index];
}

template<typename P, int D>
const P &tensor_t<P, D>::at(int index) const noexcept
{
    assert(is_available() && index < elem_count_);
    return data_[index];
}

template<typename P, int D>
void tensor_t<P, D>::swap(self_t &swap_with) noexcept
{
    std::swap(data_, swap_with.data_);
    std::swap(shape_, swap_with.shape_);
    std::swap(elem_count_, swap_with.elem_count_);
}

template<typename P, int D>
template<typename F>
auto tensor_t<P, D>::map(F &&func) const
{
    using ret_pixel_t = rm_rcv_t<decltype(func(data_[0]))>;

    if(!is_available())
        return tensor_t<ret_pixel_t, D>();

    return tensor_t<ret_pixel_t, D>::from_lineared_indexed_fn(
        shape_, [&](int i)
    {
        return func(data_[i]);
    });
}

template<typename P, int D>
P *tensor_t<P, D>::raw_data() noexcept
{
    return data_.get();
}

template<typename P, int D>
const P *tensor_t<P, D>::raw_data() const noexcept
{
    return data_.get();
}

template<typename P, int D>
size_t tensor_t<P, D>::raw_data_size() const noexcept
{
    return sizeof(P) * elem_count_;
}

template<typename P, int D, typename F>
auto elemwise_unary(const tensor_t<P, D> &opd, F &&opr)
{
    return opd.map(std::forward<F>(opr));
}

template<typename P, int D, typename F>
auto elemwise_binary(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs, F &&opr)
{
    if(lhs.shape() != rhs.shape() || !lhs.is_available())
    {
        throw std::runtime_error(
            "invalid/unmatched tensor size in elementwise binary operation");
    }

    using ret_pixel_t = rm_rcv_t<std::invoke_result_t<F, const P&, const P&>>;
    return tensor_t<ret_pixel_t, D>::from_lineared_indexed_fn(lhs.shape(),
        [&](int i)
    {
        return opr(lhs.at(i), rhs.at(i));
    });
}

template<typename P, int D>
bool operator==(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    if(lhs.shape() != rhs.shape() || !lhs.is_available())
    {
        throw std::runtime_error(
            "invalid/unmatched tensor size in elementwise binary operation");
    }

    for(int i = 0; i != lhs.elem_count(); ++i)
    {
        if(lhs.at(i) != rhs.at(i))
            return false;
    }

    return true;
}

template<typename P, int D>
bool operator!=(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    if(lhs.shape() != rhs.shape() || !lhs.is_available())
    {
        throw std::runtime_error(
            "invalid/unmatched tensor size in elementwise binary operation");
    }

    for(int i = 0; i != lhs.elem_count(); ++i)
    {
        if(lhs.at(i) != rhs.at(i))
            return true;
    }

    return false;
}

template<typename P, int D>
auto operator+(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    return elemwise_binary(lhs, rhs, std::plus<tensor_t<P, D>>());
}

template<typename P, int D>
auto operator-(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    return elemwise_binary(lhs, rhs, std::minus<tensor_t<P, D>>());
}

template<typename P, int D>
auto operator*(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    return elemwise_binary(lhs, rhs, std::multiplies<tensor_t<P, D>>());
}

template<typename P, int D>
auto operator/(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs)
{
    return elemwise_binary(lhs, rhs, std::divides<tensor_t<P, D>>());
}

AGZM_END
