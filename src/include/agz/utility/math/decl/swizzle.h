#pragma once

#include "color2.h"
#include "color3.h"
#include "color4.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace agz::math {

namespace swizzle_impl
{
    
    template<int A0>                     constexpr int axis_list_length() { return 1;                               }
    template<int A0, int A1, int...A123> constexpr int axis_list_length() { return 1 + axis_list_length<A1, A123...>(); }

    template<typename T, int D> struct dim_2_vec { };
    template<typename T> struct dim_2_vec<T, 1> { using type = T;        };
    template<typename T> struct dim_2_vec<T, 2> { using type = tvec2<T>; };
    template<typename T> struct dim_2_vec<T, 3> { using type = tvec3<T>; };
    template<typename T> struct dim_2_vec<T, 4> { using type = tvec4<T>; };

    template<typename T, int D> struct dim_2_color { };
    template<typename T> struct dim_2_color<T, 2> { using type = tcolor2<T>; };
    template<typename T> struct dim_2_color<T, 3> { using type = tcolor3<T>; };
    template<typename T> struct dim_2_color<T, 4> { using type = tcolor4<T>; };

    template<typename V, int I> auto idx_2_mem(const V &vec) noexcept { return vec[I]; }

} // namespace swizzle_impl

template<int...Is, typename V>
auto swizzle_vec(const V &vec) noexcept
{
    using ret_t = typename swizzle_impl::dim_2_vec<
        typename V::elem_t, swizzle_impl::axis_list_length<Is...>()>::type;
    return ret_t(swizzle_impl::idx_2_mem<V, Is>(vec)...);
}

template<int...Is, typename V>
auto swizzle_color(const V &color) noexcept
{
    using ret_t = typename swizzle_impl::dim_2_color<
        typename V::elem_t, swizzle_impl::axis_list_length<Is...>()>::type;
    return ret_t(swizzle_impl::idx_2_mem<V, Is>(color)...);
}

} // namespace agz::math
