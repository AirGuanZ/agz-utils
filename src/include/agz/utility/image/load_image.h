#pragma once

#include <string>

#include <agz/utility/math.h>

namespace agz::img
{

/**
 * 支持的文件格式：JPEG, PNG, TGA, BMP, GIF, HDR, PIC, PNM
 */
    
/** 从内存中读取灰度图像，要求data具有特定的图像文件格式 */
math::tensor_t<math::byte, 2> load_gray_from_memory(const void *data, size_t byte_length);

/** 从内存中读取灰度 + alpha图像，要求data具有特定的图像文件格式 */
math::tensor_t<math::color2b, 2> load_gray_alpha_from_memory(const void *data, size_t byte_length);

/** 从内存中读取rgb图像，要求data具有特定的图像文件格式 */
math::tensor_t<math::color3b, 2> load_rgb_from_memory(const void *data, size_t byte_length);

/** 从内存中读取rgba图像，要求data具有特定的图像文件格式 */
math::tensor_t<math::color4b, 2> load_rgba_from_memory(const void *data, size_t byte_length);

/** 从文件中读取灰度图像 */
math::tensor_t<math::byte, 2> load_gray_from_file(const std::string &filename);

/** 从文件中读取灰度 + alpha图像 */
math::tensor_t<math::color2b, 2> load_gray_alpha_from_file(const std::string &filename);

/** 从文件中读取rgb图像 */
math::tensor_t<math::color3b, 2> load_rgb_from_file(const std::string &filename);

/** 从文件中读取rgba图像 */
math::tensor_t<math::color4b, 2> load_rgba_from_file(const std::string &filename);

/** 从hdr文件中读取rgb图像 */
math::tensor_t<math::color3f, 2> load_rgb_from_hdr_file(const std::string &filename);

} // namespace agz::img
