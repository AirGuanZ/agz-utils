#pragma once

#include <exception>
#include <string>

namespace agz::misc
{
    
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

#define AGZ_HIERARCHY_TRY try {

#define AGZ_HIERARCHY_WRAP(MSG) \
    } \
    catch(...) \
    { \
        std::throw_with_nested(std::runtime_error(MSG)); \
    }

} // namespace agz::misc
