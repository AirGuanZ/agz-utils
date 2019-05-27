#pragma once

namespace agz::misc
{
    
/**
 * @brief 不可复制类型
 * 
 * 凡继承此类者无默认复制构造/赋值函数
 */
class uncopyable_t
{
public:

    uncopyable_t()                               = default;
    uncopyable_t(const uncopyable_t&)            = delete;
    uncopyable_t &operator=(const uncopyable_t&) = delete;
};

} // namespace agz::misc
