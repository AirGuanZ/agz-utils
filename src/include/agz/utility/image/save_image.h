#pragma once

#include <string>

#include "common.h"

namespace agz::img
{
    
std::vector<unsigned char> save_gray_to_png_in_memory      (const math::byte *data, int w, int h);
std::vector<unsigned char> save_gray_alpha_to_png_in_memory(const math::color2b *data, int w, int h);
std::vector<unsigned char> save_rgb_to_png_in_memory       (const math::color3b *data, int w, int h);
std::vector<unsigned char> save_rgba_to_png_in_memory      (const math::color4b *data, int w, int h);

void save_gray_to_png_file(const std::string &filename, const math::byte *data, int w, int h);
void save_gray_to_png_file(const std::string &filename, const image_buffer<math::byte> &data);

void save_gray_alpha_to_png_file(const std::string &filename, const math::color2b *data, int w, int h);
void save_gray_alpha_to_png_file(const std::string &filename, const image_buffer<math::color2b> &data);

void save_rgb_to_png_file(const std::string &filename, const math::color3b *data, int w, int h);
void save_rgb_to_png_file(const std::string &filename, const image_buffer<math::color3b> &data);

void save_rgba_to_png_file(const std::string &filename, const math::color4b *data, int w, int h);
void save_rgba_to_png_file(const std::string &filename, const image_buffer<math::color4b> &data);

} // namespace agz::img
