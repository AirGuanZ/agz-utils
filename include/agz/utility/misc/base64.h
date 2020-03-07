#pragma once

#include <string>
#include <vector>

namespace agz::misc
{

std::string encode_base64(const void *data, size_t byte_size);

std::vector<unsigned char> decode_base64(std::string_view str);

} // namespace agz::misc
