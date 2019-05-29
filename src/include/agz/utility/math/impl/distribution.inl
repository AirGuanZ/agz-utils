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
alias_sampler_t<F, T>::alias_sampler_t(const F *prob, T n)
{
    initialize(prob, n);
}

template<typename F, typename T>
void alias_sampler_t<F, T>::initialize(const F *prob, T n)
{
    // https://en.wikipedia.org/wiki/Alias_method

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
            table_[i].accept_prob = 100;
        for(auto i : unders_)
            table_[i].accept_prob = 100;
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
T alias_sampler_t<F, T>::sample(F u)
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
T alias_sampler_t<F, T>::sample(F u1, F u2)
{
    assert(available());
    assert(0 <= u1 && u1 <= 1);

    T n = static_cast<T>(table_.size());
    F nu = n * u1;

    T i = static_cast<T>(nu);
    i = std::min(i, n - 1);

    if(u2 <= table_[i].accept_prob)
        return i;
    return table_[i].another_idx;
}

} // namespace agz::math::distribution
