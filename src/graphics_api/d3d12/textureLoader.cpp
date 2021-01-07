#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/mipmapGenerator.h>
#include <agz-utils/graphics_api/d3d12/textureLoader.h>
#include <agz-utils/image.h>
#include <agz-utils/string.h>

AGZ_D3D12_BEGIN

namespace
{
    // 1. channels数量不一致
    // 2. component类型不一致
    // 3. 需要mipmap
    //
    // 生成时：
    // 1. 先生成mipmap链
    // 2. 补全/截取channels
    // 3. 转换component类型

    template<typename Component, int Channels>
    struct DestTexelTrait;

    template<typename Texel>
    struct TexelTrait;

    template<> struct DestTexelTrait<uint8_t, 1> { using Type = uint8_t;       };
    template<> struct DestTexelTrait<uint8_t, 3> { using Type = math::color3b; };
    template<> struct DestTexelTrait<uint8_t, 4> { using Type = math::color4b; };
    template<> struct DestTexelTrait<float,   1> { using Type = float;         };
    template<> struct DestTexelTrait<float,   3> { using Type = math::color3f; };
    template<> struct DestTexelTrait<float,   4> { using Type = math::color4f; };

    template<> struct TexelTrait<uint8_t>       { using Comp = uint8_t; static constexpr int Channels = 1; };
    template<> struct TexelTrait<math::color3b> { using Comp = uint8_t; static constexpr int Channels = 3; };
    template<> struct TexelTrait<math::color4b> { using Comp = uint8_t; static constexpr int Channels = 4; };
    template<> struct TexelTrait<float>         { using Comp = float;   static constexpr int Channels = 1; };
    template<> struct TexelTrait<math::color3f> { using Comp = float;   static constexpr int Channels = 3; };
    template<> struct TexelTrait<math::color4f> { using Comp = float;   static constexpr int Channels = 4; };

    template<typename Texel>
    auto &getComp(Texel &texel, int comp) noexcept
    {
        if constexpr(std::is_arithmetic_v<Texel>)
        {
            assert(comp == 0);
            return texel;
        }
        else
        {
            return texel[comp];
        }
    }

    template<typename Dst, typename Src>
    Dst convertComp(Src src) noexcept
    {
        if constexpr(std::is_same_v<Dst, Src>)
            return src;
        else if constexpr(std::is_same_v<Src, uint8_t>)
        {
            static_assert(std::is_same_v<Dst, float>);
            return src / 255.0f;
        }
        else
        {
            static_assert(std::is_same_v<Src, float>);
            static_assert(std::is_same_v<Dst, uint8_t>);
            return static_cast<uint8_t>(math::saturate(src) * 255);
        }
    }

    int getDesiredChannels(DXGI_FORMAT format)
    {
        switch(format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case DXGI_FORMAT_R8_UNORM:
            return 1;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return 4;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return 3;
        case DXGI_FORMAT_R32_FLOAT:
            return 1;
        default:
            throw D3D12Exception("TextureLoader: unsupported texture format");
        }
    }

    bool isComponentFloat(DXGI_FORMAT format)
    {
        switch(format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return false;
        case DXGI_FORMAT_R8_UNORM:
            return false;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return true;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return true;
        case DXGI_FORMAT_R32_FLOAT:
            return true;
        default:
            throw D3D12Exception("TextureLoader: unsupported texture format");
        }
    }

