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

image_buffer<math::byte> load_grey_from_memory(const void *data, size_t byte_length)
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

    return image_buffer<math::byte>::from_lineared_indexed_fn(
        { h, w }, [&](int i) { return bytes[i]; });
}

image_buffer<math::byte> load_grey_from_file(const std::string &filename)
{
    auto content = file::read_raw_file(filename);
    return load_grey_from_memory(content.data(), content.size());
}

} // namespace agz::img
