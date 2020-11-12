#pragma once

#include <agz/utility/graphics_api/d3d12/resourceManager.h>
#include <agz/utility/graphics_api/d3d12/resourceUploader.h>
#include <agz/utility/texture.h>

AGZ_D3D12_BEGIN

class TextureLoader
{
public:

    template<typename T>
    using Tex2D = texture::texture2d_t<T>;

    TextureLoader(
        ResourceManager  &rscMgr,
        ResourceUploader &uploader);

    UniqueResource loadFromMemory(
        DXGI_FORMAT           format,
        int                   mipLevels,
        const Tex2D<uint8_t> &data,
        bool                  delaySubmit = false);

    UniqueResource loadFromMemory(
        DXGI_FORMAT                 format,
        int                         mipLevels,
        const Tex2D<math::color3b> &data,
        bool                        delaySubmit = false);

    UniqueResource loadFromMemory(
        DXGI_FORMAT                 format,
        int                         mipLevels,
        const Tex2D<math::color4b> &data,
        bool                        delaySubmit = false);

    UniqueResource loadFromMemory(
        DXGI_FORMAT         format,
        int                 mipLevels,
        const Tex2D<float> &data,
        bool                delaySubmit = false);

    UniqueResource loadFromMemory(
        DXGI_FORMAT                 format,
        int                         mipLevels,
        const Tex2D<math::color3f> &data,
        bool                        delaySubmit = false);

    UniqueResource loadFromMemory(
        DXGI_FORMAT                 format,
        int                         mipLevels,
        const Tex2D<math::color4f> &data,
        bool                        delaySubmit = false);

    UniqueResource loadFromFile(
        DXGI_FORMAT        format,
        int                mipLevels,
        const std::string &filename,
        bool               delaySubmit = false);

    void submit();

    void sync();

    void submitAndSync();

private:

    ResourceManager  &manager_;
    ResourceUploader &uploader_;
};

AGZ_D3D12_END
