#pragma once

#include <vector>

namespace agz::file
{

/**
 * @brief 读取一个二进制文件的全部内容
 */
std::vector<unsigned char> read_raw_file(const std::string &filename);

/**
 * @brief 一次写入一个二进制文件的全部内容
 * 
 * 若存在原文件，覆盖之
 */
void write_raw_file(const std::string &filename, const void *data, size_t byte_size);

/**
 * @brief 读取一个文本文件的全部内容
 */
std::string read_txt_file(const std::string &filename);

} // namespace agz::file
