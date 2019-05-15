#include "Catch.hpp"

#include <agzm/agzm.h>

using namespace agzm;

bool EQ(float x, float y) noexcept
{
    return std::abs(x - y) < 1e-4f;
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
    }
}
