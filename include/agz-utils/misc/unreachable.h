#pragma once

#include "../system/platform.h"

namespace agz::misc
{
    
/** @brief 用于提示编译器这是一条不可到达分支 */
[[noreturn]] inline void unreachable()
{
#if defined(AGZ_CC_MSVC)
    __assume(0);
#elif defined(AGZ_CC_IS_GNU)
    __builtin_unreachable();
#else
    std::terminate();
#endif
}

} // namespace agz::misc
