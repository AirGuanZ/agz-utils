#pragma once

#include <stdexcept>

namespace agz::texture
{

template<typename T>
mipmap_chain_t<T>::mipmap_chain_t()
{
    
}

template<typename T>
mipmap_chain_t<T>::mipmap_chain_t(const texture2d_t<T> &most_detailed)
{
    this->generate(most_detailed);
}

template<typename T>
void mipmap_chain_t<T>::generate(const texture2d_t<T> &most_detailed)
{
    // IMPROVE: use better filter

    this->destroy();
    chain_.push_back(most_detailed);

    int size = most_detailed.width();
    assert(size == most_detailed.height());

    while(size > 1)
    {
        if(size & 1)
            throw std::runtime_error(
                "invalid input texture size: must be power of 2");

        const int next_size = (std::max)(1, size / 2);
        texture2d_t<T> next_elem(next_size, next_size);
        texture2d_t<T> &last_elem = chain_.back();

        for(int y = 0; y < next_size; ++y)
        {
            const int py = 2 * y;
            for(int x = 0; x < next_size; ++x)
            {
                const int px = 2 * x;
                const T a = last_elem.at(py, px);
                const T b = last_elem.at(py, px + 1);
                const T c = last_elem.at(py + 1, px);
                const T d = last_elem.at(py + 1, px + 1);
                next_elem.at(y, x) = T(0.25) * (a + b + c + d);
            }
        }

        chain_.push_back(std::move(next_elem));
        size = next_size;
    }
}

template<typename T>
bool mipmap_chain_t<T>::available() const noexcept
{
    return !chain_.empty();
}

template<typename T>
void mipmap_chain_t<T>::destroy()
{
    chain_.clear();
}

template<typename T>
int mipmap_chain_t<T>::chain_length() const noexcept
{
    return int(chain_.size());
}

template<typename T>
texture2d_t<T> &mipmap_chain_t<T>::chain_elem(int index) noexcept
{
    assert(0 <= index && index < chain_length());
    return chain_[index];
}

template<typename T>
const texture2d_t<T>& mipmap_chain_t<T>::chain_elem(int index) const noexcept
{
    assert(0 <= index && index < chain_length());
    return chain_[index];
}

} // namespace agz::texture
