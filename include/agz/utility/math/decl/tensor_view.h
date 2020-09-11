#pragma once

#include "../../misc/type_list.h"
#include "vec.h"

namespace agz::math {
    
template<typename P, int D>
class tensor_t;

template<typename P, int D, bool CONST_TENSOR>
class tensor_view_t
{
    using data_t = std::conditional_t<
        CONST_TENSOR,
        const tensor_t<P, D> *,
              tensor_t<P, D> *>;

    data_t data_;

    vec<int, D> shape_;
    vec<int, D> base_;
    
public:

    using elem_t  = std::conditional_t<CONST_TENSOR, const P, P>;
    using index_t = vec<int, D>;
    using self_t  = tensor_view_t<P, D, CONST_TENSOR>;

    static constexpr int dim_v = D;

    tensor_view_t(
        data_t             data,
        const vec<int, D> &view_shape,
        const vec<int, D> &view_base) noexcept;

    tensor_view_t();

    template<typename U, bool CONST2,
             typename = std::enable_if_t<
                std::is_same_v<P, U> && CONST_TENSOR && !CONST2>>
    tensor_view_t(const tensor_view_t<U, D, false> &tv) noexcept;

    const index_t &shape() const noexcept;

    auto &at(const index_t &index)       noexcept;
    auto &at(const index_t &index) const noexcept;

    template<typename...Args,
             typename = std::enable_if_t<
                misc::type_list_t<Args...>::length == D>>
    auto &operator()(Args...indices) noexcept;
    
    template<typename...Args,
             typename = std::enable_if_t<
                misc::type_list_t<Args...>::length == D>>
    auto &operator()(Args...indices) const noexcept;

    void swap(self_t &swap_with) noexcept;

    self_t get_subview(
        const index_t &base,
        const index_t &shape);

    tensor_view_t<P, D, true> get_subview(
        const index_t &base,
        const index_t &shape) const;

    tensor_view_t<P, D, true> get_const_subview(
        const index_t &base,
        const index_t &shape) const;

    auto  _get_data() const noexcept { return data_; }
    auto &_get_base() const noexcept { return base_; }
};

} // namespace agz::math
