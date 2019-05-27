#pragma once

#include <cstring>
#include <type_traits>

namespace agz::misc
{

/**
 * @brief һ�������Ƿ��ܺϷ��ذ�λתΪ��һ������
 */
template<typename To, typename From>
constexpr bool can_bitcast_v = sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivial_v<To>;

/**
 * @brief ��λ����ת��
 */
template<class To, class From>
std::enable_if_t<can_bitcast_v<To, From>, To> bit_cast(const From &src) noexcept
{
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

} // namespace agz::misc
