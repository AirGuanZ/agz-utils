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
    auto path = absolute(filename);
    if(!path.has_parent_path())
        return;
    path = path.parent_path();
    if(exists(path))
    {
        if(!is_directory(path))
            throw std::runtime_error("failed to create directory " + path.string() + " (exists but is not a directory)");
    }
    else if(!create_directories(path))
        throw std::runtime_error("failed to create direcotry " + path.string());
}

} // namespace agz::file
