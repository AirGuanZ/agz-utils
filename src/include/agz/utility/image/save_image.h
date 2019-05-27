#pragma once

#include <string>

#include "common.h"

namespace agz::img
{

/** @brief ���Ҷ�ͼ��png��ʽ������ڴ��� */
std::vector<unsigned char> save_gray_to_png_in_memory(const math::byte *data, int w, int h);

/** @brief ���Ҷ� + alphaͼ����png��ʽ������ڴ��� */
std::vector<unsigned char> save_gray_alpha_to_png_in_memory(const math::color2b *data, int w, int h);

/** @brief ��rgbͼ����png��ʽ������ڴ��� */
std::vector<unsigned char> save_rgb_to_png_in_memory(const math::color3b *data, int w, int h);

/** @brief ��rgbaͼ����png��ʽ������ڴ��� */
std::vector<unsigned char> save_rgba_to_png_in_memory(const math::color4b *data, int w, int h);

/** @brief ���Ҷ�ͼ�����png�ļ� */
void save_gray_to_png_file(const std::string &filename, const math::byte *data, int w, int h);
/** @brief ���Ҷ�ͼ�����png�ļ� */
void save_gray_to_png_file(const std::string &filename, const math::tensor_t<math::byte, 2> &data);

/** @brief ���Ҷ� + alphaͼ�����png�ļ� */
void save_gray_alpha_to_png_file(const std::string &filename, const math::color2b *data, int w, int h);
/** @brief ���Ҷ� + alphaͼ�����png�ļ� */
void save_gray_alpha_to_png_file(const std::string &filename, const math::tensor_t<math::color2b, 2> &data);

/** @brief ��rgbͼ�������png�ļ� */
void save_rgb_to_png_file(const std::string &filename, const math::color3b *data, int w, int h);
/** @brief ��rgbͼ�������png�ļ� */
void save_rgb_to_png_file(const std::string &filename, const math::tensor_t<math::color3b, 2> &data);

/** @brief ��rgbaͼ�������png�ļ� */
void save_rgba_to_png_file(const std::string &filename, const math::color4b *data, int w, int h);
/** @brief ��rgbaͼ�������png�ļ� */
void save_rgba_to_png_file(const std::string &filename, const math::tensor_t<math::color4b, 2> &data);

} // namespace agz::img
