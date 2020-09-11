#pragma once

#include "./vec2.h"

namespace agz::math::lowd
{

inline float hammersleyRadicalInverse(uint32_t bits) noexcept;

inline vec2f hammersley2D(uint32_t i, uint32_t N) noexcept;

} // namespace agz::math::lowd
