#pragma once

#include <string>

#include "common.h"

namespace agz::img
{
    
image_buffer<math::byte>    load_grey_from_memory      (const void *data, size_t byte_length);
image_buffer<math::color2b> load_grey_alpha_from_memory(const void *data, size_t byte_length);
image_buffer<math::color3b> load_rgb_from_memory       (const void *data, size_t byte_length);

image_buffer<math::byte>    load_grey_from_file      (const std::string &filename);
image_buffer<math::color2b> load_grey_alpha_from_file(const std::string &filename);
image_buffer<math::color3b> load_rgb_from_file       (const std::string &filename);

} // namespace agz::img
