#pragma once

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
size_t array_size(const T (&arr)[N]) noexcept
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

} // namespace agz
