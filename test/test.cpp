#include "Catch.hpp"

#include <agzm/agzm.h>

using namespace agzm;

bool EQ(float x, float y) noexcept
{
    return std::abs(x - y) < 1e-4f;
}

bool EQ(const vec3f &x, const vec3f &y) noexcept
{
    return EQ(x[0], y[0]) && EQ(x[1], y[1]) && EQ(x[2], y[2]);
}

bool EQ(const vec4f &x, const vec4f &y) noexcept
{
    return EQ(x[0], y[0]) && EQ(x[1], y[1]) && EQ(x[2], y[2]) && EQ(x[3], y[3]);
}

bool EQ(const mat3f_c &a, const mat3f_c &b) noexcept
{
    for(int c = 0; c != 3; ++c)
    {
        for(int r = 0; r != 3; ++r)
        {
            if(!EQ(a(r, c), b(r, c)))
                return false;
        }
    }
    return true;
}

bool EQ(const mat4f_c &a, const mat4f_c &b) noexcept
{
    for(int c = 0; c != 4; ++c)
    {
        for(int r = 0; r != 4; ++r)
        {
            if(!EQ(a(r, c), b(r, c)))
                return false;
        }
    }
    return true;
}

TEST_CASE("vec")
{
    SECTION("vec2")
    {
        REQUIRE(vec2f().is_zero());
        REQUIRE(EQ(vec2f(1, 2).length(), std::sqrt(5.0f)));
    }

    SECTION("swizzle")
    {
        REQUIRE(swizzle_vec<2, 1, 0>(vec4f(0, 1, 2, 3)) == vec3f(2, 1, 0));
        REQUIRE(vec3f(0, 1, 2).yyz() == vec3f(1, 1, 2));

        REQUIRE(color4d(0, 1, 2, 3).rggb() == color4d{ 0, 1, 1, 2 });
    }
}

TEST_CASE("mat")
{
    SECTION("mat3")
    {
        REQUIRE(EQ(mat3f_c::translate(1, 2) * mat3f_c::translate(1, 2).inv(), mat3f_c::identity()));
        REQUIRE(EQ(mat3f_c::translate(1, 2) * vec3f(1, 2, 1), vec3f(2, 4, 1)));
    }

    SECTION("mat4")
    {
        REQUIRE(EQ(mat4f_c::translate(1, 2, 3) * mat4f_c::translate(1, 2, 3).inv(), mat4f_c::identity()));
        REQUIRE(EQ(mat4f_c::translate(1, 2, 3) * vec4f(1, 2, 1, 1), vec4f(2, 4, 4, 1)));
    }
}
