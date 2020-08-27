#pragma once

#include <agz/utility/texture.h>

#include "common.h"

AGZ_D3D11_BEGIN

class Texture2DLoader
{
public:

    static ComPtr<ID3D11ShaderResourceView> loadFromMemory(
        DXGI_FORMAT format,
        int width, int height, int mipLevels,
        int dataChannels, const float *data);

    static ComPtr<ID3D11ShaderResourceView> loadFromMemory(
        DXGI_FORMAT format,
        int width, int height, int mipLevels,
        int dataChannels, const uint8_t *data);

    static ComPtr<ID3D11ShaderResourceView> loadFromFile(
        DXGI_FORMAT format,
        const std::string &filename, int mipLevels);
};

AGZ_D3D11_END
