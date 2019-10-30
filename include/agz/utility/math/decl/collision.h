#pragma once

#include "./vec3.h"

namespace agz::math
{

/**
 * @brief 球体和AABB之间的碰撞测试
 */
template<typename F>
bool test_sphere_aabb_collision(const tvec3<F> &o, F r, const tvec3<F> &low, const tvec3<F> &high);

} // namespace agz::math
