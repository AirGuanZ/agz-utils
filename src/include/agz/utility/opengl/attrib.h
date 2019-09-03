#pragma once

#include "./common.h"

namespace agz::gl
{
    
/**
 * @brief shader输入属性
 */
template<typename Var>
class attrib_variable_t
{
    GLuint loc_;

public:

    explicit attrib_variable_t(GLuint loc = 0) noexcept
        : loc_(loc)
    {
        
    }

    GLuint location() const noexcept
    {
        return loc_;
    }
};

} // namespace agz::gl
