#pragma once

AGZ_MATH_BEGIN

namespace collision_impl
{

    template<typename F>
    F square_dist_between_pnt_aabb(
        const tvec3<F> &p, const tvec3<F> &low, const tvec3<F> &high)
    {
        F ret = 0;
        for(int i = 0; i < 3; ++i)
        {
            const F v = p[i];
            if(v < low[i])
                ret += (low[i] - v) * (low[i] - v);
            if(v > high[i])
                ret += (v - high[i]) * (v - high[i]);
        }
        return ret;
    }

} // namespace collision_impl

template<typename F>
std::pair<tvec3<F>, F> compute_bounding_sphere(
    const tvec3<F> *vertices, size_t vertex_count)
{
    // use native algo

    static_assert(std::is_floating_point_v<F>, "");

    tvec3<F> center;
    const F ratio = F(1) / vertex_count;
    for(size_t i = 0; i < vertex_count; ++i)
        center += ratio * vertices[i];

    F radius = 0;
    for(size_t i = 0; i < vertex_count; ++i)
        radius = (std::max)(radius, vertices[i]);

    return { center, F(1.01) * radius };
}

template<typename F>
bool test_sphere_aabb_collision(
    const tvec3<F> &o, F r, const tvec3<F> &low, const tvec3<F> &high)
{
    return collision_impl::square_dist_between_pnt_aabb(o, low, high) <= r * r;
}

AGZ_MATH_END

