#pragma once

#include "common.h"

AGZ_D3D11_BEGIN

class RenderTarget : public misc::uncopyable_t
{
public:

    RenderTarget();

    explicit RenderTarget(
        const Int2 &size,
        int         sampleCount   = 1,
        int         sampleQuality = 0);

    RenderTarget(RenderTarget &&moveFrom) noexcept;

    RenderTarget &operator=(RenderTarget &&moveFrom) noexcept;

    void swap(RenderTarget &swapWith) noexcept;

    Int2 getSize() const noexcept;

    int getWidth() const noexcept;

    int getHeight() const noexcept;

    float getWOverH() const noexcept;

    void addColorBuffer(
        DXGI_FORMAT format, DXGI_FORMAT rtvFormat);

    void addColorBuffer(
        DXGI_FORMAT format, DXGI_FORMAT rtvFormat, DXGI_FORMAT srvFormat);

    void addColorBuffer(
        ComPtr<ID3D11Texture2D>          tex,
        ComPtr<ID3D11RenderTargetView>   rtv,
        ComPtr<ID3D11ShaderResourceView> srv);

    void addDepthStencil(
        DXGI_FORMAT format, DXGI_FORMAT dsvFormat);

    void addDepthStencil(
        DXGI_FORMAT format, DXGI_FORMAT dsvFormat, DXGI_FORMAT srvFormat);

    void addDepthStencil(
        ComPtr<ID3D11Texture2D>          tex,
        ComPtr<ID3D11DepthStencilView>   dsv,
        ComPtr<ID3D11ShaderResourceView> srv);

    void clearColorBuffer(int index, const Color4 &color);

    void clearDepth(float depth);

    void clearStencil(uint8_t stencil);

    void clearDepthStencil(float depth, uint8_t stencil);

    ComPtr<ID3D11ShaderResourceView> getColorShaderResourceView(int index) const;

    ComPtr<ID3D11ShaderResourceView> getDepthShaderResourceView() const;

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

inline RenderTarget::RenderTarget()
    : RenderTarget({ 640, 480 }, 1, 0)
{
    
}

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

inline RenderTarget::RenderTarget(RenderTarget &&moveFrom) noexcept
    : RenderTarget()
{
    swap(moveFrom);
}

inline RenderTarget &RenderTarget::operator=(RenderTarget &&moveFrom) noexcept
{
    swap(moveFrom);
    return *this;
}

inline void RenderTarget::swap(RenderTarget &swapWith) noexcept
{
    std::swap(width_,      swapWith.width_);
    std::swap(height_,     swapWith.height_);
    std::swap(sampleDesc_, swapWith.sampleDesc_);
    std::swap(colors_,     swapWith.colors_);
    std::swap(rawRTVs_,    swapWith.rawRTVs_);
    std::swap(dsTex_,      swapWith.dsTex_);
    std::swap(DSV_,        swapWith.DSV_);
    std::swap(depthSRV_,   swapWith.depthSRV_);
}

inline Int2 RenderTarget::getSize() const noexcept
{
    return { getWidth(), getHeight() };
}

inline int RenderTarget::getWidth() const noexcept
{
    return static_cast<int>(width_);
}

inline int RenderTarget::getHeight() const noexcept
{
    return static_cast<int>(height_);
}

inline float RenderTarget::getWOverH() const noexcept
{
    return static_cast<float>(width_) / height_;
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

inline void RenderTarget::addColorBuffer(
    ComPtr<ID3D11Texture2D>          tex,
    ComPtr<ID3D11RenderTargetView>   rtv,
    ComPtr<ID3D11ShaderResourceView> srv)
{
    auto rawRTV = rtv.Get();
    colors_.push_back({ std::move(tex), std::move(rtv), std::move(srv) });
    rawRTVs_.push_back(rawRTV);
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

inline void RenderTarget::addDepthStencil(
    ComPtr<ID3D11Texture2D>          tex,
    ComPtr<ID3D11DepthStencilView>   dsv,
    ComPtr<ID3D11ShaderResourceView> srv)
{
    dsTex_    = std::move(tex);
    DSV_      = std::move(dsv);
    depthSRV_ = std::move(srv);
}

inline void RenderTarget::clearColorBuffer(int index, const Color4 &color)
{
    deviceContext.d3dDeviceContext->ClearRenderTargetView(
        rawRTVs_[index], &color.r);
}

inline void RenderTarget::clearDepth(float depth)
{
    deviceContext.d3dDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

inline void RenderTarget::clearStencil(uint8_t stencil)
{
    deviceContext.d3dDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_STENCIL, 1, stencil);
}

inline void RenderTarget::clearDepthStencil(float depth, uint8_t stencil)
{
    deviceContext.d3dDeviceContext->ClearDepthStencilView(
        DSV_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

inline ComPtr<ID3D11ShaderResourceView>
    RenderTarget::getColorShaderResourceView(int index) const
{
    return colors_[index].SRV;
}

inline ComPtr<ID3D11ShaderResourceView>
    RenderTarget::getDepthShaderResourceView() const
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
    deviceContext.d3dDeviceContext->RSSetViewports(1, &vp);
}

inline void RenderTarget::bind() const
{
    deviceContext.d3dDeviceContext->OMSetRenderTargets(
        static_cast<UINT>(colors_.size()), rawRTVs_.data(), DSV_.Get());
}

inline void RenderTarget::unbind() const
{
    ID3D11RenderTargetView *EMPTY_RTV[1] = { nullptr };
    deviceContext.d3dDeviceContext->OMSetRenderTargets(0, EMPTY_RTV, nullptr);
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

    ComPtr<ID3D11Texture2D> tex = device.createTex2D(texDesc, nullptr);
    
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = rtvFormat;
    if(sampleDesc_.Count == 1)
    {
        rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
    }
    else
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    ComPtr<ID3D11RenderTargetView> rtv = device.createRTV(tex, rtvDesc);
    
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

    ComPtr<ID3D11ShaderResourceView> srv = device.createSRV(tex, srvDesc);
    
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

    ComPtr<ID3D11Texture2D> tex = device.createTex2D(texDesc, nullptr);
    
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

    ComPtr<ID3D11DepthStencilView> dsv = device.createDSV(tex, dsvDesc);
    
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

    ComPtr<ID3D11ShaderResourceView> srv = device.createSRV(tex, srvDesc);
    
    dsTex_    = std::move(tex);
    DSV_      = std::move(dsv);
    depthSRV_ = std::move(srv);
}

AGZ_D3D11_END
