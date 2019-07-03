#include "Catch.hpp"

#include <agz/utility/math.h>

using namespace agz::math;

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

TEST_CASE("tensor")
{
    auto t2i0 = tensor_t<int, 2>::from_linear_indexed_fn(
        { 2, 3 }, [](int i) { return i * i; });
    REQUIRE(t2i0(0, 0) == 0);
    REQUIRE(t2i0(0, 1) == 1);
    REQUIRE(t2i0(1, 0) == 9);
    REQUIRE(t2i0(1, 2) == 25);

    auto t2i1 = tensor_t<int, 2>::from_indexed_fn(
        { 2, 3 }, [](tensor_t<int, 2>::index_t i)
    {
        int linear_index = i[0] * 3 + i[1];
        return linear_index * linear_index;
    });
    REQUIRE(t2i0 == t2i1);

    t2i0 = t2i0 + t2i1;
    REQUIRE(t2i0(0, 0) == 0);
    REQUIRE(t2i0(0, 1) == 2);
    REQUIRE(t2i0(1, 0) == 18);
    REQUIRE(t2i0(1, 2) == 50);

    float data[] = { 0, 1, 2, 3, 4, 5 };
    auto t2f = tensor_t<float, 3>::from_array({ 3, 1, 2 }, data);
    REQUIRE(t2f(2, 0, 0) == 4);

    t2f.destroy();
    REQUIRE(!t2f.is_available());
}
