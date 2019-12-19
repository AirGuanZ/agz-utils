#pragma once

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

class DepthStencilBuffer : public agz::misc::uncopyable_t
{
public:

    DepthStencilBuffer() = default;

    DepthStencilBuffer(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat);

    DepthStencilBuffer(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat, DXGI_FORMAT srvFormat);

    void Initialize(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat);
    
    void Initialize(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat, DXGI_FORMAT srvFormat);

    bool IsAvailable() const noexcept;

    bool IsShaderResourceViewAvailable() const noexcept;

    void Destroy();

    int GetWidth() const noexcept;

    int GetHeight() const noexcept;

    ComPtr<ID3D11DepthStencilView> GetDepthStencilView();

    ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();

private:

    int width_  = 0;
    int height_ = 0;

    ComPtr<ID3D11DepthStencilView>   dsv_;
    ComPtr<ID3D11ShaderResourceView> srv_;
};

inline DepthStencilBuffer::DepthStencilBuffer(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat)
{
    Destroy();

    assert(width > 0 && height > 0);

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = texFormat;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = 0;

    ComPtr<ID3D11Texture2D> tex;
    if(FAILED(gDevice->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create depth stencil buffer");
    }

    // TODO
}

AGZ_D3D11_END
