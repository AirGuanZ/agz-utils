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

struct mesh_t
{
    std::string name;
    std::vector<triangle_t> triangles;
};

struct face_t
{
    bool is_quad = false;
    vertex_t vertices[4];
};

/**
 * @brief 计算一组顶点的包围盒
 */
math::aabb3f compute_bounding_box(const vertex_t *vertices, size_t verte_count);

std::vector<vertex_t> triangle_to_vertex(const std::vector<triangle_t> &triangles);

/**
 * @brief 从.obj文件中加载网格
 */
std::vector<face_t> load_from_obj(const std::string &filename);

/**
 * @brief 从内存中加载三角网格
 */
std::vector<triangle_t> load_from_obj_mem(const std::string &str);

/**
 * @brief 从内存中解析obj格式，加载网格对象
 */
std::vector<mesh_t> load_meshes_from_obj_mem(const std::string &str);

/**
 * @brief 从.obj中加载网格对象
 */
std::vector<mesh_t> load_meshes_from_obj(const std::string &filename);

/**
 * @brief 从内存中解析ply格式，加载网格对象
 */
std::vector<triangle_t> load_from_ply_mem(
    const std::vector<uint8_t> &byte_buffer);

/**
 * @brief 从ply文件中加载网格
 */
std::vector<triangle_t> load_from_ply(const std::string &filename);

/**
 * @brief 从.obj/.stl/.ply文件中加载三角网格
 */
std::vector<triangle_t> load_from_file(const std::string &filename);

} // namespace agz::mesh
