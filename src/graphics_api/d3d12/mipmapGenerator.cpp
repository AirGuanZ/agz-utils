#ifdef AGZ_ENABLE_D3D12

#include <avir.h>

#include <agz/utility/graphics_api/d3d12/mipmapGenerator.h>

AGZ_D3D12_BEGIN

namespace
{

    template<typename Texel, typename Comp, int Channels>
    texture::texture2d_t<Texel> generateLevel(
        const texture::texture2d_t<Texel> &initialLevel,
        int                                newWidth,
        int                                newHeight)
    {
        static_assert(std::is_standard_layout_v<Texel>);
        static_assert(sizeof(Texel) == sizeof(Comp) * Channels);

        texture::texture2d_t<Texel> output(newHeight, newWidth);

        avir::CImageResizer imageResizer;
        imageResizer.resizeImage(
            reinterpret_cast<const Comp *>(initialLevel.raw_data()),
            initialLevel.width(),
            initialLevel.height(),
            0,
            reinterpret_cast<Comp *>(output.raw_data()),
            newWidth,
            newHeight,
            Channels,
            0);

        return output;
    }

    template<typename Texel, typename Comp, int Channels>
    std::vector<texture::texture2d_t<Texel>> constructMipmapChainImpl(
        texture::texture2d_t<Texel> LOD0, int chainLength)
    {
        std::vector<texture::texture2d_t<Texel>> ret;
        ret.push_back(std::move(LOD0));

        for(int i = 1; i < chainLength; ++i)
        {
            const int newWidth  = static_cast<int>(ret.back().width()) / 2;
            const int newHeight = static_cast<int>(ret.back().height()) / 2;

            if(newWidth <= 0 || newHeight <= 0)
                break;

            ret.push_back(generateLevel<Texel, Comp, Channels>(
                ret.front(), newWidth, newHeight));
        }

        return ret;
    }

} // namespace anonymous

int MipmapGenerator::computeMipmapChainLength(int width, int height) noexcept
{
    int ret = 1;
    while(width > 1 || height > 1)
    {
        width  >>= 1;
        height >>= 1;
        ++ret;
    }
    return ret;
}

std::vector<texture::texture2d_t<math::color3b>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<math::color3b> &LOD0,
        int                                        chainLength)
{
    return constructMipmapChainImpl<math::color3b, uint8_t, 3>(
                LOD0, chainLength);
}

std::vector<texture::texture2d_t<math::color3f>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<math::color3f> &LOD0,
        int                                        chainLength)
{
    return constructMipmapChainImpl<math::color3f, float, 3>(
                LOD0, chainLength);
}

std::vector<texture::texture2d_t<math::color4b>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<math::color4b> &LOD0,
        int                                        chainLength)
{
    return constructMipmapChainImpl<math::color4b, uint8_t, 4>(
                LOD0, chainLength);
}

std::vector<texture::texture2d_t<math::color4f>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<math::color4f> &LOD0,
        int                                        chainLength)
{
    return constructMipmapChainImpl<math::color4f, float, 4>(
                LOD0, chainLength);
}

std::vector<texture::texture2d_t<uint8_t>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<uint8_t> &LOD0,
        int                                  chainLength)
{
    return constructMipmapChainImpl<uint8_t, uint8_t, 1>(
                LOD0, chainLength);
}

std::vector<texture::texture2d_t<float>>
    MipmapGenerator::constructMipmapChain(
        const texture::texture2d_t<float> &LOD0,
        int                                chainLength)
{
    return constructMipmapChainImpl<float, float, 1>(
        LOD0, chainLength);
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
