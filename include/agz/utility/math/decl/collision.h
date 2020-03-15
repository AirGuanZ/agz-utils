#pragma once

#include "./vec3.h"

namespace agz::math
{

/**
 * @brief 计算一系列点的包围球
 *
 * 返回球心位置和半径
 */
template<typename F>
std::pair<tvec3<F>, F> compute_bounding_sphere(
    const tvec3<F> *vertices, size_t vertex_count);

/**
 * @brief 球体和AABB之间的碰撞测试
 */
template<typename F>
bool test_sphere_aabb_collision(
    const tvec3<F> &o, F r, const tvec3<F> &low, const tvec3<F> &high);

} // namespace agz::math
