#pragma once

#include <type_traits>

#include "../common/common.h"

namespace agz::misc
{

template<typename Constructor>
class lazy_constructor_t
{
    Constructor constructor_;

public:

    template<typename F, typename =
        std::enable_if_t<!std::is_same_v<
            rm_rcv_t<F>, lazy_constructor_t>>>
    explicit lazy_constructor_t(F &&f)
        : constructor_{ std::forward<F>(f) }
    {
        
    }

    lazy_constructor_t(const lazy_constructor_t &) = default;
    lazy_constructor_t(lazy_constructor_t &&) noexcept = default;

    lazy_constructor_t &operator=(const lazy_constructor_t &) = default;
    lazy_constructor_t &operator=(lazy_constructor_t &&) noexcept = default;

    [[nodiscard]] operator auto() noexcept(noexcept(constructor_()))
    {
        return constructor_();
    }
};

template<typename F>
auto lazy_construct(F &&f) noexcept
{
    return lazy_constructor_t<rm_rcv_t<F>>(std::forward<F>(f));
}

#define AGZ_LAZY(EXPR) (::agz::misc::lazy_construct([&]{ return (EXPR); }))

} // namespace agz::misc
