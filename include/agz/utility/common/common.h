#pragma once

#include <type_traits>

namespace agz
{
    
/** @brief 作为构造函数类型参数，表示不对成员进行初始化的标志类型 */
struct uninitialized_t { };
/** @brief 不初始化标志类型的值 */
inline uninitialized_t UNINIT;

/** @brief remove reference/const/volatile */
template<typename T>
using rm_rcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

/** @brief static array size */
template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) noexcept
{
    return N;
}

/** @brief 根据成员变量指针取得它在类内的字节偏移 */
template<typename C, typename M>
ptrdiff_t byte_offset(M(C::*memPtr)) noexcept
{
    // 理论上这是UB，但我不知道有什么更好的方法
    return reinterpret_cast<char*>(&(reinterpret_cast<C*>(0)->*memPtr))
         - reinterpret_cast<char*>(reinterpret_cast<C*>(0));
}

/** @brief identity_t<T> is T */
template<typename T>
using identity_t = T;

/** @brief 取得一个临时变量指针，注意返回值的失效时间 */
template<typename T>
const T *get_temp_ptr(const T &val) noexcept
{
    return &val;
}

/**
 * 用于消除一些代码检查工具的警告
 *
 * [[maybe_unused]]在某些情境下对一些工具不起作用
 */
#define AGZ_UNACCESSED(X) ((void)(X))

} // namespace agz
