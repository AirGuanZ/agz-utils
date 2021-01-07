#pragma once

#include <memory>
#include <string>
#include <typeinfo>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace agz::misc
{

template<typename T>
std::string get_pretty_typename()
{
    using TR = std::remove_reference_t<T>;

    std::unique_ptr<char, void(*)(void *)> own
    (
#ifndef _MSC_VER
        abi::__cxa_demangle(typeid(TR).name(), nullptr,
            nullptr, nullptr),
#else
        nullptr,
#endif
        std::free
    );

    std::string r = own != nullptr ? own.get() : typeid(TR).name();

    if(std::is_const<TR>::value)
        r += " const";
    if(std::is_volatile<TR>::value)
        r += " volatile";
    if(std::is_lvalue_reference<T>::value)
        r += "&";
    else if(std::is_rvalue_reference<T>::value)
        r += "&&";

    return r;
}

inline std::string typeid_to_name(const std::type_info &info)
{
    std::unique_ptr<char, void(*)(void *)> own
    (
#ifndef _MSC_VER
        abi::__cxa_demangle(info.name(), nullptr,
            nullptr, nullptr),
#else
        nullptr,
#endif
        std::free
    );

    return own != nullptr ? own.get() : info.name();
}

} // namespace agz::misc
