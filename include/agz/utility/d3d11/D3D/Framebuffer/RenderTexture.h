#pragma once

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

class RenderTexture : public agz::misc::uncopyable_t
{
public:

    RenderTexture() = default;

    RenderTexture(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT rtFormat, DXGI_FORMAT viewFormat);

    void Initialize(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT rtFormat, DXGI_FORMAT viewFormat);

    bool IsAvailable() const noexcept;

    void Destroy();

    int GetWidth() const noexcept;

    int GetHeight() const noexcept;

    ComPtr<ID3D11Texture2D> GetRenderTarget();

    ComPtr<ID3D11RenderTargetView> GetRenderTargetView();

    ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();

private:

    int width_ = 0, height_ = 0;

    ComPtr<ID3D11Texture2D>          tex_;
    ComPtr<ID3D11RenderTargetView>   rtv_;
    ComPtr<ID3D11ShaderResourceView> srv_;
};

inline RenderTexture::RenderTexture(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT rtFormat, DXGI_FORMAT viewFormat)
    : width_(0), height_(0)
{
    Initialize(width, height, texFormat, rtFormat, viewFormat);
}

inline void RenderTexture::Initialize(int width, int height, DXGI_FORMAT texFormat, DXGI_FORMAT rtFormat, DXGI_FORMAT viewFormat)
{
    Destroy();

    assert(width > 0 && height > 0);

    D3D11_TEXTURE2D_DESC rtDesc;
    rtDesc.Width              = static_cast<UINT>(width);
    rtDesc.Height             = static_cast<UINT>(height);
    rtDesc.MipLevels          = 1;
    rtDesc.ArraySize          = 1;
    rtDesc.Format             = texFormat;
    rtDesc.SampleDesc.Count   = 1;
    rtDesc.SampleDesc.Quality = 0;
    rtDesc.Usage              = D3D11_USAGE_DEFAULT;
    rtDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    rtDesc.CPUAccessFlags     = 0;
    rtDesc.MiscFlags          = 0;

    if(FAILED(gDevice->CreateTexture2D(&rtDesc, nullptr, tex_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create render target");
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format             = rtFormat;
    rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    if(FAILED(gDevice->CreateRenderTargetView(tex_.Get(), &rtvDesc, rtv_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create render target view");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = viewFormat;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    if(FAILED(gDevice->CreateShaderResourceView(tex_.Get(), &srvDesc, srv_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create shader resource view");
    }

    width_  = width;
    height_ = height;
}

inline bool RenderTexture::IsAvailable() const noexcept
{
    assert((rtv_ != nullptr) == (srv_ != nullptr));
    return rtv_ != nullptr;
}

inline void RenderTexture::Destroy()
{
    width_  = 0;
    height_ = 0;
    rtv_.Reset();
    srv_.Reset();
}

inline int RenderTexture::GetWidth() const noexcept
{
    return width_;
}

inline int RenderTexture::GetHeight() const noexcept
{
    return height_;
}

inline ComPtr<ID3D11Texture2D> RenderTexture::GetRenderTarget()
{
    return tex_;
}

inline ComPtr<ID3D11RenderTargetView> RenderTexture::GetRenderTargetView()
{
    return rtv_;
}

inline ComPtr<ID3D11ShaderResourceView> RenderTexture::GetShaderResourceView()
{
    return srv_;
}

AGZ_D3D11_END
