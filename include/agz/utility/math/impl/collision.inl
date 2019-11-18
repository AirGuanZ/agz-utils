#pragma once

namespace agz::math
{

namespace collision_impl
{

    template<typename F>
    F square_dist_between_pnt_aabb(const tvec3<F> &p, const tvec3<F> &low, const tvec3<F> &high)
    {
        F ret = 0;
        for(int i = 0; i < 3; ++i)
        {
            F v = p[i];
            if(v < low[i])
                ret += (low[i] - v) * (low[i] - v);
            if(v > high[i])
                ret += (v - high[i]) * (v - high[i]);
        }
        return ret;
    }

} // namespace collision_impl

template<typename F>
bool test_sphere_aabb_collision(const tvec3<F> &o, F r, const tvec3<F> &low, const tvec3<F> &high)
{
    return collision_impl::square_dist_between_pnt_aabb(o, low, high) <= r * r;
}

} // namespace agz::math

