#pragma once

#include <filesystem>

namespace agz::file
{
    
/**
 * @brief Ϊ�ض��ļ�·��׼�����������ļ���
 * 
 * ���óɹ���֤absolute(filename)�ĸ�Ŀ¼����
 * 
 * �޷���֤ʱ���쳣
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
