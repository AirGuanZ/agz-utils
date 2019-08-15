#pragma once

#include <variant>

namespace agz::misc
{
   
/**
 * @brief ��std::variant�Ļ����������is��as�Լ�as_if����
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
        noexcept(noexcept(*static_cast<std_variant_t*>(this) = std::forward<T>(rhs)))
    {
        *static_cast<std_variant_t*>(this) = std::forward<T>(rhs);
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
};

namespace variant_impl
{

    template<typename T>
    decltype(auto) to_std_variant(T &&var) { return std::forward<T>(var); }

    template<typename...Ts>
    std::variant<Ts...> &to_std_variant(variant_t<Ts...> &var)
    {
        return static_cast<std::variant<Ts...>&>(var);
    }

    template<typename...Ts>
    const std::variant<Ts...> &to_std_variant(const variant_t<Ts...> &var)
    {
        return static_cast<const std::variant<Ts...>&>(var);
    }

    template<typename...Ts>
    std::variant<Ts...> to_std_variant(variant_t<Ts...> &&var)
    {
        return static_cast<std::variant<Ts...>>(var);
    }

} // namespace variant_impl

/**
 * @brief ��һ��ɵ��ö�����һ��variant visitor��������ƥ��һ��std::variant��misc::variant_t����
 * @param e ��ƥ���variant����
 * @param vs ���ڴ����variant��֧��functor classʵ����
 *			  ���ǵ�operator()����ֵ���ͱ���ȫ����ͬ���Ҳ����ܸ������е�variant���Ρ�
 *			  ������[](auto){ ... }������Ĭ�Ϸ�֧������ģʽƥ���еġ�_����
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
