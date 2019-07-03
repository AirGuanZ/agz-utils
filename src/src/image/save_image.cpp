#include <cassert>
#include <agz/utility/file/file_raw.h>
#include <agz/utility/image/save_image.h>

//============= includes for stb =============

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cmath>

//============= end includes for stb =============

namespace
{

#if defined(_MSC_VER)
#define STBI_MSC_SECURE_CRT
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

}

namespace agz::img
{

template<typename P>
using image_buffer = math::tensor_t<P, 2>;
    
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

    template<int CH, typename P>
    std::vector<unsigned char> save_to_png_in_memory(const P *data, int w, int h)
    {
        std::vector<unsigned char> ret;
        buffer_context_t bc = { &ret };

        if(!stbi_write_png_to_func(
            buffer_func, &bc,
            w, h,
            CH, data, 0))
        {
            throw std::runtime_error("failed to construct PNG file in memory");
        }

        return ret;
    }
}

std::vector<unsigned char> save_gray_to_png_in_memory(const math::byte *data, int w, int h)
{
    return save_to_png_in_memory<1>(data, w, h);
}

std::vector<unsigned char> save_gray_alpha_to_png_in_memory(const math::color2b *data, int w, int h)
{
    return save_to_png_in_memory<2>(data, w, h);
}

std::vector<unsigned char> save_rgb_to_png_in_memory(const math::color3b *data, int w, int h)
{
    return save_to_png_in_memory<3>(data, w, h);
}

std::vector<unsigned char> save_rgba_to_png_in_memory(const math::color4b *data, int w, int h)
{
    return save_to_png_in_memory<4>(data, w, h);
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

void save_gray_alpha_to_png_file(const std::string &filename, const math::color2b *data, int w, int h)
{
    auto content = save_gray_alpha_to_png_in_memory(data, w, h);
    file::write_raw_file(filename, content.data(), content.size());
}

void save_gray_alpha_to_png_file(const std::string &filename, const image_buffer<math::color2b> &data)
{
    save_gray_alpha_to_png_file(filename, data.raw_data(), data.shape()[1], data.shape()[0]);
}

void save_rgb_to_png_file(const std::string &filename, const math::color3b *data, int w, int h)
{
    auto content = save_rgb_to_png_in_memory(data, w, h);
    file::write_raw_file(filename, content.data(), content.size());
}

void save_rgb_to_png_file(const std::string &filename, const image_buffer<math::color3b> &data)
{
    save_rgb_to_png_file(filename, data.raw_data(), data.shape()[1], data.shape()[0]);
}

void save_rgba_to_png_file(const std::string &filename, const math::color4b *data, int w, int h)
{
    auto content = save_rgba_to_png_in_memory(data, w, h);
    file::write_raw_file(filename, content.data(), content.size());
}

void save_rgba_to_png_file(const std::string &filename, const image_buffer<math::color4b> &data)
{
    save_rgba_to_png_file(filename, data.raw_data(), data.shape()[1], data.shape()[0]);
}

} // namespace agz::img
