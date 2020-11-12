#pragma once

#include <agz/utility/graphics_api/d3d12/common.h>
#include <agz/utility/texture.h>

AGZ_D3D12_BEGIN

class MipmapGenerator
{
public:

    static int computeMipmapChainLength(int width, int height) noexcept;

    static std::vector<texture::texture2d_t<math::color4b>>
        constructMipmapChain(
            const texture::texture2d_t<math::color4b> &LOD0, int chainLength);

    static std::vector<texture::texture2d_t<math::color3b>>
        constructMipmapChain(
            const texture::texture2d_t<math::color3b> &LOD0, int chainLength);

    static std::vector<texture::texture2d_t<uint8_t>>
        constructMipmapChain(
            const texture::texture2d_t<uint8_t> &LOD0, int chainLength);

    static std::vector<texture::texture2d_t<float>>
        constructMipmapChain(
            const texture::texture2d_t<float> &LOD0, int chainLength);

    static std::vector<texture::texture2d_t<math::color4f>>
        constructMipmapChain(
            const texture::texture2d_t<math::color4f> &LOD0, int chainLength);

    static std::vector<texture::texture2d_t<math::color3f>>
        constructMipmapChain(
            const texture::texture2d_t<math::color3f> &LOD0, int chainLength);
};

AGZ_D3D12_END
