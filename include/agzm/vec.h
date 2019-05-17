#pragma once

#include <tuple>

#include "common.h"

AGZM_BEGIN

namespace tvec_impl
{

    template<typename...Ts> constexpr size_t type_list_length_v = std::tuple_size_v<std::tuple<Ts...>>;

} // namespace tvec_impl

template<typename T, int D>
class tvec
{
    static_assert(D > 0);

public:

    using self_t = tvec<T, D>;
    using elem_t = T;

    T data[D];

    tvec()                         noexcept;
    explicit tvec(T val)           noexcept;
    explicit tvec(uninitialized_t) noexcept;

    template<typename...As, typename = std::enable_if_t<(D > 1 && tvec_impl::type_list_length_v<As...> == D)>>
    explicit tvec(As&&...args) noexcept;

    bool is_zero() const noexcept;

    auto length()        const noexcept;
    auto length_square() const noexcept;

    self_t normalize() const noexcept;

    self_t clamp(T min_v, T max_v) const noexcept;
    self_t clamp_low (T min_v)     const noexcept;
    self_t clamp_high(T max_v)     const noexcept;

    auto sum()     const noexcept;
    auto product() const noexcept;

    template<typename F> auto map(F &&f) const noexcept;

    bool operator!() const noexcept;
          T &operator[](size_t idx)       noexcept;
    const T &operator[](size_t idx) const noexcept;

    self_t &operator+=(const self_t &rhs) noexcept;
    self_t &operator-=(const self_t &rhs) noexcept;
    self_t &operator*=(const self_t &rhs) noexcept;
    self_t &operator/=(const self_t &rhs) noexcept;

    self_t &operator+=(T rhs) noexcept;
    self_t &operator-=(T rhs) noexcept;
    self_t &operator*=(T rhs) noexcept;
    self_t &operator/=(T rhs) noexcept;
};

template<typename T, int D> tvec<T, D> operator-(const tvec<T, D> &vec) noexcept;

template<typename T, int D> tvec<T, D> operator+(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> tvec<T, D> operator-(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> tvec<T, D> operator*(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> tvec<T, D> operator/(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D> tvec<T, D> operator+(const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D> tvec<T, D> operator-(const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D> tvec<T, D> operator*(const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D> tvec<T, D> operator/(const tvec<T, D> &lhs, T rhs) noexcept;

template<typename T, int D> tvec<T, D> operator+(T lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> tvec<T, D> operator*(T lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D> bool operator==(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> bool operator!=(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D> auto dot(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D> auto cos(const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

AGZM_END
