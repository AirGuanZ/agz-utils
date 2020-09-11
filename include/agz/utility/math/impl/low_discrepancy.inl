#pragma once

namespace agz::math::lowd
{

inline float hammersleyRadicalInverse(uint32_t bits) noexcept
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return bits * 2.3283064365386963e-10f;
}

inline vec2f hammersley2D(uint32_t i, uint32_t N) noexcept
{
    return vec2f(static_cast<float>(i) / N, hammersleyRadicalInverse(i));
}

} // namespace agz::math::lowd
