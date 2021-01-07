#pragma once

#include "./sample2d.h"

namespace agz::texture
{
    
/**
 * @brief 对纹理进行最近邻采样
 * 
 * @param uvw    纹理坐标，取值范围[0, 1]^3，三个维度分别对应水平、垂直和纵深
 * @param tex    可调用对象，用于取得纹素。调用参数为三个int，分别为x，y，z下标
 * @param width  纹理宽度
 * @param height 纹理高度
 * @param depth  纹理深度
 */
template<typename F, typename Tex>
auto nearest_sample3d(
    const math::tvec3<F> &uvw, const Tex &tex, int width, int height, int depth)
    -> decltype(tex(0, 0, 0))
{
    assert(width > 0 && height > 0 && depth > 0);
    const int x = math::clamp(int(uvw.x * width), 0, width - 1);
    const int y = math::clamp(int(uvw.y * height), 0, height - 1);
    const int z = math::clamp(int(uvw.z * depth), 0, depth - 1);
    return tex(x, y, z);
}

/**
 * @brief 对纹理进行双线性插值采样
 *
 * @param uvw    纹理坐标，取值范围[0, 1]^3，三个维度分别对应水平、垂直和纵深
 * @param tex    可调用对象，用于取得纹素。调用参数为三个int，分别为x，y，z下标
 * @param width  纹理宽度
 * @param height 纹理高度
 * @param depth  纹理深度
 */
template<typename F, typename Tex>
auto linear_sample3d(
    const math::tvec3<F> &uvw, const Tex &tex, int width, int height, int depth)
    -> decltype(tex(0, 0, 0))
{
    const auto fu = F(uvw.x * width);
    const auto fv = F(uvw.y * height);
    const auto fw = F(uvw.z * depth);

    const int pu = math::clamp(int(fu), 0, width - 1);
    const int pv = math::clamp(int(fv), 0, height - 1);
    const int pw = math::clamp(int(fw), 0, depth - 1);

    const int dpu = (fu > pu + F(0.5)) ? 1 : -1;
    const int dpv = (fv > pv + F(0.5)) ? 1 : -1;
    const int dpw = (fw > pw + F(0.5)) ? 1 : -1;

    const int apu = math::clamp(pu + dpu, 0, width - 1);
    const int apv = math::clamp(pv + dpv, 0, height - 1);
    const int apw = math::clamp(pw + dpw, 0, depth - 1);

    const F du = (std::min)(std::abs(fu - pu - F(0.5)), F(1));
    const F dv = (std::min)(std::abs(fv - pv - F(0.5)), F(1));
    const F dw = (std::min)(std::abs(fw - pw - F(0.5)), F(1));

    const auto ppp = tex( pu,  pv,  pw);
    const auto ppa = tex( pu,  pv, apw);
    const auto pap = tex( pu, apv,  pw);
    const auto paa = tex( pu, apv, apw);
    const auto app = tex(apu,  pv,  pw);
    const auto apa = tex(apu,  pv, apw);
    const auto aap = tex(apu, apv,  pw);
    const auto aaa = tex(apu, apv, apw);

    const auto interp_p = sample_impl::linear_interpolate_2d(
        ppp, app, pap, aap, du, dv);

    const auto interp_a = sample_impl::linear_interpolate_2d(
        ppa, apa, paa, aaa, du, dv);

    return math::lerp(interp_p, interp_a, dw);
}

} // namespace agz::texture
