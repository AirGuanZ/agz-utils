#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace agz::stdstr
{

inline char is_lower(char c) noexcept;
inline char is_upper(char c) noexcept;

inline char to_lower(char c) noexcept;
inline char to_upper(char c) noexcept;

inline void to_lower_(std::string &s) noexcept;
inline void to_upper_(std::string &s) noexcept;

inline std::string to_lower(std::string_view str) noexcept;
inline std::string to_upper(std::string_view str) noexcept;
   
inline bool starts_with(std::string_view str, std::string_view prefix) noexcept;
inline bool ends_with  (std::string_view str, std::string_view suffix) noexcept;

inline void trim_left_ (std::string &str);
inline void trim_right_(std::string &str);
inline void trim_      (std::string &str);

inline std::string trim_left (std::string_view str);
inline std::string trim_right(std::string_view str);
inline std::string trim      (std::string_view str);

template<typename TIt>
std::string join(char joiner, TIt begin, TIt end);

template<typename TIt>
std::string join(std::string_view joiner, TIt begin, TIt end);

inline std::vector<std::string> split(std::string_view src, char             splitter, bool rm_empty_result = true);
inline std::vector<std::string> split(std::string_view src, std::string_view splitter, bool rm_empty_result = true);

template<typename TIt>
size_t split(std::string_view src, char             splitter, TIt out_iterator, bool rm_empty_result = true);
template<typename TIt>
size_t split(std::string_view src, std::string_view splitter, TIt out_iterator, bool rm_empty_result = true);

inline size_t replace_(std::string &str, std::string_view old_seg, std::string_view new_seg);
inline std::string replace(std::string_view str, std::string_view old_seg, std::string_view new_seg);

template<typename T>
T from_string(const std::string &str);

} // namespace agz::stdstr

#include "impl/stdstr.inl"
