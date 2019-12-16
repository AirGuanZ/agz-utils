#pragma once

#include <filesystem>

namespace agz::file
{
    
/**
 * @brief 为特定文件路径准备包含它的文件夹
 * 
 * 调用成功后保证absolute(filename)的父目录存在
 * 
 * 无法保证时抛异常
 */
inline void create_directory_for_file(const std::filesystem::path &filename)
{
    const auto path = absolute(filename);
    if(!path.has_parent_path())
        return;
    const auto parent_path = path.parent_path();
    if(exists(parent_path))
    {
        if(!is_directory(parent_path))
            throw std::runtime_error("failed to create directory " + parent_path.string() + " (exists but is not a directory)");
    }
    else if(!create_directories(parent_path))
        throw std::runtime_error("failed to create direcotry " + parent_path.string());
}

} // namespace agz::file
