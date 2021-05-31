#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>

AGZ_MATH_BEGIN

namespace distribution
{
    
template<typename T, typename F, typename>
T uniform_integer(T beg, T end, F u)
{
    assert(beg < end);
    assert(0 <= u && u <= 1);
    const T ret = beg + static_cast<T>((end - beg) * u);
    return (std::min)(ret, end - 1);
}

template<typename F, typename T>
bsearch_sampler_t<F, T>::bsearch_sampler_t(const F *prob, T n)
{
    this->initialize(prob, n);
}

template<typename F, typename T>
void bsearch_sampler_t<F, T>::initialize(const F *prob, T n)
{
    assert(n > 0);

    partial_sum_.clear();
    std::partial_sum(prob, prob + n, std::back_inserter(partial_sum_));

    const F ratio = 1 / partial_sum_.back();
    for(auto &p : partial_sum_)
        p *= ratio;
}

template<typename F, typename T>
bool bsearch_sampler_t<F, T>::available() const noexcept
{
    return !partial_sum_.empty();
}

template<typename F, typename T>
void bsearch_sampler_t<F, T>::destroy()
{
    partial_sum_.clear();
}

template<typename F, typename T>
T bsearch_sampler_t<F, T>::sample(F u) const noexcept
{
    assert(available());
    assert(0 <= u && u <= 1);

    const auto upper = std::upper_bound(
        partial_sum_.begin(), partial_sum_.end(), u);
    if(upper == partial_sum_.end())
        return static_cast<T>(partial_sum_.size() - 1);
    return static_cast<T>(upper - partial_sum_.begin());
}

template<typename F, typename T>
alias_sampler_t<F, T>::alias_sampler_t(const F *prob, T n)
{
    this->initialize(prob, n);
}

template<typename F, typename T>
void alias_sampler_t<F, T>::initialize(const F *prob, T n)
{
    // https://en.wikipedia.org/wiki/Alias_method

    assert(n > 0);

    const F sum = std::accumulate(prob, prob + n, F(0));
    const F ratio = n / sum;

    std::vector<T> overs;
    std::vector<T> unders;

    table_.clear();
    table_.resize(n);

    for(T i = 0; i < n; ++i)
    {
        const F p = prob[i] * ratio;

        table_[i].accept_prob = p;
        table_[i].another_idx = i;

        if(p > 1)
            overs.push_back(i);
        else if(p < 1)
            unders.push_back(i);
    }

    while(!overs.empty() && !unders.empty())
    {
        const T over  = overs.back();
        const T under = unders.back();
        overs.pop_back();
        unders.pop_back();

        table_[over].accept_prob -= 1 - table_[under].accept_prob;
        table_[under].another_idx = over;

        if(table_[over].accept_prob > 1)
            overs.push_back(over);
        else if(table_[over].accept_prob < 1)
            unders.push_back(over);
    }

    for(auto i : overs)
        table_[i].accept_prob = 1;
    for(auto i : unders)
        table_[i].accept_prob = 1;
}

template<typename F, typename T>
void alias_sampler_t<F, T>::destroy()
{
    table_.clear();
}

template<typename F, typename T>
bool alias_sampler_t<F, T>::available() const noexcept
{
    return !table_.empty();
}

template<typename F, typename T>
T alias_sampler_t<F, T>::sample(F u) const noexcept
{
    assert(available());
    assert(0 <= u && u <= 1);

    const T n = static_cast<T>(table_.size());
    const F nu = n * u;

    const T i = (std::min)(static_cast<T>(nu), n - 1);
    const F s = nu - i;

    if(s <= table_[i].accept_prob)
        return i;
    return table_[i].another_idx;
}

template<typename F, typename T>
T alias_sampler_t<F, T>::sample(F u1, F u2) const noexcept
{
    assert(available());
    assert(0 <= u1 && u1 <= 1);
    assert(0 <= u2 && u2 <= 1);

    const T n = static_cast<T>(table_.size());
    const F nu = n * u1;

    const T i = (std::min)(static_cast<T>(nu), n - 1);

    if(u2 <= table_[i].accept_prob)
        return i;
    return table_[i].another_idx;
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_sphere(F u1, F u2) noexcept
{
    static_assert(std::is_floating_point_v<F>, "");

    const F z = 1 - 2 * u1;
    const F phi = 2 * PI<F> * u2;
    const F r = std::sqrt((std::max)(F(0), 1 - z * z));
    const F x = r * std::cos(phi);
    const F y = r * std::sin(phi);
    return { { x, y, z }, inv4PI<F> };
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_hemisphere(F u1, F u2) noexcept
{
    static_assert(std::is_floating_point_v<F>, "");

    const F z = u1;
    const F phi = 2 * PI<F> * u2;
    const F r = std::sqrt((std::max)(F(0), 1 - z * z));
    const F x = r * std::cos(phi);
    const F y = r * std::sin(phi);

    return { { x, y, z }, inv2PI<F> };
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_cone(
    F max_cos_theta, F u1, F u2) noexcept
{
    const F cos_theta = (1 - u1) + u1 * max_cos_theta;
    const F sin_theta = std::sqrt((std::max)(F(0), 1 - cos_theta * cos_theta));
    const F phi = 2 * PI<F> * u2;
    const F pdf = 1 / (2 * PI<F> * (1 - max_cos_theta));
    return {
        { std::cos(phi) * sin_theta, std::sin(phi) * sin_theta, cos_theta },
        pdf
    };
}

template<typename F>
F uniform_on_cone_pdf(F max_cos_theta) noexcept
{
    return 1 / (2 * PI<F> * (1 - max_cos_theta));
}

template<typename F>
std::pair<tvec3<F>, F> zweighted_on_hemisphere(F u1, F u2) noexcept
{
    tvec2<F> sam;
    u1 = 2 * u1 - 1;
    u2 = 2 * u2 - 1;
    if(u1 || u2)
    {
        F theta, r;
        if(std::abs(u1) > std::abs(u2))
        {
            r = u1;
            theta = F(0.25) * PI<F> * (u2 / u1);
        }
        else
        {
            r = u2;
            theta = F(0.5) * PI<F> - F(0.25) * PI<F> * (u1 / u2);
        }
        sam = r * tvec2<F>(std::cos(theta), std::sin(theta));
    }

    const F z = std::sqrt((std::max)(F(0), 1 - sam.length_square()));
    return { { sam.x, sam.y, z }, z * invPI<F> };
}

template<typename F>
F zweighted_on_hemisphere_pdf(F z) noexcept
{
    return z > 0 ? z * invPI<F> : 0;
}

template<typename F>
tvec2<F> uniform_on_triangle(F u1, F u2) noexcept
{
    const F t = std::sqrt(u1);
    return { 1 - t, t * u2 };
}

template<typename F>
tvec2<F> uniform_on_unit_disk(F u1, F u2) noexcept
{
    const F phi = 2 * PI<F> * u1;
    const F r   = std::sqrt(u2);
    return { r * std::cos(phi), r * std::sin(phi) };
}

template<typename F, typename I>
std::pair<I, F> extract_uniform_int(F u, I begin, I end)
{
    assert(begin < end);

    const I delta = end - begin;
    const I integer = begin + (std::min<I>)(I(u * delta), delta - 1);
    const F real = (std::min<F>)(begin + u * delta - integer, 1);
    return { integer, real };
}

template<typename F>
F sample_inv_cdf_table(
    F u, const F *inv_cdf, size_t tab_size) noexcept
{
    assert(tab_size >= 2);
    const F global = u * (tab_size - 1);
    const size_t low = (std::min<size_t>)(
        static_cast<size_t>(global), tab_size - 2);
    const F local = global - low;
    return inv_cdf[low] * (1 - local) + inv_cdf[low + 1] * local;
}

} // namespace distribution

AGZ_MATH_END
