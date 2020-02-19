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

struct face_t
{
    bool is_quad = false;
    vertex_t vertices[4];
};

/**
 * @brief 从.obj文件中加载网格
 */
std::vector<face_t> load_from_obj(const std::string &filename);

/**
 * @brief 从内存中加载三角网格
 */
std::vector<triangle_t> load_from_obj_mem(const std::string &str);

/**
 * @brief 从.obj/.stl文件中加载三角网格
 */
std::vector<triangle_t> load_from_file(const std::string &filename);

} // namespace agz::mesh
