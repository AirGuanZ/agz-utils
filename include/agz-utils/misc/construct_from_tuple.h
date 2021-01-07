#pragma once

#include <tuple>

namespace agz::misc
{

namespace impl
{
    template<class T, class Tuple, size_t... Is>
    T construct_from_tuple(Tuple &&tuple, std::index_sequence<Is...>)
    {
        return T(std::get<Is>(std::forward<Tuple>(tuple))...);
    }

} // namespace impl

/**
 * @brief 以tuple中的元素为参数创建一个T类型的对象
 */
template<class T, class Tuple>
T construct_from_tuple(Tuple &&tuple)
{
    return impl::construct_from_tuple<T>(
        std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}

} // namespace agz::misc
