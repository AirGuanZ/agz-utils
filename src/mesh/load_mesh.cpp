#include <agz/utility/file.h>
#include <agz/utility/mesh/load_mesh.h>
#include <agz/utility/string.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "./stl_reader.h"
#include "./tiny_obj_loader.h"

namespace agz::mesh
{
    
static std::vector<triangle_t> load_obj(const std::string &filename)
{
    std::string src = file::read_txt_file(filename);
    stdstr::replace_(src, "\\\n", " ");
    stdstr::replace_(src, "\\\r\n", " ");

    tinyobj::ObjReader reader;
    if(!reader.ParseFromString(src, ""))
        throw std::runtime_error(reader.Error());

    auto &attrib = reader.GetAttrib();

    auto get_pos = [&](size_t index)
    {
        if(3 * index + 2 >= attrib.vertices.size())
            throw std::runtime_error("invalid obj vertex index: out of range");
        return math::vec3f(attrib.vertices[3 * index + 0],
                           attrib.vertices[3 * index + 1],
                           attrib.vertices[3 * index + 2]);
    };

    auto get_nor = [&](size_t index)
    {
        if(3 * index + 2 >= attrib.normals.size())
            throw std::runtime_error("invalid obj normal index: out of range");
        return math::vec3f(attrib.normals[3 * index + 0],
                           attrib.normals[3 * index + 1],
                           attrib.normals[3 * index + 2]);
    };

    auto get_uv = [&](size_t index)
    {
        if(2 * index + 1 >= attrib.texcoords.size())
            throw std::runtime_error("invalid obj texcoord index: out of range");
        return math::vec2f(attrib.texcoords[2 * index + 0],
                           attrib.texcoords[2 * index + 1]);
    };

    std::vector<triangle_t> build_triangles;

    for(auto &shape : reader.GetShapes())
    {
        for(auto face_vertex_count : shape.mesh.num_face_vertices)
        {
            if(face_vertex_count != 3)
                throw std::runtime_error("invalid obj face vertex count: " + std::to_string(+face_vertex_count));
        }

        if(shape.mesh.indices.size() % 3 != 0)
            throw std::runtime_error("invalid obj index count: " + std::to_string(shape.mesh.indices.size()));

        build_triangles.reserve(build_triangles.size() + shape.mesh.indices.size() / 3);
        size_t triangle_count = shape.mesh.indices.size() / 3;

        for(size_t i = 0, j = 0; i < triangle_count; ++i, j += 3)
        {
            triangle_t tri;
            auto &vtx = tri.vertices;

            for(size_t k = 0; k < 3; ++k)
                vtx[k].position = get_pos(shape.mesh.indices[j + k].vertex_index);

            for(size_t k = 0; k < 3; ++k)
            {
                auto &idx = shape.mesh.indices[j + k];

                if(idx.normal_index < 0)
                    vtx[k].normal = cross(vtx[1].position - vtx[0].position, vtx[2].position - vtx[0].position).normalize();
                else
                {
                    vtx[k].normal = get_nor(idx.normal_index);
                    if(!vtx[k].normal)
                        vtx[k].normal = cross(vtx[1].position - vtx[0].position, vtx[2].position - vtx[0].position).normalize();
                }

                if(idx.texcoord_index < 0)
                    vtx[k].tex_coord = math::vec2f();
                else
                    vtx[k].tex_coord = get_uv(idx.texcoord_index);
            }

            build_triangles.push_back(tri);
        }
    }

    return build_triangles;
}

static std::vector<triangle_t> load_stl(const std::string &filename)
{
    std::vector<triangle_t> ret;

    stl_reader::StlMesh<float, int> mesh(filename);
    for(size_t i = 0; i < mesh.num_tris(); ++i)
    {
        triangle_t tri;
        auto &vtx = tri.vertices;
        for(int k = 0; k < 3; ++k)
        {
            auto c = mesh.vrt_coords(mesh.tri_corner_ind(i, k));
            vtx[k].position[0] = c[0];
            vtx[k].position[1] = c[1];
            vtx[k].position[2] = c[2];
        }
        for(int k = 0; k < 3; ++k)
            vtx[k].normal = cross(vtx[1].position - vtx[0].position, vtx[2].position - vtx[0].position).normalize();

        ret.push_back(tri);
    }

    return ret;
}

std::vector<triangle_t> load_from_file(const std::string &filename)
{
    if(stdstr::ends_with(filename, ".obj"))
        return load_obj(filename);
    if(stdstr::ends_with(filename, ".stl"))
        return load_stl(filename);
    throw std::runtime_error("unsupported mesh file: " + filename);
}

} // namespace agz::mesh