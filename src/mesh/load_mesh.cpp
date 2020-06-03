#include <agz/utility/file.h>
#include <agz/utility/mesh/load_mesh.h>
#include <agz/utility/string.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "./tinyply.h"
#include "./stl_reader.h"
#include "./tiny_obj_loader.h"

namespace agz::mesh
{

static std::vector<mesh_t> parse_meshes_from_obj(const std::string &_src)
{
    std::string src = _src;
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

    std::vector<mesh_t> meshes;

    for(auto &shape : reader.GetShapes())
    {
        for(auto face_vertex_count : shape.mesh.num_face_vertices)
        {
            if(face_vertex_count != 3)
            {
                throw std::runtime_error(
                    "invalid obj face vertex count: " +
                    std::to_string(+face_vertex_count));
            }
        }

        if(shape.mesh.indices.size() % 3 != 0)
        {
            throw std::runtime_error(
                "invalid obj index count: " +
                std::to_string(shape.mesh.indices.size()));
        }

        const size_t triangle_count = shape.mesh.indices.size() / 3;

        meshes.emplace_back();
        auto &mesh = meshes.back();

        mesh.name = shape.name;
        mesh.triangles.reserve(triangle_count);

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
                    vtx[k].normal = cross(
                        vtx[1].position - vtx[0].position,
                        vtx[2].position - vtx[0].position).normalize();
                else
                {
                    vtx[k].normal = get_nor(idx.normal_index);
                    if(!vtx[k].normal)
                        vtx[k].normal = cross(
                            vtx[1].position - vtx[0].position,
                            vtx[2].position - vtx[0].position).normalize();
                }

                if(idx.texcoord_index < 0)
                    vtx[k].tex_coord = math::vec2f();
                else
                    vtx[k].tex_coord = get_uv(idx.texcoord_index);
            }

            mesh.triangles.push_back(tri);
        }
    }

    return meshes;
}

static std::vector<triangle_t> load_triangles_from_stl(
    const std::string &filename)
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
            vtx[k].normal = cross(
                vtx[1].position - vtx[0].position,
                vtx[2].position - vtx[0].position).normalize();

        ret.push_back(tri);
    }

    return ret;
}

math::aabb3f compute_bounding_box(const vertex_t *vertices, size_t verte_count)
{
    return std::accumulate(
        vertices, vertices + verte_count, math::aabb3f{},
        [](const auto &a, const auto &b) { return a | b.position; });
}

std::vector<vertex_t> triangle_to_vertex(const std::vector<triangle_t> &triangles)
{
    std::vector<vertex_t> ret;
    for(auto &t : triangles)
    {
        ret.push_back(t.vertices[0]);
        ret.push_back(t.vertices[1]);
        ret.push_back(t.vertices[2]);
    }
    return ret;
}

std::vector<face_t> load_from_obj(const std::string &filename)
{
    std::string src = file::read_txt_file(filename);
    stdstr::replace_(src, "\\\n", " ");
    stdstr::replace_(src, "\\\r\n", " ");

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate = false;
    if(!reader.ParseFromString(src, "", reader_config))
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

    std::vector<face_t> build_faces;

    for(auto &shape : reader.GetShapes())
    {
        int cur_idx = 0;

        for(auto face_vertex_count : shape.mesh.num_face_vertices)
        {
            if(face_vertex_count != 3 && face_vertex_count != 4)
                throw std::runtime_error("invalid obj face vertex count: " +
                            std::to_string(face_vertex_count));

            face_t face;
            face.is_quad = face_vertex_count == 4;

            for(int i = 0; i < face_vertex_count; ++i)
            {
                auto &idx = shape.mesh.indices[cur_idx + i];
                face.vertices[i].position = get_pos(idx.vertex_index);
            }

            for(int i = 0; i < face_vertex_count; ++i)
            {
                auto &idx = shape.mesh.indices[cur_idx + i];
                if(idx.normal_index < 0)
                {
                    face.vertices[i].normal = cross(
                        face.vertices[1].position - face.vertices[0].position,
                        face.vertices[2].position - face.vertices[0].position).normalize();
                }
                else
                {
                    face.vertices[i].normal = get_nor(idx.normal_index);
                    if(!face.vertices[i].normal)
                    {
                        face.vertices[i].normal = cross(
                            face.vertices[1].position - face.vertices[0].position,
                            face.vertices[2].position - face.vertices[0].position).normalize();
                    }
                }

                if(idx.texcoord_index < 0)
                    face.vertices[i].tex_coord = math::vec2f();
                else
                    face.vertices[i].tex_coord = get_uv(idx.texcoord_index); 
            }

            build_faces.push_back(face);
            cur_idx += face_vertex_count;
        }
    }

    return build_faces;
}

