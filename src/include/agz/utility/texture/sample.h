#pragma once

#include "../math.h"

namespace agz::texture
{

namespace sample_impl
{
    template<typename F, typename P>
    auto linear_interpolate(const P &u0v0, const P &u1v0, const P &u0v1, const P &u1v1, F u, F v)
    {
        return (u0v0 * (1 - u) + u1v0 * u) * (1 - v) + (u0v1 * (1 - u) + u1v1 * u) * v;
    }
} // namespace sample_impl

/**
 * @brief �������������ڲ���
 * 
 * @param uv �������꣬ȡֵ��Χ[0, 1]^2������ά�ȷֱ��Ӧˮƽ�ʹ�ֱ
 * @param tex �ɵ��ö�������ȡ�����ء����ò���Ϊ����int����һ��Ϊx�±꣬�ڶ���Ϊy�±�
 * @param width ������
 * @param height ����߶�
 */
template<typename F, typename Tex>
auto nearest_sample(const math::tvec2<F> &uv, const Tex &tex, int width, int height)
    -> decltype(tex(std::declval<int>(), std::declval<int>()))
{
    assert(width > 0 && height > 0);
    int x = uv.x * width;
    int y = uv.y * height;
    x = math::clamp(x, 0, width - 1);
    y = math::clamp(y, 0, height - 1);
    return tex(x, y);
}

/**
 * @brief �������������ڲ���
 *
 * @param uv �������꣬ȡֵ��Χ[0, 1]^2������ά�ȷֱ��Ӧˮƽ�ʹ�ֱ
 * @param tex �ɵ��ö�������ȡ�����ء����ò���Ϊ����int����һ��Ϊx�±꣬�ڶ���Ϊy�±�
 * @param width ������
 * @param height ����߶�
 */
template<typename F, typename Tex>
auto linear_sample(const math::tvec2<F> &uv, const Tex &tex, int width, int height)
    -> decltype(tex(std::declval<int>(), std::declval<int>()))
{
    auto fu = static_cast<F>(uv.x * width);
    auto fv = static_cast<F>(uv.y * height);

    int pu = math::clamp(static_cast<int>(fu), 0, width - 1);
    int pv = math::clamp(static_cast<int>(fv), 0, height - 1);

    int dpu = (fu > pu + F(0.5)) ? 1 : -1;
    int dpv = (fv > pv + F(0.5)) ? 1 : -1;

    int apu = math::clamp(pu + dpu, 0, width - 1);
    int apv = math::clamp(pv + dpv, 0, height - 1);

    F du = (std::min)(std::abs(fu - pu - F(0.5)), F(1));
    F dv = (std::min)(std::abs(fv - pv - F(0.5)), F(1));

    auto pupv   = tex(pu,  pv);
    auto apupv  = tex(apu, pv);
    auto puapv  = tex(pu,  apv);
    auto apuapv = tex(apu, apv);

    return sample_impl::linear_interpolate(pupv, apupv, puapv, apuapv, du, dv);
}

} // namespace agz::texture
