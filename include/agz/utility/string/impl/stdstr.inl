#pragma once

#include <cctype>
#include <iterator>
#include <sstream>

#include "../../misc/type_list.h"

namespace agz::stdstr
{

inline char is_lower(char c) noexcept
{
    return 'a' <= c && c <= 'z';
}

inline char is_upper(char c) noexcept
{
    return 'A' <= c && c <= 'Z';
}

inline char to_lower(char c) noexcept
{
    return is_upper(c) ? c - 'A' + 'a' : c;
}

inline char to_upper(char c) noexcept
{
    return  is_lower(c) ? c - 'a' + 'A' : c;
}

inline void to_lower_(std::string &s) noexcept
{
    for(char &c : s)
        c = to_lower(c);
}

inline void to_upper_(std::string &s) noexcept
{
    for(char &c : s)
        c = to_upper(c);
}

inline std::string to_lower(std::string_view str) noexcept
{
    std::string ret(str);
    to_lower_(ret);
    return ret;
}

inline std::string to_upper(std::string_view str) noexcept
{
    std::string ret(str);
    to_upper_(ret);
    return ret;
}

inline bool starts_with(std::string_view str, std::string_view prefix) noexcept
{
    return str.substr(0, prefix.size()) == prefix;
}

inline bool ends_with(std::string_view str, std::string_view suffix) noexcept
{
    if(str.size() < suffix.size())
        return false;
    return str.substr(str.size() - suffix.size()) == suffix;
}

inline void trim_left_(std::string &str)
{
    if(str.empty())
        return;

    auto it = str.begin();
    while(it != str.end())
    {
        if(!std::isspace(*it))
            break;
        ++it;
    }

    str.erase(str.begin(), it);
}

inline void trim_right_(std::string &str)
{
    if(str.empty())
        return;

    auto it = str.end();
    do
    {
        if(!std::isspace(*(it - 1)))
            break;
        --it;
    } while(it != str.begin());

    str.erase(it, str.end());
}

inline void trim_(std::string &str)
{
    trim_left_(str);
    trim_right_(str);
}

inline std::string trim_left(std::string_view str)
{
    std::string ret(str);
    trim_left_(ret);
    return ret;
}

inline std::string trim_right(std::string_view str)
{
    std::string ret(str);
    trim_right_(ret);
    return ret;
}

inline std::string trim(std::string_view str)
{
    std::string ret(str);
    trim_(ret);
    return ret;
}

template<typename TIt>
std::string join(char joiner, TIt begin, TIt end)
{
    if(begin == end)
        return std::string();

    std::string ret = *begin++;
    while(begin != end)
    {
        ret.push_back(joiner);
        ret.append(*begin++);
    }

    return ret;
}

template<typename TIt>
std::string join(std::string_view joiner, TIt begin, TIt end)
{
    if(begin == end)
        return std::string();

    std::string ret = *begin++;
    while(begin != end)
    {
        ret.append(joiner);
        ret.append(*begin++);
    }

    return ret;
}

template<typename TIt>
size_t split(
    std::string_view src, char splitter, TIt out_iterator, bool rm_empty_result)
{
    size_t beg = 0, ret = 0;
    while(beg < src.size())
    {
        size_t end = beg;
        while(end < src.size() && src[end] != splitter)
            ++end;
        if(end != beg || !rm_empty_result)
        {
            ++ret;
            out_iterator = typename TIt::container_type::value_type(
                src.substr(beg, end - beg));
            ++out_iterator;
        }
        beg = end + 1;
    }
    return ret;
}

template<typename TIt>
size_t split(
    std::string_view src, std::string_view splitter, TIt out_iterator,
    bool rm_empty_result)
{
    size_t beg = 0, ret = 0;
    while(beg < src.size())
    {
        size_t end = src.find(splitter, beg);

        if(end == std::string::npos)
        {
            ++ret;
            out_iterator = typename TIt::container_type::value_type(
                src.substr(beg, src.size() - beg));
            ++out_iterator;
            break;
        }

        if(end != beg || !rm_empty_result)
        {
            ++ret;
            out_iterator = typename TIt::container_type::value_type(
                src.substr(beg, end - beg));
            ++out_iterator;
        }

        beg = end + splitter.size();
    }
    return ret;
}

inline std::vector<std::string> split(
    std::string_view src, char splitter, bool rm_empty_result)
{
    std::vector<std::string> ret;
    split(src, splitter, std::back_inserter(ret), rm_empty_result);
    return ret;
}

inline std::vector<std::string> split(
    std::string_view src, std::string_view splitter, bool rm_empty_result)
{
    std::vector<std::string> ret;
    split(src, splitter, std::back_inserter(ret), rm_empty_result);
    return ret;
}

inline size_t replace_(
    std::string &str, std::string_view old_seg, std::string_view new_seg)
{
    if(old_seg.empty())
        return 0;
    size_t ret = 0, pos = 0;
    while((pos = str.find(old_seg, pos)) != std::string::npos)
    {
        str.replace(pos, old_seg.size(), new_seg);
        pos += new_seg.size();
        ++ret;
    }
    return ret;
}

inline std::string replace(
    std::string_view str, std::string_view old_seg, std::string_view new_seg)
{
    std::string ret(str);
    replace_(ret, old_seg, new_seg);
    return ret;
}

namespace stdstr_impl
{