static std::vector<triangle_t> load_ply(std::vector<uint8_t> byte_buffer)
{
    struct memory_buffer : public std::streambuf
    {
        char * p_start {nullptr};
        char * p_end {nullptr};
        size_t size;
    
        memory_buffer(char const * first_elem, size_t size)
            : p_start(const_cast<char*>(first_elem)),
              p_end(p_start + size), size(size)
        {
            setg(p_start, p_start, p_end);
        }
    
        pos_type seekoff(
            off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode which) override
        {
            if(dir == std::ios_base::cur)
                gbump(static_cast<int>(off));
            else
                setg(
                    p_start,
                    (dir == std::ios_base::beg ? p_start : p_end) + off, p_end);
            return gptr() - p_start;
        }
    
        pos_type seekpos(pos_type pos, std::ios_base::openmode which) override
        {
            return seekoff(pos, std::ios_base::beg, which);
        }
    };
    
    struct memory_stream : virtual memory_buffer, public std::istream
    {
        memory_stream(char const * first_elem, size_t size)
            : memory_buffer(first_elem, size),
              std::istream(static_cast<std::streambuf*>(this)) {}
    };

    memory_stream file_stream(
        reinterpret_cast<char*>(byte_buffer.data()), byte_buffer.size());
    file_stream.seekg(0, std::ios::beg);
    
    tinyply::PlyFile file;
    file.parse_header(file_stream);
    
    std::shared_ptr<tinyply::PlyData> vertices, normals, texcoords, faces;
    
    vertices = file.request_properties_from_element("vertex", { "x", "y", "z" });

    try { normals   = file.request_properties_from_element(
        "vertex", { "nx", "ny", "nz" }); }  catch(...) { }
    try { texcoords = file.request_properties_from_element(
        "vertex", { "s", "t" }); }          catch(...) { }
    try { faces     = file.request_properties_from_element(
        "face", { "vertex_indices" }, 3); } catch(...) {}
    
    file.read(file_stream);
    
    if(!vertices)
        throw std::runtime_error("vertex data not found in ply");
    
    if(vertices->count <= 0)
        throw std::runtime_error(
            "invalid vertex count in ply: " +std::to_string(vertices->count));

    std::vector<vertex_t> all_vertices(vertices->count);
    
    if(vertices->t != tinyply::Type::FLOAT32)
        throw std::runtime_error("invalid vertex element type in ply: not float32");
    
    std::vector<math::vec3f> position_data(
        vertices->buffer.size_bytes() / sizeof(math::vec3f));
    std::memcpy(
        position_data.data(), vertices->buffer.get(),
        vertices->buffer.size_bytes());
    for(size_t i = 0; i < position_data.size(); ++i)
        all_vertices[i].position = position_data[i];
    
    if(normals)
    {
        if(normals->count != vertices->count)
            throw std::runtime_error("normals.count != vertices.count in ply");
        
        std::vector<math::vec3f> normal_data(normals->count);
        std::memcpy(
            normal_data.data(), normals->buffer.get(), normals->buffer.size_bytes());
        for(size_t i = 0; i < normal_data.size(); ++i)
            all_vertices[i].normal = normal_data[i];
    }
    
    if(texcoords)
    {
        if(texcoords->count != vertices->count)
            throw std::runtime_error("texcoords.count != vertices.count in ply");
        
        std::vector<math::vec2f> texcoord_data(texcoords->count);
        std::memcpy(
            texcoord_data.data(), texcoords->buffer.get(), texcoords->buffer.size_bytes());
        for(size_t i = 0; i < texcoord_data.size(); ++i)
            all_vertices[i].tex_coord = texcoord_data[i];
    }

    std::vector<triangle_t> triangles;

    if(faces)
    {
        if(faces->t != tinyply::Type::INT32 && faces->t != tinyply::Type::UINT32)
            throw std::runtime_error("invalid face index type: not int32");

        std::vector<math::vec3i> face_data(faces->count);
        std::memcpy(face_data.data(), faces->buffer.get(), faces->buffer.size_bytes());
        for(auto &f : face_data)
        {
            triangle_t tri;
            tri.vertices[0] = all_vertices[f[0]];
            tri.vertices[1] = all_vertices[f[1]];
            tri.vertices[2] = all_vertices[f[2]];

            if(!tri.vertices[0].normal ||
               !tri.vertices[1].normal ||
               !tri.vertices[2].normal)
            {
                math::vec3f nor = cross(
                    tri.vertices[1].position - tri.vertices[0].position,
                    tri.vertices[2].position - tri.vertices[0].position).normalize();

                for(int i = 0; i < 3; ++i)
                    tri.vertices[i].normal = nor;
            }

            triangles.push_back(tri);
        }
    }
    else
    {
        if(all_vertices.size() % 3 != 0)
            throw std::runtime_error("vertex count % 3 != 0 in ply");

        for(size_t i = 0; i < all_vertices.size(); i += 3)
        {
            triangle_t tri;
            tri.vertices[0] = all_vertices[i];
            tri.vertices[1] = all_vertices[i + 1];
            tri.vertices[2] = all_vertices[i + 2];

            if(!tri.vertices[0].normal || !tri.vertices[1].normal ||
               !tri.vertices[2].normal)
            {
                math::vec3f nor = cross(
                    tri.vertices[1].position - tri.vertices[0].position,
                    tri.vertices[2].position - tri.vertices[0].position).normalize();

                for(int j = 0; j < 3; ++j)
                    tri.vertices[i].normal = nor;
            }

            triangles.push_back(tri);
        }
    }
    
    return triangles;
}

