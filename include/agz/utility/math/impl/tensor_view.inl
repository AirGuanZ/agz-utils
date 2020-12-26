#pragma once

#include <cassert>

AGZ_MATH_BEGIN

template<typename P, int D, bool CONST_TENSOR>
tensor_view_t<P, D, CONST_TENSOR>::tensor_view_t(
    data_t             data,
    const vec<int, D> &view_shape,
    const vec<int, D> &view_base) noexcept
    : data_(data), shape_(view_shape), base_(view_base)
{
    
}

template<typename P, int D, bool CONST_TENSOR>
tensor_view_t<P, D, CONST_TENSOR>::tensor_view_t()
    : data_(nullptr), shape_(0), base_(0)
{
    
}

template<typename P, int D, bool CONST_TENSOR>
template<typename U, bool CONST2, typename>
tensor_view_t<P, D, CONST_TENSOR>::tensor_view_t(
    const tensor_view_t<U, D, false> &tv) noexcept
    : data_(tv._get_data()), shape_(tv.shape()), base_(tv._get_base())
{
    
}

template<typename P, int D, bool CONST_TENSOR>
const typename tensor_view_t<P, D, CONST_TENSOR>::index_t &
    tensor_view_t<P, D, CONST_TENSOR>::shape() const noexcept
{
    return shape_;
}

template<typename P, int D, bool CONST_TENSOR>
auto &tensor_view_t<P, D, CONST_TENSOR>::at(const index_t &index) noexcept
{
    assert(index.elem_less(shape_));
    return data_->at(index + base_);
}

template<typename P, int D, bool CONST_TENSOR>
auto &tensor_view_t<P, D, CONST_TENSOR>::at(const index_t &index) const noexcept
{
    using ret_t = std::add_const_t<P>;

    assert(index.elem_less(shape_));
    return static_cast<ret_t&>(data_->at(index + base_));
}

template<typename P, int D, bool CONST_TENSOR>
template<typename ... Args, typename>
auto &tensor_view_t<P, D, CONST_TENSOR>::operator()(Args ... indices) noexcept
{
    return at({ int(indices)... });
}

template<typename P, int D, bool CONST_TENSOR>
template<typename ... Args, typename>
auto &tensor_view_t<P, D, CONST_TENSOR>::operator()(Args ... indices) const noexcept
{
    return at({ int(indices)... });
}

template<typename P, int D, bool CONST_TENSOR>
void tensor_view_t<P, D, CONST_TENSOR>::swap(self_t &swap_with) noexcept
{
    std::swap(data_,  swap_with.data_);
    std::swap(shape_, swap_with.shape_);
    std::swap(base_,  swap_with.base_);
}

template<typename P, int D, bool CONST_TENSOR>
typename tensor_view_t<P, D, CONST_TENSOR>::self_t
    tensor_view_t<P, D, CONST_TENSOR>::get_subview(
        const index_t &base, const index_t &shape)
{
    return self_t(data_, shape, base_ + base);
}

template<typename P, int D, bool CONST_TENSOR>
tensor_view_t<P, D, true> tensor_view_t<P, D, CONST_TENSOR>::get_subview(
    const index_t &base, const index_t &shape) const
{
    return tensor_view_t<P, D, true>(data_, shape, base_ + base);
}

template<typename P, int D, bool CONST_TENSOR>
tensor_view_t<P, D, true> tensor_view_t<P, D, CONST_TENSOR>::get_const_subview(
    const index_t &base, const index_t &shape) const
{
    return tensor_view_t<P, D, true>(data_, shape, base_ + base);
}

AGZ_MATH_END