    template<typename Texel>
    UniqueResource loadDirectly(
        ResourceManager                                &rscMgr,
        ResourceUploader                               &uploader,
        DXGI_FORMAT                                     format,
        const std::vector<texture::texture2d_t<Texel>> &dataChain)
    {
        D3D12_RESOURCE_DESC desc;
        desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment        = 0;
        desc.Width            = dataChain.front().width();
        desc.Height           = dataChain.front().height();
        desc.DepthOrArraySize = 1;
        desc.MipLevels        = static_cast<UINT16>(dataChain.size());
        desc.Format           = format;
        desc.SampleDesc       = { 1, 0 };
        desc.Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags            = D3D12_RESOURCE_FLAG_NONE;

        auto result = rscMgr.create(
            D3D12_HEAP_TYPE_DEFAULT, desc, D3D12_RESOURCE_STATE_COMMON);

        std::vector<ResourceUploader::Texture2DInitData> data(dataChain.size());
        for(size_t i = 0; i < dataChain.size(); ++i)
            data[i].data = dataChain[i].raw_data();

        uploader.uploadTexture2D(result->resource.Get(), data.data());

        return result;
    }

    template<
        typename SrcComp,
        typename DstComp,
        int      SrcChannels,
        int      DstChannels,
        typename Texel>
    auto convertTexel(const Texel &texel)
    {
        using Result = typename DestTexelTrait<DstComp, DstChannels>::Type;

        constexpr int MinChannels =
            SrcChannels < DstChannels ? SrcChannels : DstChannels;

        Result result;
        for(int i = 0; i < MinChannels; ++i)
            getComp(result, i) = convertComp<DstComp>(getComp(texel, i));

        if constexpr(DstChannels > SrcChannels)
        {
            if constexpr(DstChannels == 4)
            {
                for(int i = SrcChannels; i < 3; ++i)
                    getComp(result, i) = 0;

                if constexpr(std::is_same_v<DstComp, uint8_t>)
                    getComp(result, 3) = 255;
                else
                    getComp(result, 3) = 1;
            }
            else
            {
                for(int i = SrcChannels; i < DstChannels; ++i)
                    getComp(result, i) = 0;
            }
        }

        return result;
    }

    template<
        typename SrcComp,
        typename DstComp,
        int      SrcChannels,
        int      DstChannels,
        typename Texel>
    UniqueResource loadWithConversion(
        ResourceManager                                &rscMgr,
        ResourceUploader                               &uploader,
        DXGI_FORMAT                                     format,
        const std::vector<texture::texture2d_t<Texel>> &dataChain)
    {
        if constexpr(std::is_same_v<SrcComp, DstComp> &&
                     SrcChannels == DstChannels)
        {
            return loadDirectly(rscMgr, uploader, format, dataChain);
        }
        else
        {
            using DstTexel = typename DestTexelTrait<DstComp, DstChannels>::Type;

            std::vector<texture::texture2d_t<DstTexel>> dstChain;
            for(auto &data : dataChain)
            {
                dstChain.push_back(data.map([](const Texel &texel)
                {
                    return convertTexel
                        <SrcComp, DstComp, SrcChannels, DstChannels>(texel);
                }));
            }

            return loadDirectly(rscMgr, uploader, format, dstChain);
        }
    }

    template<typename Texel>
    UniqueResource loadImpl(
        ResourceManager                   &rscMgr,
        ResourceUploader                  &uploader,
        DXGI_FORMAT                        format,
        int                                mipLevels,
        const texture::texture2d_t<Texel> &LOD0,
        bool                               delaySubmit)
    {
        AGZ_SCOPE_GUARD({
            if(!delaySubmit)
                uploader.submitAndSync();
        });

        // generate LODs

        if(mipLevels <= 0)
        {
            mipLevels = MipmapGenerator::computeMipmapChainLength(
                LOD0.width(), LOD0.height());
        }

        const auto mipmapChain =
            MipmapGenerator::constructMipmapChain(LOD0, mipLevels);

        // collect src & dst meta data

        using SrcComp             = typename TexelTrait<Texel>::Comp;
        constexpr int SrcChannels = TexelTrait<Texel>::Channels;

        const bool isDstCompFloat = isComponentFloat(format);
        const int  dstChannels    = getDesiredChannels(format);

        // create resource

        assert(dstChannels == 1 || dstChannels == 3 || dstChannels == 4);
        if(isDstCompFloat)
        {
            using DstComp = float;

            switch(dstChannels)
            {
            case 1:
                return loadWithConversion<SrcComp, DstComp, SrcChannels, 1>(
                    rscMgr, uploader, format, mipmapChain);
            case 2:
                return loadWithConversion<SrcComp, DstComp, SrcChannels, 3>(
                    rscMgr, uploader, format, mipmapChain);
            default:
                return loadWithConversion<SrcComp, DstComp, SrcChannels, 4>(
                    rscMgr, uploader, format, mipmapChain);
            }
        }

        using DstComp = uint8_t;

        switch(dstChannels)
        {
        case 1:
            return loadWithConversion<SrcComp, DstComp, SrcChannels, 1>(
                rscMgr, uploader, format, mipmapChain);
        case 2:
            return loadWithConversion<SrcComp, DstComp, SrcChannels, 3>(
                rscMgr, uploader, format, mipmapChain);
        default:
            return loadWithConversion<SrcComp, DstComp, SrcChannels, 4>(
                rscMgr, uploader, format, mipmapChain);
        }
    }

} // namespace anonymous

