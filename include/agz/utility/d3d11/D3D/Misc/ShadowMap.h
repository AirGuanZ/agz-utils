#pragma once

#include <array>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

class ShadowMap : public agz::misc::uncopyable_t
{
    int width_;
    int height_;

    D3D11_VIEWPORT viewport_;

    ComPtr<ID3D11DepthStencilView> dsv_;

    ComPtr<ID3D11RenderTargetView> rtv_;
    ComPtr<ID3D11ShaderResourceView> srv_;

    std::vector<D3D11_VIEWPORT> oldViewports_;
    std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> oldRenderTargets_;
    ID3D11DepthStencilView *oldDepthStencilView_;

    void ReleaseOldCOMObjects()
    {
        for(auto &rt : oldRenderTargets_)
        {
            ReleaseCOMObjects(rt);
        }
        ReleaseCOMObjects(oldDepthStencilView_);
    }

public:

    ShadowMap(int width, int height);

    ~ShadowMap();

    ComPtr<ID3D11DepthStencilView> GetDSV() noexcept;

    ComPtr<ID3D11ShaderResourceView> GetSRV() noexcept;

    int GetWidth() const noexcept;

    int GetHeight() const noexcept;

    void Begin();

    void End();
};

inline ShadowMap::ShadowMap(int width, int height)
    : width_(width), height_(height),
      viewport_{
        0, 0,
        static_cast<float>(width),
		static_cast<float>(height),
        0, 1
      },
      oldRenderTargets_{ nullptr },
      oldDepthStencilView_(nullptr)
{
    assert(width > 0 && height > 0);

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = 0;

    ComPtr<ID3D11Texture2D> depth;
    if(FAILED(gDevice->CreateTexture2D(&texDesc, nullptr, depth.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create depth texture for shadow mapping");
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags              = 0;
    dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    if(FAILED(gDevice->CreateDepthStencilView(depth.Get(), &dsvDesc, dsv_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create depth stencil view for shadow mapping");
    }

    D3D11_TEXTURE2D_DESC rtDesc;
    rtDesc.Width              = width;
    rtDesc.Height             = height;
    rtDesc.MipLevels          = 1;
    rtDesc.ArraySize          = 1;
    rtDesc.Format             = DXGI_FORMAT_R32_FLOAT;
    rtDesc.SampleDesc.Count   = 1;
    rtDesc.SampleDesc.Quality = 0;
    rtDesc.Usage              = D3D11_USAGE_DEFAULT;
    rtDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    rtDesc.CPUAccessFlags     = 0;
    rtDesc.MiscFlags          = 0;

    ComPtr<ID3D11Texture2D> rt;
    if(FAILED(gDevice->CreateTexture2D(&rtDesc, nullptr, rt.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create render target texture for shadow mapping");
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format             = DXGI_FORMAT_R32_FLOAT;
    rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    if(FAILED(gDevice->CreateRenderTargetView(rt.Get(), &rtvDesc, rtv_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create render target view for shadow mapping");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    if(FAILED(gDevice->CreateShaderResourceView(rt.Get(), &srvDesc, srv_.GetAddressOf())))
    {
        throw VRPGBaseException("failed to create shader resource view for shadow mapping");
    }
}

inline ShadowMap::~ShadowMap()
{
    ReleaseOldCOMObjects();
}

inline ComPtr<ID3D11DepthStencilView> ShadowMap::GetDSV() noexcept
{
    return dsv_;
}

inline ComPtr<ID3D11ShaderResourceView> ShadowMap::GetSRV() noexcept
{
    return srv_;
}

inline int ShadowMap::GetWidth() const noexcept
{
    return width_;
}

inline int ShadowMap::GetHeight() const noexcept
{
    return height_;
}

inline void ShadowMap::Begin()
{
    assert(dsv_ && srv_);

    UINT oldViewportCount;
    gDeviceContext->RSGetViewports(&oldViewportCount, nullptr);
    oldViewports_.resize(oldViewportCount);
    gDeviceContext->RSGetViewports(&oldViewportCount, oldViewports_.data());
    gDeviceContext->RSSetViewports(1, &viewport_);

    gDeviceContext->OMGetRenderTargets(
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldRenderTargets_.data(),
        &oldDepthStencilView_);

    ID3D11RenderTargetView *renderTarget[1] = { rtv_.Get() };
    gDeviceContext->OMSetRenderTargets(1, renderTarget, dsv_.Get());

    static const float BACKGROUND_COLOR[4] = { 1, 1, 1, 1 };
    gDeviceContext->ClearRenderTargetView(rtv_.Get(), BACKGROUND_COLOR);
    gDeviceContext->ClearDepthStencilView(dsv_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}

inline void ShadowMap::End()
{
    assert(dsv_ && srv_);

    UINT oldViewCount = static_cast<UINT>(oldViewports_.size());
    gDeviceContext->RSSetViewports(oldViewCount, oldViewports_.data());

    gDeviceContext->OMSetRenderTargets(
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldRenderTargets_.data(), oldDepthStencilView_);

    ReleaseOldCOMObjects();
}

AGZ_D3D11_END
