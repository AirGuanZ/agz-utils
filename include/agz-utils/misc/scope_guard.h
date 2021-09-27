#pragma once

#include "../common/common.h"
#include "uncopyable.h"

namespace agz::misc
{

template<typename T, typename = std::enable_if_t<std::is_invocable_v<T>>>
class scope_guard_t : public uncopyable_t
{
    bool call_ = true;

    T func_;

public:

    explicit scope_guard_t(const T &func)
        : func_(func)
    {
        
    }

    explicit scope_guard_t(T &&func)
        : func_(std::move(func))
    {
        
    }

    ~scope_guard_t()
    {
        if(call_)
            func_();
    }

    void dismiss()
    {
        call_ = false;
    }
};

template<typename F, bool ExecuteOnException>
class exception_scope_guard_t : public uncopyable_t
{
    F func_;

    int exceptions_;

public:

    explicit exception_scope_guard_t(const F &func)
        : func_(func), exceptions_(std::uncaught_exceptions())
    {
        
    }

    explicit exception_scope_guard_t(F &&func)
        : func_(std::move(func)),
        exceptions_(std::uncaught_exceptions())
    {
        
    }

    ~exception_scope_guard_t()
    {
        const int now_exceptions = std::uncaught_exceptions();
        if((now_exceptions > exceptions_) == ExecuteOnException)
        {
            func_();
        }
    }
};

struct scope_guard_builder_t            {};
struct scope_guard_on_fail_builder_t    {};
struct scope_guard_on_success_builder_t {};

template<typename Func>
auto operator+(scope_guard_builder_t, Func &&f)
{
    return scope_guard_t<std::decay_t<Func>>(std::forward<Func>(f));
}

template<typename Func>
auto operator+(scope_guard_on_fail_builder_t, Func &&f)
{
    return exception_scope_guard_t<std::decay_t<Func>, true>(std::forward<Func>(f));
}

template<typename Func>
auto operator+(scope_guard_on_success_builder_t, Func &&f)
{
    return exception_scope_guard_t<std::decay_t<Func>, false>(std::forward<Func>(f));
}

#define AGZ_SCOPE_EXIT \
    auto AGZ_ANONYMOUS_NAME(_agz_scope_exit) = \
    ::agz::misc::scope_guard_builder_t{} + [&]

#define AGZ_SCOPE_FAIL \
    auto AGZ_ANONYMOUS_NAME(_agz_scope_fail) = \
    ::agz::misc::scope_guard_on_fail_builder_t{} + [&]

#define AGZ_SCOPE_SUCCESS \
    auto AGZ_ANONYMOUS_NAME(_agz_scope_success) = \
    ::agz::misc::scope_guard_on_success_builder_t{} + [&]

} // namespace agz::misc
