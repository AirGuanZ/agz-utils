#pragma once

namespace agz
{
    
/** @brief ��Ϊ���캯�����Ͳ�������ʾ���Գ�Ա���г�ʼ���ı�־���� */
struct uninitialized_t { };
/** @brief ����ʼ����־���͵�ֵ */
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

/** @brief ���ݳ�Ա����ָ��ȡ���������ڵ��ֽ�ƫ�� */
template<typename C, typename M>
ptrdiff_t byte_offset(M(C::*memPtr)) noexcept
{
    // ����������UB�����Ҳ�֪����ʲô���õķ���
    return reinterpret_cast<char*>(&(reinterpret_cast<C*>(0)->*memPtr))
         - reinterpret_cast<char*>(reinterpret_cast<C*>(0));
}

} // namespace agz
