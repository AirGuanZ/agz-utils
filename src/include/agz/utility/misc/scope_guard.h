#pragma once

#include <functional>

#include "uncopyable.h"

namespace agz::misc
{
    
class scope_guard_t : public uncopyable_t
{
    std::function<void()> func_;

public:

    template<typename T, typename = std::enable_if_t<std::is_invocable_v<T>>>
    explicit scope_guard_t(T &&func)
        : func_(std::forward<T>(func))
    {

    }

    ~scope_guard_t()
    {
        if(func_)
            func_();
    }

    void dismiss()
    {
        func_ = std::function<void()>();
    }
};

/**
 * @brief 定义一个自动命名的scope guard，离开定义作用域时自动调用指定的操作
 * 
 * 由于命名是根据行号自动进行的，很难对其调用dismiss操作。如有此需要，可显式使用 AGZ::ScopeGuard
 */
#define AGZ_SCOPE_GUARD(X)             AGZ_SCOPE_GUARD_IMPL0(X, __LINE__)
#define AGZ_SCOPE_GUARD_IMPL0(X, LINE) AGZ_SCOPE_GUARD_IMPL1(X, LINE)
#define AGZ_SCOPE_GUARD_IMPL1(X, LINE) ::agz::misc::scope_guard_t _auto_scope_guard##LINE([&] X)

} // namespace agz::misc
