#pragma once

#include <exception>

namespace agz::misc
{
    
/**
 * @brief ��e�ڲ��������쳣��Ϣ�������ڵ���ȡ����
 * 
 * ���nested exception����ʩ
 */
template<typename TIt>
void extract_hierarchy_exceptions(const std::exception &e, TIt out_it)
{
    *out_it++ = typename TIt::container_type::value_type(e.what());
    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception &e2)
    {
        extract_hierarchy_exceptions(e2, out_it);
    }
    catch(...)
    {
        *out_it++ = typename TIt::container_type::value_type("an unknown exception was thrown");
    }
}

/**
 * @brief ������nested exception��try���Ʒ
 */
#define AGZ_HIERARCHY_TRY try {

/**
 * @brief �ѽӵ����쳣��nested exception��һ��֮�����׳�ȥ
 */
#define AGZ_HIERARCHY_WRAP(MSG) \
    } \
    catch(...) \
    { \
        std::throw_with_nested(std::runtime_error(MSG)); \
    }

inline std::string extract_exception_ptr(const std::exception_ptr &ptr)
{
    try
    {
        std::rethrow_exception(ptr);
    }
    catch(const std::exception &err)
    {
        return err.what();
    }
    catch(...)
    {
        return "unknown exception";
    }
}

} // namespace agz::misc