std::vector<triangle_t> load_from_obj_mem(const std::string &str)
{
    auto meshes = parse_meshes_from_obj(str);

    size_t total_triangle_count = 0;
    for(auto &mesh : meshes)
        total_triangle_count += mesh.triangles.size();

    std::vector<triangle_t> ret;
    ret.reserve(total_triangle_count);

    for(auto &mesh : meshes)
    {
        for(auto &tri : mesh.triangles)
            ret.push_back(tri);
    }

    return ret;
}

std::vector<mesh_t> load_meshes_from_obj_mem(const std::string &str)
{
    return parse_meshes_from_obj(str);
}

std::vector<mesh_t> load_meshes_from_obj(const std::string &filename)
{
    return load_meshes_from_obj_mem(file::read_txt_file(filename));
}

std::vector<triangle_t> load_from_ply_mem(const std::vector<uint8_t> &byte_buffer)
{
    return load_ply(byte_buffer);
}

std::vector<triangle_t> load_from_ply(const std::string &filename)
{
    return load_from_ply_mem(file::read_raw_file(filename));
}

std::vector<triangle_t> load_from_file(const std::string &filename)
{
    if(stdstr::ends_with(filename, ".obj"))
        return load_from_obj_mem(file::read_txt_file(filename));
    if(stdstr::ends_with(filename, ".stl"))
        return load_triangles_from_stl(filename);
    if(stdstr::ends_with(filename, ".ply"))
        return load_from_ply(filename);
    throw std::runtime_error("unsupported mesh file: " + filename);
}

} // namespace agz::mesh
