#pragma once

#include "../math.h"

namespace agz::mesh
{

struct vertex_t
{
    math::vec3f position;
    math::vec3f normal;
    math::vec2f tex_coord;
};

struct triangle_t
{
    vertex_t vertices[3];
};

/**
 * @brief 从.obj/.stl文件中加载三角网格
 */
std::vector<triangle_t> load_from_file(const std::string &filename);

} // namespace agz::mesh
