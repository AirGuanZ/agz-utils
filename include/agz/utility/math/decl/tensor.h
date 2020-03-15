#pragma once

#include "../../misc/type_list.h"
#include "common.h"
#include "vec.h"

namespace agz::math {
    
template<typename P, int D>
class tensor_t
{
    P *data_;

    vec<int, D> shape_;
    int elem_count_;

    struct from_indexed_func_t { };
    struct from_linear_indexed_func_t { };

    template<typename F>
    tensor_t(from_indexed_func_t, const vec<int, D> &shape, F &&func);

    template<typename F>
    tensor_t(from_linear_indexed_func_t, const vec<int, D> &shape, F &&func);

    void destruct();

public:

    using elem_t  = P;
    using index_t = vec<int, D>;
    using self_t  = tensor_t<P, D>;

    static constexpr int dim_v = D;

    tensor_t();
    tensor_t(const index_t &shape, uninitialized_t);
    explicit tensor_t(const index_t &shape, const P &init_value = P());

    template<typename F>
    static self_t from_indexed_fn(const index_t &shape, F &&func);
   
    template<typename F>
    static self_t from_linear_indexed_fn(const index_t &shape, F &&func);

    static self_t from_array(const index_t &shape, const P *data);

    tensor_t(const self_t &copy_from);
    tensor_t(self_t &&move_from) noexcept;

    tensor_t<P, D> &operator=(const self_t &copy_from);
    tensor_t<P, D> &operator=(self_t &&move_from) noexcept;

    ~tensor_t();

    void initialize(const index_t &shape, const P &init_value = P());

    bool is_available() const noexcept;

    void destroy();

    const index_t &shape() const noexcept;

    int elem_count() const noexcept;

    P &at(const index_t &index) noexcept;

    const P &at(const index_t &index) const noexcept;

    P &at(int index) noexcept;

    const P &at(int index) const noexcept;

    template<typename...Args,
             typename = std::enable_if_t<
                misc::type_list_t<Args...>::length == D>>
    P &operator()(Args...indices) noexcept;

    template<typename...Args,
             typename = std::enable_if_t<
                misc::type_list_t<Args...>::length == D>>
    const P &operator()(Args...indices) const noexcept;

    void swap(self_t &swap_with) noexcept;

    template<typename F>
    auto map(F &&func) const;

          P *raw_data() noexcept;
    const P *raw_data() const noexcept;

    size_t raw_data_size() const noexcept;
};

template<typename P, int D, typename F>
auto elemwise_unary(const tensor_t<P, D> &opd, F &&opr);

template<typename P, int D, typename F>
auto elemwise_binary(
    const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs, F &&opr);

template<typename P, int D>
bool operator==(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

template<typename P, int D>
bool operator!=(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

template<typename P, int D>
auto operator+(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

template<typename P, int D>
auto operator-(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

template<typename P, int D>
auto operator*(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

template<typename P, int D>
auto operator/(const tensor_t<P, D> &lhs, const tensor_t<P, D> &rhs);

} // namespace agz::math
