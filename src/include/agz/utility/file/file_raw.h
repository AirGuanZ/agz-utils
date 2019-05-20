#pragma once

#include <vector>

namespace agz::file
{

std::vector<unsigned char> read_raw_file(const std::string &filename);

void write_raw_file(const std::string &filename, const void *data, size_t byte_size);

} // namespace agz::file
