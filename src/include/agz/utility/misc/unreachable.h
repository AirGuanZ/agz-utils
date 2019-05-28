#pragma once

#include "./platform.h"

namespace agz::misc
{
    
[[noreturn]] inline void unreachable()
{
#if defined(AGZ_CC_MSVC)
    __assume(0);
#elif defined(AGZ_CC_IS_GNU)
    __builtin_unreachable();
#endif
    std::terminate();
}

} // namespace agz::misc
