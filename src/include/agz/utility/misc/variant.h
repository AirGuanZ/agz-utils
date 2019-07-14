#pragma once

#include <variant>

namespace agz::misc
{
   
/**
 * @brief ��std::variant�Ļ����������is��as����
 */
template<typename...Types>
class variant_t : public std::variant<Types...>
{
public:

    using std::variant<Types...>::variant;

    template<typename T>
    bool is() const noexcept
    {
        return std::holds_alternative<T>(*this);
    }

    template<typename T>
    auto &as() const noexcept
    {
        return std::get<T>(*this);
    }

    template<typename T>
    auto &as() noexcept
    {
        return std::get<T>(*this);
    }

    template<typename T>
    variant_t<Types...> &operator=(T &&rhs)
        noexcept(noexcept(*static_cast<std::variant<Types...>*>(this) = std::forward<T>(rhs)))
    {
        *static_cast<std::variant<Types...>*>(this) = std::forward<T>(rhs);
        return *this;
    }
};

} // namespace agz::misc
