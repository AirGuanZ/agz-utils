#pragma once

namespace agz::misc
{
    
namespace type_list_impl
{
    
    template<typename T>
    constexpr int length()
    {
        return 1;
    }

    template<typename T, typename T1, typename...Ts>
    constexpr int length()
    {
        return 1 + length<T1, Ts...>();
    }

    template<int I, typename T, typename...Ts>
    class at_aux
    {
        using type = typename at_aux<I - 1, Ts...>::type;
    };

    template<typename T, typename...Ts>
    class at_aux<0, T, Ts...>
    {
        using type = T;
    };

} // namespace type_list_impl

template<typename...Ts>
class type_list_t
{
public:

    static constexpr int length = type_list_impl::length<Ts...>();
    static constexpr int size   = length;

    template<int I>
    using at = typename type_list_impl::at_aux<I, Ts...>::type;

    template<int...Is>
    using sublist = type_list_t<at<Is>...>;
};

} // namespace misc
