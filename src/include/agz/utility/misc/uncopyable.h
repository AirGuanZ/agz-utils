#pragma once

namespace agz::misc
{
    
class uncopyable_t
{
public:

    uncopyable_t()                               = default;
    uncopyable_t(const uncopyable_t&)            = delete;
    uncopyable_t &operator=(const uncopyable_t&) = delete;
};

} // namespace agz::misc
