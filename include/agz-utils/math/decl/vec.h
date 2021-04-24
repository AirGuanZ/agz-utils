#pragma once

#include <ostream>
#include <tuple>

#include "../../misc/type_list.h"
#include "common.h"

AGZ_MATH_BEGIN

template<typename T, int D>
class tvec
{
    static_assert(D > 0, "dimension of tvec is less than 1");

public:

    using self_t = tvec<T, D>;
    using elem_t = T;

    T data[D];

    AGZ_MATH_API tvec()                         noexcept;
    AGZ_MATH_API explicit tvec(T val)           noexcept;
    AGZ_MATH_API explicit tvec(uninitialized_t) noexcept;

    template<typename...As,
             typename = std::enable_if_t<
                (D > 1 && sizeof...(As) == D)>>
    AGZ_MATH_API tvec(As&&...args) noexcept;

    AGZ_MATH_API bool is_zero() const noexcept;

    AGZ_MATH_API auto length()        const noexcept;
    AGZ_MATH_API auto length_square() const noexcept;

    AGZ_MATH_API self_t normalize() const noexcept;

    AGZ_MATH_API self_t clamp(T min_v, T max_v) const noexcept;
    AGZ_MATH_API self_t clamp_low (T min_v)     const noexcept;
    AGZ_MATH_API self_t clamp_high(T max_v)     const noexcept;

    AGZ_MATH_API auto sum()     const noexcept;
    AGZ_MATH_API auto product() const noexcept;

    template<typename F>
    AGZ_MATH_API auto map(F &&f) const noexcept;

    AGZ_MATH_API bool operator!() const noexcept;

    AGZ_MATH_API       T &operator[](size_t idx)       noexcept;
    AGZ_MATH_API const T &operator[](size_t idx) const noexcept;

    AGZ_MATH_API self_t &operator+=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator-=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator*=(const self_t &rhs) noexcept;
    AGZ_MATH_API self_t &operator/=(const self_t &rhs) noexcept;

    AGZ_MATH_API self_t &operator+=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator-=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator*=(T rhs) noexcept;
    AGZ_MATH_API self_t &operator/=(T rhs) noexcept;

    AGZ_MATH_API bool elem_less(const self_t &rhs) const noexcept;
};

template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator-(
    const tvec<T, D> &vec) noexcept;

template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator+(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator-(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator*(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator/(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator+(
    const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator-(
    const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator*(
    const tvec<T, D> &lhs, T rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator/(
    const tvec<T, D> &lhs, T rhs) noexcept;

template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator+(
    T lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API tvec<T, D> operator*(
    T lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D>
AGZ_MATH_API bool operator==(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API bool operator!=(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D>
AGZ_MATH_API auto dot(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;
template<typename T, int D>
AGZ_MATH_API auto cos(
    const tvec<T, D> &lhs, const tvec<T, D> &rhs) noexcept;

template<typename T, int D>
std::ostream &operator<<(
    std::ostream &out, const tvec<T, D> &vec);

template<typename T, int D>
using vec = tvec<T, D>;

AGZ_MATH_END
