#pragma once

#include "uncopyable.h"

namespace agz::misc
{

/**
 * @brief 非常简易的饿汉模式单件类，线程安全，但不提供主动销毁功能
 */
template<typename T>
class singleton_t : public uncopyable_t
{
public:

    static T &instance()
    {
        static T ret;
        return ret;
    }
};

} // namespace agz::misc