    template<typename T>
    T from_string_impl(const std::string &) { return T(0); }

    template<> inline int       from_string_impl<int>      (const std::string &str) { return std::stoi(str); }
    template<> inline long      from_string_impl<long>     (const std::string &str) { return std::stol(str); }
    template<> inline long long from_string_impl<long long>(const std::string &str) { return std::stoll(str); }

    template<> inline unsigned int from_string_impl<unsigned int>(const std::string &str)
    {
        unsigned long ul = std::stoul(str);
        if(ul > std::numeric_limits<unsigned int>::max())
            throw std::out_of_range(str);
        return static_cast<unsigned int>(ul);
    }
    template<> inline unsigned long      from_string_impl<unsigned long>     (const std::string &str) { return std::stoul(str); }
    template<> inline unsigned long long from_string_impl<unsigned long long>(const std::string &str) { return std::stoull(str); }

    template<> inline float       from_string_impl<float>      (const std::string &str) { return std::stof(str); }
    template<> inline double      from_string_impl<double>     (const std::string &str) { return std::stod(str); }
    template<> inline long double from_string_impl<long double>(const std::string &str) { return std::stold(str); }

} // namespace stdstr_impl

template<typename T>
T from_string(const std::string &str)
{
    using supported_types = misc::type_list_t<
        int,       unsigned int,
        long,      unsigned long,
        long long, unsigned long long,
        float, double, long double
    >;
    static_assert(supported_types::contains<T>,
                  "unsupported dst type by agz::stdstr::from_string");
    return stdstr_impl::from_string_impl<T>(str);
}

namespace stdstr_impl
{

    template<typename Arg>
    std::string concat_impl(std::stringstream &sst, Arg &&arg)
    {
        sst << std::forward<Arg>(arg);
        return sst.str();
    }
    
    template<typename Arg, typename Next, typename...Others>
    std::string concat_impl(
        std::stringstream &sst, Arg &&arg, Next &&next, Others&&...others)
    {
        sst << std::forward<Arg>(arg);
        return concat_impl(
            sst, std::forward<Next>(next), std::forward<Others>(others)...);
    }

} // namespace stdstr_impl

template<typename...Args>
std::string cat(Args&&...args)
{
    std::stringstream sst;
    return stdstr_impl::concat_impl(sst, std::forward<Args>(args)...);
}

inline std::string align_left(std::string_view str, size_t width, char padder)
{
    if(str.length() >= width)
        return std::string(str);
    return std::string(str) + std::string(width - str.length(), padder);
}

inline std::string align_right(std::string_view str, size_t width, char padder)
{
    if(str.length() >= width)
        return std::string(str);
    return std::string(width - str.length(), padder) + std::string(str);
}

inline std::wstring u8_to_wstr(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

} // namespace agz::stdstr
