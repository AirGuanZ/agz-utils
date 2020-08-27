#ifdef AGZ_ENABLE_D3D11

#include <agz/utility/graphics_api/d3d11/device.h>
#include <agz/utility/graphics_api/d3d11/deviceContext.h>
#include <agz/utility/graphics_api/d3d11/texture2d.h>
#include <agz/utility/image.h>
#include <agz/utility/misc.h>
#include <agz/utility/string.h>

AGZ_D3D11_BEGIN

namespace
{

    int getChannels(DXGI_FORMAT format)
    {
        switch(format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return 4;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return 3;
        case DXGI_FORMAT_R32_FLOAT:
            return 1;
        default:
            throw D3D11Exception("Texture2DLoader: unsupported texture2d format");
        }
    }

    bool isComponentFloat(DXGI_FORMAT format)
    {
        switch(format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return false;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return true;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return true;
        case DXGI_FORMAT_R32_FLOAT:
            return true;
        default:
            throw D3D11Exception("Texture2DLoader: unsupported texture2d format");
        }
    }

    template<typename T>
    ComPtr<ID3D11ShaderResourceView> loadExactly(
        DXGI_FORMAT format,
        int width, int height, int mipLevels,
        int channels, const T *data)
    {
        D3D11_TEXTURE2D_DESC desc;
        desc.Width          = static_cast<UINT>(width);
        desc.Height         = static_cast<UINT>(height);
        desc.MipLevels      = mipLevels;
        desc.ArraySize      = 1;
        desc.Format         = format;
        desc.SampleDesc     = { 1, 0 };
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags      = 0;

        if(mipLevels == 0 || mipLevels > 1)
        {
            desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }

        D3D11_SUBRESOURCE_DATA subrscData;
        subrscData.pSysMem          = data;
        subrscData.SysMemPitch      = sizeof(T) * channels * width;
        subrscData.SysMemSlicePitch = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format                    = format;
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = mipLevels == 0 ? -1 : mipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        auto tex = device.createTex2D(desc, &subrscData);
        auto srv = device.createSRV(tex, srvDesc);
        
        if(mipLevels == 0 || mipLevels > 1)
            deviceContext.d3dDeviceContext->GenerateMips(srv.Get());

        return srv;
    }

    template<typename A, typename B>
    B loadTexelComponent(A component) noexcept
    {
        if constexpr(std::is_same_v<A, B>)
            return component;
        else if constexpr(std::is_same_v<A, float>)
        {
            static_assert(std::is_same_v<B, uint8_t>);
            return static_cast<uint8_t>(math::saturate(component) * 255);
        }
        else
        {
            static_assert(std::is_same_v<A, uint8_t> && std::is_same_v<B, float>);
            return component / 255.0f;
        }
    }

    template<typename Data, typename Desired>
    ComPtr<ID3D11ShaderResourceView> load(
        DXGI_FORMAT format,
        int width, int height, int mipLevels,
        int dataChannels, int desireChannels, const Data *data)
    {
        if(std::is_same_v<Data, Desired> && dataChannels == desireChannels)
        {
            return loadExactly(
                format, width, height, mipLevels, dataChannels, data);
        }

        std::vector<Desired> desiredData(width * height * desireChannels, 1);

        const int texelCount = width * height;
        const int minChannels = (std::min)(dataChannels, desireChannels);

        for(int texelIdx = 0, i = 0, j = 0; texelIdx < texelCount;
            ++texelIdx, i += dataChannels, j += desireChannels)
        {
            for(int k = 0; k < minChannels; ++k)
            {
                desiredData[j + k] =
                    loadTexelComponent<Data, Desired>(data[i + k]);
            }
        }

        return loadExactly(
            format, width, height, mipLevels,
            desireChannels, desiredData.data());
    }

} // namespace anonymous

ComPtr<ID3D11ShaderResourceView> Texture2DLoader::loadFromMemory(
    DXGI_FORMAT format,
    int width, int height, int mipLevels,
    int dataChannels, const float *data)
{
    const int desireChannels = getChannels(format);
    const bool desireFloat = isComponentFloat(format);

    if(desireFloat)
    {
        return load<float, float>(
            format, width, height, mipLevels,
            dataChannels, desireChannels, data);
    }

    return load<float, uint8_t>(
        format, width, height, mipLevels,
        dataChannels, desireChannels, data);
}

ComPtr<ID3D11ShaderResourceView> Texture2DLoader::loadFromMemory(
    DXGI_FORMAT format,
    int width, int height, int mipLevels,
    int dataChannels, const uint8_t *data)
{
    const int desireChannels = getChannels(format);
    const bool desireFloat = isComponentFloat(format);

    if(desireFloat)
    {
        return load<uint8_t, float>(
            format, width, height, mipLevels,
            dataChannels, desireChannels, data);
    }

    return load<uint8_t, uint8_t>(
        format, width, height, mipLevels,
        dataChannels, desireChannels, data);
}

ComPtr<ID3D11ShaderResourceView> Texture2DLoader::loadFromFile(
    DXGI_FORMAT format, const std::string &filename, int mipLevels)
{
    if(stdstr::ends_with(stdstr::to_lower(filename), ".hdr"))
    {
        const auto data = img::load_rgb_from_hdr_file(filename);
        if(!data.is_available())
            throw D3D11Exception("failed to load hdr from " + filename);

        return loadFromMemory(
            format, data.shape()[1], data.shape()[0], mipLevels,
            3, &data.raw_data()->r);
    }

    const int channels = getChannels(format);

    if(channels == 1)
    {
        const auto data = img::load_gray_from_file(filename);
        if(!data.is_available())
            throw D3D11Exception("failed to load texture from " + filename);

        return loadFromMemory(
            format, data.shape()[1], data.shape()[0], mipLevels,
            1, data.raw_data());
    }

    if(channels == 3)
    {
        const auto data = img::load_rgb_from_file(filename);
        if(!data.is_available())
            throw D3D11Exception("failed to load texture from " + filename);

        return loadFromMemory(
            format, data.shape()[1], data.shape()[0], mipLevels,
            3, &data.raw_data()->r);
    }

    if(channels == 4)
    {
        const auto data = img::load_rgba_from_file(filename);
        if(!data.is_available())
            throw D3D11Exception("failed to load texture from " + filename);

        return loadFromMemory(
            format, data.shape()[1], data.shape()[0], mipLevels,
            4, &data.raw_data()->r);
    }

    misc::unreachable();
}

AGZ_D3D11_END

#endif // #ifdef AGZ_ENABLE_D3D11
