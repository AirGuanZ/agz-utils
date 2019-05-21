#include <agz/utility/file/file_raw.h>
#include <agz/utility/image/load_image.h>

namespace
{

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

}

namespace agz::img
{

image_buffer<math::byte> load_gray_from_memory(const void *data, size_t byte_length)
{
    int w, h, channels;
    unsigned char *bytes = stbi_load_from_memory(
        static_cast<const stbi_uc*>(data),
        static_cast<int>(byte_length),
        &w, &h, &channels, STBI_grey);
    if(!bytes)
        return image_buffer<math::byte>();

    AGZ_SCOPE_GUARD({ stbi_image_free(bytes); });

    assert(w > 0 && h > 0);

    return image_buffer<math::byte>::from_linear_indexed_fn(
        { h, w }, [&](int i) { return bytes[i]; });
}

namespace img_impl
{

    template<typename P, int CH, int STBchannel>
    image_buffer<P> load_from_memory(const void *data, size_t byte_length)
    {
        int w, h, channels;
        unsigned char *bytes = stbi_load_from_memory(
            static_cast<const stbi_uc*>(data),
            static_cast<int>(byte_length),
            &w, &h, &channels, STBchannel);
        if(!bytes)
            return image_buffer<P>();

        AGZ_SCOPE_GUARD({ stbi_image_free(bytes); });

        assert(w > 0 && h > 0);

        int idx = 0;

        return image_buffer<P>::from_linear_indexed_fn(
            { h, w }, [&](int i)
        {
            P ret(UNINIT);
            for(int j = 0; j < CH; ++j)
                ret[j] = bytes[idx + j];
            idx += CH;
            return ret;
        });
    }

} // namespace img_impl

image_buffer<math::color2b> load_gray_alpha_from_memory(const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color2b, 2, STBI_grey_alpha>(data, byte_length);
}

image_buffer<math::color3b> load_rgb_from_memory(const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color3b, 3, STBI_rgb>(data, byte_length);
}

image_buffer<math::color4b> load_rgba_from_memory(const void *data, size_t byte_length)
{
    return img_impl::load_from_memory<math::color4b, 4, STBI_rgb_alpha>(data, byte_length);
}

image_buffer<math::byte> load_gray_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_gray_from_memory(content.data(), content.size());
}

image_buffer<math::color2b> load_gray_alpha_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_gray_alpha_from_memory(content.data(), content.size());
}

image_buffer<math::color3b> load_rgb_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_rgb_from_memory(content.data(), content.size());
}

image_buffer<math::color4b> load_rgba_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_rgba_from_memory(content.data(), content.size());
}

} // namespace agz::img
