#pragma once

#include <codecvt>
#include <locale>
#include <string>
#include <string_view>
#include <vector>

namespace agz::stdstr
{

/** @brief 单个字符是否是小写英文字母 */
inline char is_lower(char c) noexcept;
/** @brief 单个字符是否是大写英文字母 */
inline char is_upper(char c) noexcept;

/** @brief 若c是大写英文字母，将其变为小写形式，否则保持不变 */
inline char to_lower(char c) noexcept;
/** @brief 若c是小写英文字母，将其变为大写形式，否则保持不变 */
inline char to_upper(char c) noexcept;

/** @brief 原地将字符串中的大写英文字母都转变为小写 */
inline void to_lower_(std::string &s) noexcept;
/** @brief 原地将字符串中的小写英文字母都转变为大写 */
inline void to_upper_(std::string &s) noexcept;

/** @brief 将字符串中的大写英文字母都转变为小写，返回得到的新字符串 */
inline std::string to_lower(std::string_view str) noexcept;
/** @brief 将字符串中的小写英文字母都转变为大写，返回得到的新字符串 */
inline std::string to_upper(std::string_view str) noexcept;

/** @brief 字符串是否具有给定的前缀 */
inline bool starts_with(std::string_view str, std::string_view prefix) noexcept;
/** @brief 字符串是否具有给定的后缀 */
inline bool ends_with(std::string_view str, std::string_view suffix) noexcept;

/** @brief 原地删除一个字符串开头的所有空白字符 */
inline void trim_left_(std::string &str);
/** @brief 原地删除一个字符串末尾的所有空白字符 */
inline void trim_right_(std::string &str);
/** @brief 原地删除一个字符串开头和末尾的所有空白字符 */
inline void trim_(std::string &str);

/** @brief 删除字符串开头的所有空白字符，返回得到的新字符串 */
inline std::string trim_left (std::string_view str);
/** @brief 删除字符串末尾的所有空白字符，返回得到的新字符串 */
inline std::string trim_right(std::string_view str);
/** @brief 删除字符串开头和末尾的所有空白字符，返回得到的新字符串 */
inline std::string trim(std::string_view str);

/**
 * @brief 用一个字符来连接一系列字符串，返回连接后的字符串
 * 
 * begin和end是表示字符串范围的迭代器
 */
template<typename TIt>
std::string join(char joiner, TIt begin, TIt end);

/**
 * @brief 用一个字符串来连接一系列字符串，返回连接后的字符串
 *
 * begin和end是表示字符串范围的迭代器
 */
template<typename TIt>
std::string join(std::string_view joiner, TIt begin, TIt end);

/**
 * @brief 以一个字符为分隔符分割一个字符串
 * 
 * @param src 被分割的字符串
 * @param splitter 分隔符
 * @param rm_empty_result 是否移除分割结果中的空串
 * 
 * @return 分割得到的所有字符串构成的vector
 */
inline std::vector<std::string> split(
    std::string_view src, char splitter, bool rm_empty_result = true);

/**
 * @brief 以一个字符串为分隔符分割一个字符串
 *
 * @param src 被分割的字符串
 * @param splitter 分隔串
 * @param rm_empty_result 是否移除分割结果中的空串
 *
 * @return 分割得到的所有字符串构成的vector
 */
inline std::vector<std::string> split(
    std::string_view src, std::string_view splitter, bool rm_empty_result = true);

/***
 * @brief 以一个字符为分隔符分割一个字符串
 *
 * @param src 被分割的字符串
 * @param splitter 分隔符
 * @param out_iterator 用于输出分割结果的迭代器
 * @param rm_empty_result 是否移除分割结果中的空串
 *
 * @return 共输出了多少个分割结果
 */
template<typename TIt>
size_t split(
    std::string_view src, char splitter, TIt out_iterator,
    bool rm_empty_result = true);

/***
 * @brief 以一个字符串为分隔符分割一个字符串
 *
 * @param src 被分割的字符串
 * @param splitter 分隔串
 * @param out_iterator 用于输出分割结果的迭代器
 * @param rm_empty_result 是否移除分割结果中的空串
 *
 * @return 共输出了多少个分割结果
 */
template<typename TIt>
size_t split(
    std::string_view src, std::string_view splitter, TIt out_iterator,
    bool rm_empty_result = true);

/***
 * @brief 原地在字符串中进行子串替换
 * 
 * 替换不会递归进行
 */
inline size_t replace_(
    std::string &str, std::string_view old_seg, std::string_view new_seg);

/***
 * @brief 返回在字符串中进行子串替换得到的新串
 * 
 * 替换不会递归进行
 */
inline std::string replace(
    std::string_view str, std::string_view old_seg, std::string_view new_seg);

/**
 * @brief 把一个字符串转为指定的数值类型
 * 
 * 支持：(unsigned) int, (unsigned) long, (unsigned) long long, float, double, long double
 */
template<typename T>
T from_string(const std::string &str);

/**
 * @brief 将一系列对象用std::stringstream连接成一个字符串
 */
template<typename...Args>
std::string cat(Args&&...args);

/**
 * @brief 用给定字符填充str的右侧，使其长度不小于width，返回填充得到的新串
 */
inline std::string align_left(
    std::string_view str, size_t width, char padder = ' ');

/**
 * @brief 用给定字符填充str的左侧，使其长度不小于width，返回填充得到的新串
 */
inline std::string align_right(
    std::string_view str, size_t width, char padder = ' ');

/**
 * @brief 将以std::string存储的utf-8字符串转为本平台上的宽字符串
 */
inline std::wstring u8_to_wstr(const std::string &str);

/**
 * @brief 将宽字符串转为utf-8编码的std::string
 */
inline std::string wstr_to_u8(const std::wstring &str);

} // namespace agz::stdstr

#include "impl/stdstr.inl"
