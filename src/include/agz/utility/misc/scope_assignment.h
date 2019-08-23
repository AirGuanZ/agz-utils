#pragma once

#include <cassert>

#include "./uncopyable.h"

namespace agz::misc
{
    
/**
 * @brief 进行一次赋值，并在离开作用域时自动恢复原来的值
 */
template<typename T>
class scope_assignment_t : public uncopyable_t
{
    T *addr_;
    T old_value_;

public:

    scope_assignment_t(T *addr, const T &value)
    {
        assert(addr);
        addr_ = addr;
        old_value_ = *addr;
        *addr = value;
    }

    ~scope_assignment_t()
    {
        if(addr_)
            *addr_ = old_value_;
    }

    void dismiss() noexcept
    {
        addr_ = nullptr;
    }
};

#define AGZ_SCOPE_ASSIGN(ADDR, TARGET)             AGZ_SCOPE_ASSIGN_IMPL0((ADDR), (TARGET), __LINE__)
#define AGZ_SCOPE_ASSIGN_IMPL0(ADDR, TARGET, LINE) AGZ_SCOPE_ASSIGN_IMPL1(ADDR, TARGET, LINE)
#define AGZ_SCOPE_ASSIGN_IMPL1(ADDR, TARGET, LINE) ::agz::misc::scope_assignment_t _auto_scope_assignment##LINE(ADDR, TARGET)

} // namespace agz::misc
