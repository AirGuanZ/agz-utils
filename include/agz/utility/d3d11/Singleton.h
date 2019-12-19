#pragma once

#include <agz/utility/misc.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

template<typename T>
class Singleton : public agz::misc::uncopyable_t
{
public:

    static T &GetInstance()
    {
        static T ret;
        return ret;
    }
};

AGZ_D3D11_END
