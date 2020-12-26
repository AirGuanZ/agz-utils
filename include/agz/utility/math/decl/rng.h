#pragma once

#include <algorithm>
#include <cstdint>

AGZ_MATH_BEGIN

class pcg_t
{
public:

    using seed_t = uint64_t;

    AGZ_MATH_API pcg_t() noexcept
        : state_(0x853c49e6748fea9bULL), inc_(0xda3e39cb94b95bdbULL)
    {
        
    }

    AGZ_MATH_API explicit pcg_t(seed_t seed) noexcept
        : state_(0x853c49e6748fea9bULL), inc_(0xda3e39cb94b95bdbULL)
    {
        set_seed(seed);
    }

    AGZ_MATH_API void set_seed(seed_t seed) noexcept
    {
        state_ = 0u;
        inc_ = (seed << 1u) | 1u;
        uniform_uint32();
        state_ += 0x853c49e6748fea9bULL;
        uniform_uint32();
    }

    AGZ_MATH_API float uniform_float()
    {
        return AGZ_MATH_MIN(0.99999994f,
                            uniform_uint32() * 2.3283064365386963e-10f);
    }

    AGZ_MATH_API double uniform_double()
    {
        return AGZ_MATH_MIN(0.99999999999999989,
                            uniform_uint32() * 2.3283064365386963e-10);
    }

private:

    AGZ_MATH_API uint32_t uniform_uint32() noexcept
    {
        const uint64_t oldstate = state_;
        state_ = oldstate * 0x5851f42d4c957f2dULL + inc_;
        const uint32_t xorshifted = static_cast<uint32_t>(
                            ((oldstate >> 18u) ^ oldstate) >> 27u);
        const uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
    }

    seed_t state_, inc_;
};

AGZ_MATH_END
