#pragma once

#include "./common.h"

namespace agz::math
{
    
template<typename T, typename F>
auto mix(const T &left, const T &right, F factor) noexcept(noexcept((1 - factor) * left + factor * right))
{
    return (1 - factor) * left + factor * right;
}

} // namespace agz::math
