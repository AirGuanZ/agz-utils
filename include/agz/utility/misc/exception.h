#pragma once

#include <exception>
#include <string>

namespace agz::misc
{
    
/**
 * @brief 将e内部的所有异常消息由外往内地提取出来
 * 
 * 针对nested exception的设施
 */
template<typename TIt>
void extract_hierarchy_exceptions(const std::exception &e, TIt out_it)
{
    *out_it++ = e.what();
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
        *out_it++ = "an unknown exception was thrown";
    }
}

/**
 * @brief 用来抛nested exception的try替代品
 */
#define AGZ_HIERARCHY_TRY try {

/**
 * @brief 把接到的异常用nested exception包一层之后再抛出去
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
