#pragma once

#include "common.h"

AGZ_D3D11_BEGIN

class RenderTarget
{
public:

    explicit RenderTarget(
        const Int2 &size,
        int         sampleCount   = 1,
        int         sampleQuality = 0);

    Int2 getSize() const noexcept;

    void addColorBuffer(
        DXGI_FORMAT format, DXGI_FORMAT rtvFormat);

    void addColorBuffer(
        DXGI_FORMAT format, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat);

    void addDepthStencil(
        DXGI_FORMAT format, DXGI_FORMAT dsvFormat);

    void addDepthStencil(
        DXGI_FORMAT format, DXGI_FORMAT dsvFormat, DXGI_FORMAT srvFormat);

    void clearColorBuffer(int index, const Color4 &color);

    void clearDepth(float depth);

    void clearStencil(uint8_t stencil);

    void clearDepthStencil(float depth, uint8_t stencil);

    ComPtr<ID3D11ShaderResourceView> getColorShaderResourceView(int index);

    ComPtr<ID3D11ShaderResourceView> getDepthShaderResourceView();

    void useDefaultViewport() const;

    void bind() const;

    void unbind() const;

private:

    void addColorBuffer(
        DXGI_FORMAT format, DXGI_FORMAT rtvFormat,
        bool enableSRV, DXGI_FORMAT srvFormat);

    void addDepthStencil(
        DXGI_FORMAT format, DXGI_FORMAT dsvFormat,
        bool enableSRV, DXGI_FORMAT srvFormat);

    struct ColorAttachment
    {
        ComPtr<ID3D11Texture2D>          tex;
        ComPtr<ID3D11RenderTargetView>   RTV;
        ComPtr<ID3D11ShaderResourceView> SRV;
    };

    UINT width_;
    UINT height_;
    DXGI_SAMPLE_DESC sampleDesc_;

    std::vector<ColorAttachment>         colors_;
    std::vector<ID3D11RenderTargetView*> rawRTVs_;

    ComPtr<ID3D11Texture2D>          dsTex_;
    ComPtr<ID3D11DepthStencilView>   DSV_;
    ComPtr<ID3D11ShaderResourceView> depthSRV_;
};

inline RenderTarget::RenderTarget(
    const Int2 &size, int sampleCount, int sampleQuality)
    : width_(static_cast<UINT>(size.x)),
      height_(static_cast<UINT>(size.y)),
      sampleDesc_{
          static_cast<UINT>(sampleCount),
          static_cast<UINT>(sampleQuality)
      }
{
    
}

inline Int2 RenderTarget::getSize() const noexcept
{
    return Int2(static_cast<int>(width_), static_cast<int>(height_));
}

inline void RenderTarget::addColorBuffer(
    DXGI_FORMAT format, DXGI_FORMAT rtvFormat)
{
    addColorBuffer(format, rtvFormat, false, DXGI_FORMAT_UNKNOWN);
}

inline void RenderTarget::addColorBuffer(
    DXGI_FORMAT format, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat)
{
    addColorBuffer(format, rtvFormat, true, srvFormat);
}

inline void RenderTarget::addDepthStencil(
    DXGI_FORMAT format, DXGI_FORMAT dsvFormat)
{
    addDepthStencil(format, dsvFormat, false, DXGI_FORMAT_UNKNOWN);
}

inline void RenderTarget::addDepthStencil(
    DXGI_FORMAT format, DXGI_FORMAT dsvFormat, DXGI_FORMAT srvFormat)
{
    addDepthStencil(format, dsvFormat, true, srvFormat);
}

inline void RenderTarget::clearColorBuffer(int index, const Color4 &color)
{
    gDeviceContext->ClearRenderTargetView(rawRTVs_[index], &color.r);
}

inline void RenderTarget::clearDepth(float depth)
{
    gDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

inline void RenderTarget::clearStencil(uint8_t stencil)
{
    gDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_STENCIL, 1, stencil);
}

