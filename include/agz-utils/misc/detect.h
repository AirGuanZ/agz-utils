#pragma once

#include <type_traits>

// see https://en.cppreference.com/w/cpp/experimental/is_detected

namespace agz::misc
{
    namespace detail
    {
        template<class AlwaysVoid, template<class...> class Op, typename...Args>
        struct detector
        {
            using value_t = std::false_type;
        };

        template<template<class...> class Op, typename...Args>
        struct detector<std::void_t<Op<Args...>>, Op, Args...>
        {
            using value_t = std::true_type;
        };

    } // namespace detail

    template<template<class...> class Op, typename...Args>
    using is_detected_t = typename detail::detector<void, Op, Args...>::value_t;

    template<template<class...> class Op, typename...Args>
    constexpr bool is_detected_v = is_detected_t<Op, Args...>::value;

} // namespace agz::misc
