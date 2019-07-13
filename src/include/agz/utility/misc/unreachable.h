#pragma once

#include "../system/platform.h"

namespace agz::misc
{
    
/** @brief ������ʾ����������һ�����ɵ����֧ */
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