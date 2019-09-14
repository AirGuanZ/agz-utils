#pragma once

#include <string>

namespace agz::sys
{
    
/**
 * @brief 使用系统默认打开方式来打开指定文件
 */
void open_with_default_app(const std::string &filename);

} // namespace agz::sys
