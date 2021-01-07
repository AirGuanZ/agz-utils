﻿#pragma once

#include <string>

#include <agz-utils/math.h>

namespace agz::img
{

//######################################## png ########################################

/** @brief 将灰度图以png格式输出到内存中 */
std::vector<unsigned char> save_gray_to_png_in_memory(
    const math::byte *data, int w, int h);

/** @brief 将灰度 + alpha图像以png格式输出到内存中 */
std::vector<unsigned char> save_gray_alpha_to_png_in_memory(
    const math::color2b *data, int w, int h);

/** @brief 将rgb图像以png格式输出到内存中 */
std::vector<unsigned char> save_rgb_to_png_in_memory(
    const math::color3b *data, int w, int h);

/** @brief 将rgba图像以png格式输出到内存中 */
std::vector<unsigned char> save_rgba_to_png_in_memory(
    const math::color4b *data, int w, int h);

/** @brief 将灰度图输出到png文件 */
void save_gray_to_png_file(
    const std::string &filename, const math::byte *data, int w, int h);
/** @brief 将灰度图输出到png文件 */
void save_gray_to_png_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

/** @brief 将灰度 + alpha图输出到png文件 */
void save_gray_alpha_to_png_file(
    const std::string &filename, const math::color2b *data, int w, int h);
/** @brief 将灰度 + alpha图输出到png文件 */
void save_gray_alpha_to_png_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

/** @brief 将rgb图像输出到png文件 */
void save_rgb_to_png_file(
    const std::string &filename, const math::color3b *data, int w, int h);
/** @brief 将rgb图像输出到png文件 */
void save_rgb_to_png_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

/** @brief 将rgba图像输出到png文件 */
void save_rgba_to_png_file(
    const std::string &filename, const math::color4b *data, int w, int h);
/** @brief 将rgba图像输出到png文件 */
void save_rgba_to_png_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

//######################################## jpg ########################################

/** @brief 将灰度图以jpg格式输出到内存中 */
std::vector<unsigned char> save_gray_to_jpg_in_memory(
    const math::byte *data, int w, int h);

/** @brief 将灰度 + alpha图像以jpg格式输出到内存中 */
std::vector<unsigned char> save_gray_alpha_to_jpg_in_memory(
    const math::color2b *data, int w, int h);

/** @brief 将rgb图像以jpg格式输出到内存中 */
std::vector<unsigned char> save_rgb_to_jpg_in_memory(
    const math::color3b *data, int w, int h);

/** @brief 将rgba图像以jpg格式输出到内存中 */
std::vector<unsigned char> save_rgba_to_jpg_in_memory(
    const math::color4b *data, int w, int h);

/** @brief 将灰度图输出到jpg文件 */
void save_gray_to_jpg_file(
    const std::string &filename, const math::byte *data, int w, int h);
/** @brief 将灰度图输出到jpg文件 */
void save_gray_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

/** @brief 将灰度 + alpha图输出到jpg文件 */
void save_gray_alpha_to_jpg_file(
    const std::string &filename, const math::color2b *data, int w, int h);
/** @brief 将灰度 + alpha图输出到jpg文件 */
void save_gray_alpha_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

/** @brief 将rgb图像输出到jpg文件 */
void save_rgb_to_jpg_file(
    const std::string &filename, const math::color3b *data, int w, int h);
/** @brief 将rgb图像输出到jpg文件 */
void save_rgb_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

/** @brief 将rgba图像输出到jpg文件 */
void save_rgba_to_jpg_file(
    const std::string &filename, const math::color4b *data, int w, int h);
/** @brief 将rgba图像输出到jpg文件 */
void save_rgba_to_jpg_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

//######################################## bmp ########################################

/** @brief 将灰度图以bmp格式输出到内存中 */
std::vector<unsigned char> save_gray_to_bmp_in_memory(
    const math::byte *data, int w, int h);

/** @brief 将灰度 + alpha图像以bmp格式输出到内存中 */
std::vector<unsigned char> save_gray_alpha_to_bmp_in_memory(
    const math::color2b *data, int w, int h);

/** @brief 将rgb图像以bmp格式输出到内存中 */
std::vector<unsigned char> save_rgb_to_bmp_in_memory(
    const math::color3b *data, int w, int h);

/** @brief 将rgba图像以bmp格式输出到内存中 */
std::vector<unsigned char> save_rgba_to_bmp_in_memory(
    const math::color4b *data, int w, int h);

/** @brief 将灰度图输出到bmp文件 */
void save_gray_to_bmp_file(
    const std::string &filename, const math::byte *data, int w, int h);
/** @brief 将灰度图输出到bmp文件 */
void save_gray_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::byte, 2> &data);

/** @brief 将灰度 + alpha图输出到bmp文件 */
void save_gray_alpha_to_bmp_file(
    const std::string &filename, const math::color2b *data, int w, int h);
/** @brief 将灰度 + alpha图输出到bmp文件 */
void save_gray_alpha_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

/** @brief 将rgb图像输出到bmp文件 */
void save_rgb_to_bmp_file(
    const std::string &filename, const math::color3b *data, int w, int h);
/** @brief 将rgb图像输出到bmp文件 */
void save_rgb_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

/** @brief 将rgba图像输出到bmp文件 */
void save_rgba_to_bmp_file(
    const std::string &filename, const math::color4b *data, int w, int h);
/** @brief 将rgba图像输出到bmp文件 */
void save_rgba_to_bmp_file(
    const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

//######################################## hdr ########################################

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const float *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::color3f *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::vec3f *data, int w, int h);

std::vector<unsigned char> save_rgb_to_hdr_in_memory(
    const math::tensor_t<math::color3f, 2> &data);

void save_rgb_to_hdr_file(
    const std::string &filename, const float *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::color3f *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::vec3f *data, int w, int h);

void save_rgb_to_hdr_file(
    const std::string &filename, const math::tensor_t<math::color3f, 2> &data);

} // namespace agz::img
