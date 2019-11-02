#pragma once

#include "./texture2d.h"

namespace agz::texture
{

/**
 * @brief 简单的mipmap链生成器
 */
template<typename T>
class mipmap_chain_t
{
    std::vector<texture2d_t<T>> chain_;

public:

    mipmap_chain_t();

    /**
     * @brief 生成完整的mipmap chain，输入图像必须是边长为2^n的正方形纹理
     */
    explicit mipmap_chain_t(const texture2d_t<T> &most_detailed);

    /**
     * @brief 生成完整的mipmap chain，输入图像必须是边长为2^n的正方形纹理
     */
    void generate(const texture2d_t<T> &most_detailed);

    /**
     * @brief 是否包含一条mipmap chain
     */
    bool available() const noexcept;

    /**
     * @brief 销毁mipmap chain
     */
    void destroy();

    /**
     * @brief 包含most detailed texture在内的纹理数量
     */
    int chain_length() const noexcept;

    /**
     * @brief 取得chain中指定元素，0为most detailed element
     */
    texture2d_t<T> &chain_elem(int index) noexcept;

    /**
     * @brief 取得chain中指定元素，0为most detailed element
     */
    const texture2d_t<T> &chain_elem(int index) const noexcept;
};

} // namespace agz::texture

#include "impl/mipmap.inl"
