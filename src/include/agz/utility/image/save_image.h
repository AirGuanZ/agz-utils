#pragma once

#include <string>

#include "common.h"

namespace agz::img
{

/** @brief 将灰度图以png格式输出到内存中 */
std::vector<unsigned char> save_gray_to_png_in_memory(const math::byte *data, int w, int h);

/** @brief 将灰度 + alpha图像以png格式输出到内存中 */
std::vector<unsigned char> save_gray_alpha_to_png_in_memory(const math::color2b *data, int w, int h);

/** @brief 将rgb图像以png格式输出到内存中 */
std::vector<unsigned char> save_rgb_to_png_in_memory(const math::color3b *data, int w, int h);

/** @brief 将rgba图像以png格式输出到内存中 */
std::vector<unsigned char> save_rgba_to_png_in_memory(const math::color4b *data, int w, int h);

/** @brief 将灰度图输出到png文件 */
void save_gray_to_png_file(const std::string &filename, const math::byte *data, int w, int h);
/** @brief 将灰度图输出到png文件 */
void save_gray_to_png_file(const std::string &filename, const math::tensor_t<math::byte, 2> &data);

/** @brief 将灰度 + alpha图输出到png文件 */
void save_gray_alpha_to_png_file(const std::string &filename, const math::color2b *data, int w, int h);
/** @brief 将灰度 + alpha图输出到png文件 */
void save_gray_alpha_to_png_file(const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

/** @brief 将rgb图像输出到png文件 */
void save_rgb_to_png_file(const std::string &filename, const math::color3b *data, int w, int h);
/** @brief 将rgb图像输出到png文件 */
void save_rgb_to_png_file(const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

/** @brief 将rgba图像输出到png文件 */
void save_rgba_to_png_file(const std::string &filename, const math::color4b *data, int w, int h);
/** @brief 将rgba图像输出到png文件 */
void save_rgba_to_png_file(const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

} // namespace agz::img
