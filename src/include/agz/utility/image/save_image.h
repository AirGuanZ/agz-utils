#pragma once

#include <string>

#include <agz/utility/math/math.h>

namespace agz::img
{
    
std::vector<unsigned char> save_gray_to_png_in_memory(const math::byte *data, int w, int h);

void save_gray_to_png_file(const std::string &filename, const math::byte *data, int w, int h);

} // namespace agz::img
