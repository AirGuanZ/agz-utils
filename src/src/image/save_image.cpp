#include <cassert>

#include <agz/utility/file/file_raw.h>
#include <agz/utility/image/save_image.h>

namespace
{

#if defined(_MSC_VER)
#define STBI_MSC_SECURE_CRT
#endif

#define STBIW_ASSERT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

}

namespace agz::img
{
    
namespace
{
    struct buffer_context_t
    {
        std::vector<unsigned char> *data = nullptr;
    };

    void buffer_func(void *context, void *data, int size)
    {
        auto bc = reinterpret_cast<buffer_context_t*>(context);
        assert(bc && bc->data);
        size_t old_size = bc->data->size();
        bc->data->resize(old_size + size);
        std::memcpy(bc->data->data() + old_size, data, size);
    }
}

std::vector<unsigned char> save_gray_to_png_in_memory(const math::byte *data, int w, int h)
{
    std::vector<unsigned char> ret;
    buffer_context_t bc = { &ret };

    if(!stbi_write_png_to_func(
        buffer_func, &bc,
        w, h,
        3, data, 0))
    {
        throw std::runtime_error("failed to construct PNG file in memory");
    }

    return ret;
}

void save_gray_to_png_file(const std::string &filename, const math::byte *data, int w, int h)
{
    auto content = save_gray_to_png_in_memory(data, w, h);
    file::write_raw_file(filename, content.data(), content.size());
}

void save_gray_to_png_file(const std::string &filename, const image_buffer<math::byte> &data)
{
    save_gray_to_png_file(filename, data.raw_data(), data.shape()[1], data.shape()[0]);
}

} // namespace agz::img
