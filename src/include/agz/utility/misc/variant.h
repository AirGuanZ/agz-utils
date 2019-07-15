#pragma once

#include <variant>

namespace agz::misc
{
   
/**
 * @brief 在std::variant的基础上添加了is，as以及as_if函数
 */
template<typename...Types>
class variant_t : public std::variant<Types...>
{
public:

    using std::variant<Types...>::variant;
    using std_variant_t = std::variant<Types...>;

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

    template<typename T>
    auto as_if() noexcept
    {
        return std::get_if<T>(this);
    }

    template<typename T>
    auto as_if() const noexcept
    {
        return std::get_if<T>(this);
    }

    std_variant_t as_std_variant() &&
    {
        return static_cast<std::variant<Types...>>(*this);
    }

    std_variant_t &as_std_variant() &
    {
        return static_cast<std::variant<Types...>&>(*this);
    }

    std_variant_t as_std_variant() const &&
    {
        return static_cast<std::variant<Types...>>(*this);
    }

    const std_variant_t &as_std_variant() const &
    {
        return static_cast<const std::variant<Types...>&>(*this);
    }
};

namespace variant_impl
{

    template<typename T, typename = void>
    struct has_std_variant : std::false_type { };

    template<typename T>
    struct has_std_variant<T, std::void_t<decltype(std::declval<T>().as_std_variant())>>
        : std::true_type { };

    template<typename T>
    decltype(auto) to_std_variant(T &&var)
    {
        using Tt = std::remove_reference_t<T>;
        if constexpr(has_std_variant<Tt>::value)
            return std::forward<T>(var).as_std_variant();
        else
            return std::forward<T>(var);
    }

} // namespace variant_impl

/**
 * @brief 用一组可调用对象构造一个variant visitor，并用以匹配一个std::variant对象
 * @param e 被匹配的variant对象
 * @param vs 用于处理各variant分支的functor class实例。
 *			  它们的operator()返回值类型必须全部相同，且参数能覆盖所有的variant情形。
 *			  可以用[](auto){ ... }来处理默认分支，类似许多语言的模式匹配中的“_”。
 */
template<typename E, typename...Vs>
auto match_variant(E &&e, Vs...vs)
{
    struct overloaded : Vs...
    {
        explicit overloaded(Vs...vss) : Vs(vss)... { }
        using Vs::operator()...;
    };
    return std::visit(overloaded(vs...), variant_impl::to_std_variant(std::forward<E>(e)));
}

} // namespace agz::misc
