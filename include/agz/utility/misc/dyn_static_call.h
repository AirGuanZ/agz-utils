#pragma once

namespace agz::misc
{

#define AGZ_DYN_FUNC_BOOL1_TEMPLATE(FUNC_NAME, TABLE_NAME)                      \
    static const decltype(&FUNC_NAME<false>) TABLE_NAME[] =                     \
    {                                                                           \
        &FUNC_NAME<false>,                                                      \
        &FUNC_NAME<true>                                                        \
    }

#define AGZ_DYN_FUNC_BOOL2_TEMPLATE(FUNC_NAME, TABLE_NAME)                      \
    static const decltype(&FUNC_NAME<false, false>) TABLE_NAME[] =              \
    {                                                                           \
        &FUNC_NAME<false, false>,                                               \
        &FUNC_NAME<false, true>,                                                \
        &FUNC_NAME<true, false>,                                                \
        &FUNC_NAME<true, true>                                                  \
    }

#define AGZ_DYN_FUNC_BOOL3_TEMPLATE(FUNC_NAME, TABLE_NAME)                      \
    static const decltype(&FUNC_NAME<false, false, false>) TABLE_NAME[] =       \
    {                                                                           \
        &FUNC_NAME<false, false, false>,                                        \
        &FUNC_NAME<false, false, true>,                                         \
        &FUNC_NAME<false, true, false>,                                         \
        &FUNC_NAME<false, true, true>,                                          \
        &FUNC_NAME<true, false, false>,                                         \
        &FUNC_NAME<true, false, true>,                                          \
        &FUNC_NAME<true, true, false>,                                          \
        &FUNC_NAME<true, true, true>                                            \
    }

#define AGZ_DYN_FUNC_BOOL4_TEMPLATE(FUNC_NAME, TABLE_NAME)                      \
    static const decltype(&FUNC_NAME<false, false, false, false>)               \
        TABLE_NAME[] =                                                          \
    {                                                                           \
        &FUNC_NAME<false, false, false, false>,                                 \
        &FUNC_NAME<false, false, false, true>,                                  \
        &FUNC_NAME<false, false, true, false>,                                  \
        &FUNC_NAME<false, false, true, true>,                                   \
        &FUNC_NAME<false, true, false, false>,                                  \
        &FUNC_NAME<false, true, false, true>,                                   \
        &FUNC_NAME<false, true, true, false>,                                   \
        &FUNC_NAME<false, true, true, true>,                                    \
        &FUNC_NAME<true, false, false, false>,                                  \
        &FUNC_NAME<true, false, false, true>,                                   \
        &FUNC_NAME<true, false, true, false>,                                   \
        &FUNC_NAME<true, false, true, true>,                                    \
        &FUNC_NAME<true, true, false, false>,                                   \
        &FUNC_NAME<true, true, false, true>,                                    \
        &FUNC_NAME<true, true, true, false>,                                    \
        &FUNC_NAME<true, true, true, true>                                      \
    }

#define AGZ_DYN_FUNC_BOOL5_TEMPLATE(FUNC_NAME, TABLE_NAME)                      \
    static const decltype(&FUNC_NAME<false, false, false, false, false>)        \
        TABLE_NAME[] =                                                          \
    {                                                                           \
        &FUNC_NAME<false, false, false, false, false>,                          \
        &FUNC_NAME<false, false, false, false, true>,                           \
        &FUNC_NAME<false, false, false, true, false>,                           \
        &FUNC_NAME<false, false, false, true, true>,                            \
        &FUNC_NAME<false, false, true, false, false>,                           \
        &FUNC_NAME<false, false, true, false, true>,                            \
        &FUNC_NAME<false, false, true, true, false>,                            \
        &FUNC_NAME<false, false, true, true, true>,                             \
        &FUNC_NAME<false, true, false, false, false>,                           \
        &FUNC_NAME<false, true, false, false, true>,                            \
        &FUNC_NAME<false, true, false, true, false>,                            \
        &FUNC_NAME<false, true, false, true, true>,                             \
        &FUNC_NAME<false, true, true, false, false>,                            \
        &FUNC_NAME<false, true, true, false, true>,                             \
        &FUNC_NAME<false, true, true, true, false>,                             \
        &FUNC_NAME<false, true, true, true, true>,                              \
        &FUNC_NAME<true, false, false, false, false>,                           \
        &FUNC_NAME<true, false, false, false, true>,                            \
        &FUNC_NAME<true, false, false, true, false>,                            \
        &FUNC_NAME<true, false, false, true, true>,                             \
        &FUNC_NAME<true, false, true, false, false>,                            \
        &FUNC_NAME<true, false, true, false, true>,                             \
        &FUNC_NAME<true, false, true, true, false>,                             \
        &FUNC_NAME<true, false, true, true, true>,                              \
        &FUNC_NAME<true, true, false, false, false>,                            \
        &FUNC_NAME<true, true, false, false, true>,                             \
        &FUNC_NAME<true, true, false, true, false>,                             \
        &FUNC_NAME<true, true, false, true, true>,                              \
        &FUNC_NAME<true, true, true, false, false>,                             \
        &FUNC_NAME<true, true, true, false, true>,                              \
        &FUNC_NAME<true, true, true, true, false>,                              \
        &FUNC_NAME<true, true, true, true, true>                                \
    }

inline size_t bools_to_dyn_call_idx(std::initializer_list<bool> bools) noexcept
{
    size_t ret = 0;
    for(auto b : bools)
        ret = 2 * ret + b;
    return ret;
}

} // namespace agz::misc