inline void RenderTarget::clearDepthStencil(float depth, uint8_t stencil)
{
    gDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

inline ComPtr<ID3D11ShaderResourceView>
    RenderTarget::getColorShaderResourceView(int index)
{
    return colors_[index].SRV;
}

inline ComPtr<ID3D11ShaderResourceView>
    RenderTarget::getDepthShaderResourceView()
{
    return depthSRV_;
}

inline void RenderTarget::useDefaultViewport() const
{
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = static_cast<float>(width_);
    vp.Height   = static_cast<float>(height_);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    gDeviceContext->RSSetViewports(1, &vp);
}

inline void RenderTarget::bind() const
{
    gDeviceContext->OMSetRenderTargets(
        static_cast<UINT>(colors_.size()), rawRTVs_.data(), DSV_.Get());
}

inline void RenderTarget::unbind() const
{
    static ID3D11RenderTargetView *
        EMPTY_RTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    gDeviceContext->OMSetRenderTargets(
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, EMPTY_RTV, nullptr);
}

inline void RenderTarget::addColorBuffer(
    DXGI_FORMAT format, DXGI_FORMAT rtvFormat,
    bool enableSRV, DXGI_FORMAT srvFormat)
{
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width          = width_;
    texDesc.Height         = height_;
    texDesc.MipLevels      = 1;
    texDesc.ArraySize      = 1;
    texDesc.Format         = format;
    texDesc.SampleDesc     = sampleDesc_;
    texDesc.Usage          = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags      = 0;

    if(enableSRV)
        texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

    ComPtr<ID3D11Texture2D> tex;
    if(FAILED(gDevice->CreateTexture2D(
        &texDesc, nullptr, tex.GetAddressOf())))
        throw D3D11Exception("failed to create texture2d");

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = rtvFormat;
    if(sampleDesc_.Count == 1)
    {
        rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
    }
    else
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    ComPtr<ID3D11RenderTargetView> rtv;
    if(FAILED(gDevice->CreateRenderTargetView(
        tex.Get(), &rtvDesc, rtv.GetAddressOf())))
        throw D3D11Exception("failed to create render target view");

    if(!enableSRV)
    {
        colors_.push_back({ std::move(tex), std::move(rtv), nullptr });
        rawRTVs_.push_back(colors_.back().RTV.Get());
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = srvFormat;
    if(sampleDesc_.Count == 1)
    {
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }
    else
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

    ComPtr<ID3D11ShaderResourceView> srv;
    if(FAILED(gDevice->CreateShaderResourceView(
        tex.Get(), &srvDesc, srv.GetAddressOf())))
        throw D3D11Exception("failed to create shader resource view");

    colors_.push_back({ std::move(tex), std::move(rtv), std::move(srv) });
    rawRTVs_.push_back(colors_.back().RTV.Get());
}

inline void RenderTarget::addDepthStencil(
    DXGI_FORMAT format, DXGI_FORMAT dsvFormat,
    bool enableSRV, DXGI_FORMAT srvFormat)
{
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width          = width_;
    texDesc.Height         = height_;
    texDesc.MipLevels      = 1;
    texDesc.ArraySize      = 1;
    texDesc.Format         = format;
    texDesc.SampleDesc     = sampleDesc_;
    texDesc.Usage          = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags      = 0;

    if(enableSRV)
        texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

    ComPtr<ID3D11Texture2D> tex;
    if(FAILED(gDevice->CreateTexture2D(
        &texDesc, nullptr, tex.GetAddressOf())))
        throw D3D11Exception("failed to create texture2d");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = dsvFormat;
    dsvDesc.Flags = 0;

    if(sampleDesc_.Count == 1)
    {
        dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
    }
    else
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    ComPtr<ID3D11DepthStencilView> dsv;
    if(FAILED(gDevice->CreateDepthStencilView(
        tex.Get(), &dsvDesc, dsv.GetAddressOf())))
        throw D3D11Exception("failed to create depth stencil view");

    if(!enableSRV)
    {
        dsTex_ = std::move(tex);
        DSV_   = std::move(dsv);
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = srvFormat;
    if(sampleDesc_.Count == 1)
    {
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }
    else
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

    ComPtr<ID3D11ShaderResourceView> srv;
    if(FAILED(gDevice->CreateShaderResourceView(
        tex.Get(), &srvDesc, srv.GetAddressOf())))
        throw D3D11Exception("failed to create shader resource view");

    dsTex_    = std::move(tex);
    DSV_      = std::move(dsv);
    depthSRV_ = std::move(srv);
}

AGZ_D3D11_END
