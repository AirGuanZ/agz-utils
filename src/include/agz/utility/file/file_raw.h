#pragma once

#include <vector>

namespace agz::file
{

/**
 * @brief ��ȡһ���������ļ���ȫ������
 */
std::vector<unsigned char> read_raw_file(const std::string &filename);

/**
 * @brief һ��д��һ���������ļ���ȫ������
 * 
 * ������ԭ�ļ�������֮
 */
void write_raw_file(const std::string &filename, const void *data, size_t byte_size);

/**
 * @brief ��ȡһ���ı��ļ���ȫ������
 */
std::string read_txt_file(const std::string &filename);

} // namespace agz::file
