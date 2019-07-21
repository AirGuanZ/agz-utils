#pragma once

#include <algorithm>
#include <cassert>
#include <numeric>

namespace agz::math::distribution
{
    
template<typename T, typename F, typename>
T uniform_integer(T beg, T end, F u)
{
    assert(beg < end);
    assert(0 <= u && u <= 1);
    T ret = beg + static_cast<T>((end - beg) * u);
    return std::min(ret, end - 1);
}

template<typename F, typename T>
bsearch_sampler_t<F, T>::bsearch_sampler_t(const F *prob, T n)
{
    initialize(prob, n);
}

template<typename F, typename T>
void bsearch_sampler_t<F, T>::initialize(const F *prob, T n)
{
    assert(n > 0);

    partial_sum_.clear();
    std::partial_sum(prob, prob + n, std::back_inserter(partial_sum_));

    F ratio = 1 / partial_sum_.back();
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

    auto upper = std::upper_bound(partial_sum_.begin(), partial_sum_.end(), u);
    if(upper == partial_sum_.end())
        return static_cast<T>(partial_sum_.size() - 1);
    return static_cast<T>(upper - partial_sum_.begin());
}

template<typename F, typename T>
alias_sampler_t<F, T>::alias_sampler_t(const F *prob, T n)
{
    initialize(prob, n);
}

template<typename F, typename T>
void alias_sampler_t<F, T>::initialize(const F *prob, T n)
{
    // https://en.wikipedia.org/wiki/Alias_method

    assert(n > 0);

    F sum = std::accumulate(prob, prob + n, F(0));
    F ratio = n / sum;

    std::vector<T> overs_;
    std::vector<T> unders_;

    table_.clear();
    table_.resize(n);

    for(T i = 0; i < n; ++i)
    {
        F p = prob[i] * ratio;

        table_[i].accept_prob = p;
        table_[i].another_idx = i;

        if(p > 1)
            overs_.push_back(i);
        else if(p < 1)
            unders_.push_back(i);
    }

    while(!overs_.empty() && !unders_.empty())
    {
        T over  = overs_.back();
        T under = unders_.back();
        overs_.pop_back();
        unders_.pop_back();

        table_[over].accept_prob -= 1 - table_[under].accept_prob;
        table_[under].another_idx = over;

        if(table_[over].accept_prob > 1)
            overs_.push_back(over);
        else if(table_[over].accept_prob < 1)
            unders_.push_back(over);

        for(auto i : overs_)
            table_[i].accept_prob = 1;
        for(auto i : unders_)
            table_[i].accept_prob = 1;
    }
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

    T n = static_cast<T>(table_.size());
    F nu = n * u;

    T i = static_cast<T>(nu);
    i = std::min(i, n - 1);

    F s = nu - i;

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

    T n = static_cast<T>(table_.size());
    F nu = n * u1;

    T i = static_cast<T>(nu);
    i = std::min(i, n - 1);

    if(u2 <= table_[i].accept_prob)
        return i;
    return table_[i].another_idx;
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_sphere(F u1, F u2) noexcept
{
    static_assert(std::is_floating_point_v<F>);

    F z = 1 - 2 * u1;
    F phi = 2 * PI<F> * u2;
    F r = std::sqrt((std::max)(F(0), 1 - z * z));
    F x = r * std::cos(phi);
    F y = r * std::sin(phi);
    return { { x, y, z }, inv4PI<F> };
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_hemisphere(F u1, F u2) noexcept
{
    static_assert(std::is_floating_point_v<F>);

    F z = u1;
    F phi = 2 * PI<F> * u2;
    F r = std::sqrt((std::max)(F(0), 1 - z * z));
    F x = r * std::cos(phi);
    F y = r * std::sin(phi);

    return { { x, y, z }, inv2PI<F> };
}

template<typename F>
std::pair<tvec3<F>, F> uniform_on_cone(F max_cos_theta, F u1, F u2) noexcept
{
    F cos_theta = (1 - u1) + u1 * max_cos_theta;
    F sin_theta = std::sqrt((std::max)(F(0), 1 - cos_theta * cos_theta));
    F phi = 2 * PI<F> * u2;
    F pdf = 1 / (2 * PI<F> * (1 - max_cos_theta));
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

    F z = std::sqrt(std::max(F(0), 1 - sam.length_square()));
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
    F t = std::sqrt(u1);
    return { 1 - t, t * u2 };
}

} // namespace agz::math::distribution
