#include <fstream>
#include <sstream>

#include <agz/utility/file/file_raw.h>

std::vector<unsigned char> agz::file::read_raw_file(const std::string &filename)
{
    std::ifstream fin(filename, std::ios::in | std::ios::binary);
    if(!fin)
        return { };

    fin.seekg(0, std::ios::end);
    const auto len = fin.tellg();
    fin.seekg(0, std::ios::beg);

    std::vector<unsigned char> ret(static_cast<size_t>(len));
    fin.read(reinterpret_cast<char*>(ret.data()), len);

    return ret;
}

void agz::file::write_raw_file(const std::string &filename, const void *data, size_t byte_size)
{
    std::ofstream fout(filename, std::ios::binary | std::ios::trunc);
    if(!fout)
        throw std::runtime_error("failed to open file: " + filename);
    fout.write(reinterpret_cast<const char*>(data), byte_size);
    fout.close();
}

std::string agz::file::read_txt_file(const std::string &filename)
{
    std::ifstream fin(filename, std::ios_base::in);
    if(!fin)
        throw std::runtime_error("failed to open text file: " + filename);
    std::stringstream sst;
    sst << fin.rdbuf();
    return sst.str();
}
