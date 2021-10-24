#pragma once

#include <type_traits>
#include <utility>

#define AGZ_REFLECTION_MAX_MEMBER_VARIABLE_COUNT 127

namespace agz::misc::refl
{

template<typename T, typename F>
void for_each_member_variable(const F &f);

namespace detail
{

    template<int N>
    struct int_t : int_t<N - 1> { };

    template<>
    struct int_t<0> { };

    template<int N>
    struct sizer_t { char _data[N]; };

    static_assert(sizeof(sizer_t<17>) == 17);

    template<typename T, int...Is>
    constexpr int get_member_variable_count_aux(
        std::integer_sequence<int, Is...>)
    {
        return (... + (T::template _agz_refl_mem_var_entry<Is>::exists));
    }

    template<typename T, typename F, int...Is>
    void for_each_member_variable_aux(
        const F &f, std::integer_sequence<int, Is...>)
    {
        (T::template _agz_refl_mem_var_entry<Is>::template process<F>(f), ...);
    }

} // namespace detail

#define AGZ_BEGIN_CLASS_RELECTION(CLASS_NAME)                                   \
    using _agz_refl_self_t = CLASS_NAME;                                        \
    static ::agz::misc::refl::detail::sizer_t<1> _agz_refl_mem_var_counter(...);\
    template<typename T, int...Is>                                              \
    friend constexpr int                                                        \
        ::agz::misc::refl::detail::get_member_variable_count_aux(               \
            std::integer_sequence<int, Is...>);                                 \
    template<typename T, typename F, int...Is>                                  \
    friend void ::agz::misc::refl::detail::for_each_member_variable_aux(        \
        const F &f, std::integer_sequence<int, Is...>);                         \
    template<int N>                                                             \
    struct _agz_refl_mem_var_entry                                              \
    {                                                                           \
        static constexpr int exists = 0;                                        \
        template<typename F>                                                    \
        static void process(const F &f) { }                                     \
    }

#define AGZ_MEMBER_VARIABLE_REFLECTION(NAME)                                    \
    static constexpr int NAME##agz_refl_mem_var_index =                         \
        sizeof(_agz_refl_mem_var_counter(                                       \
            (::agz::misc::refl::detail::int_t<                                  \
                AGZ_REFLECTION_MAX_MEMBER_VARIABLE_COUNT>*)nullptr));           \
    static ::agz::misc::refl::detail::sizer_t<NAME##agz_refl_mem_var_index + 1> \
        _agz_refl_mem_var_counter(                                              \
            ::agz::misc::refl::detail::int_t<                                   \
                NAME##agz_refl_mem_var_index + 1>*);                            \
    template<>                                                                  \
    struct _agz_refl_mem_var_entry<NAME##agz_refl_mem_var_index - 1>            \
    {                                                                           \
        static constexpr int exists = 1;                                        \
        template<typename F>                                                    \
        static void process(const F &f)                                         \
        {                                                                       \
            f(&_agz_refl_self_t::NAME, #NAME);                                  \
        }                                                                       \
    }

template<typename T>
constexpr int get_member_variable_count()
{
    static_assert(std::is_same_v<T, typename T::_agz_refl_self_t>);
    return detail::get_member_variable_count_aux<T>(
        std::make_integer_sequence<
            int, AGZ_REFLECTION_MAX_MEMBER_VARIABLE_COUNT>());
}

template<typename T, typename F>
void for_each_member_variable(const F &f)
{
    static_assert(std::is_same_v<T, typename T::_agz_refl_self_t>);
    detail::for_each_member_variable_aux<T>(f, std::make_integer_sequence<
        int, AGZ_REFLECTION_MAX_MEMBER_VARIABLE_COUNT>());
}

} // namespace agz::misc::refl
