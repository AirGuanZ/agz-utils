#pragma once

#include <string>

#include <agz/utility/math.h>

namespace agz::img
{

/**
 * ֧�ֵ��ļ���ʽ��JPEG, PNG, TGA, BMP, GIF, HDR, PIC, PNM
 */
    
/** ���ڴ��ж�ȡ�Ҷ�ͼ��Ҫ��data�����ض���ͼ���ļ���ʽ */
math::tensor_t<math::byte, 2> load_gray_from_memory(const void *data, size_t byte_length);

/** ���ڴ��ж�ȡ�Ҷ� + alphaͼ��Ҫ��data�����ض���ͼ���ļ���ʽ */
math::tensor_t<math::color2b, 2> load_gray_alpha_from_memory(const void *data, size_t byte_length);

/** ���ڴ��ж�ȡrgbͼ��Ҫ��data�����ض���ͼ���ļ���ʽ */
math::tensor_t<math::color3b, 2> load_rgb_from_memory(const void *data, size_t byte_length);

/** ���ڴ��ж�ȡrgbaͼ��Ҫ��data�����ض���ͼ���ļ���ʽ */
math::tensor_t<math::color4b, 2> load_rgba_from_memory(const void *data, size_t byte_length);

/** ���ļ��ж�ȡ�Ҷ�ͼ�� */
math::tensor_t<math::byte, 2> load_gray_from_file(const std::string &filename);

/** ���ļ��ж�ȡ�Ҷ� + alphaͼ�� */
math::tensor_t<math::color2b, 2> load_gray_alpha_from_file(const std::string &filename);

/** ���ļ��ж�ȡrgbͼ�� */
math::tensor_t<math::color3b, 2> load_rgb_from_file(const std::string &filename);

/** ���ļ��ж�ȡrgbaͼ�� */
math::tensor_t<math::color4b, 2> load_rgba_from_file(const std::string &filename);

} // namespace agz::img