TextureLoader::TextureLoader(
    ResourceManager  &rscMgr,
    ResourceUploader &uploader)
    : manager_(rscMgr), uploader_(uploader)
{
    
}

UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT           format,
    int                   mipLevels,
    const Tex2D<uint8_t> &data,
    bool                  delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}

UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT                 format,
    int                         mipLevels,
    const Tex2D<math::color3b> &data,
    bool                        delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}

UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT                 format,
    int                         mipLevels,
    const Tex2D<math::color4b> &data,
    bool                        delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}

UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT         format,
    int                 mipLevels,
    const Tex2D<float> &data,
    bool                delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}

UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT                 format,
    int                         mipLevels,
    const Tex2D<math::color3f> &data,
    bool                        delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}


UniqueResource TextureLoader::loadFromMemory(
    DXGI_FORMAT                 format,
    int                         mipLevels,
    const Tex2D<math::color4f> &data,
    bool                        delaySubmit)
{
    return loadImpl(manager_, uploader_, format, mipLevels, data, delaySubmit);
}

UniqueResource TextureLoader::loadFromFile(
    DXGI_FORMAT        format,
    int                mipLevels,
    const std::string &filename,
    bool               delaySubmit)
{
    if(stdstr::ends_with(stdstr::to_lower(filename), ".hdr"))
    {
        const auto data = Tex2D(img::load_rgb_from_hdr_file(filename));
        return loadFromMemory(format, mipLevels, data, delaySubmit);
    }

    int width, height, channels;
    const auto bytes = img::load_bytes_from_file(
        filename, &width, &height, &channels);

    if(bytes.empty())
    {
        throw D3D12Exception(
            "TextureLoader: failed to load image from " + filename);
    }

    switch(channels)
    {
    case 1:
        {
            Tex2D<uint8_t> data(height, width, bytes.data());
            return loadFromMemory(format, mipLevels, data, delaySubmit);
        }
    case 3:
        {
            assert(width * height * sizeof(math::color3b) == bytes.size());

            Tex2D<math::color3b> data(height, width);
            std::memcpy(
                data.raw_data(), bytes.data(),
                data.get_data().elem_count() * sizeof(math::color3b));
            return loadFromMemory(format, mipLevels, data, delaySubmit);
        }
    case 4:
        {
            assert(width * height * sizeof(math::color4b) == bytes.size());

            Tex2D<math::color4b> data(height, width);
            std::memcpy(
                data.raw_data(), bytes.data(),
                data.get_data().elem_count() * sizeof(math::color4b));
            return loadFromMemory(format, mipLevels, data, delaySubmit);
        }
    default:
        throw D3D12Exception(
            "TextureLoader: unsupported image channel count: " +
            std::to_string(channels));
    }
}

void TextureLoader::submit()
{
    uploader_.submit();
}

void TextureLoader::sync()
{
    uploader_.sync();
}

void TextureLoader::submitAndSync()
{
    uploader_.submitAndSync();
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
