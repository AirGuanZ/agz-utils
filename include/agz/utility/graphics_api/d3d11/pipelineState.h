#pragma once

#include "device.h"
#include "shader.h"

AGZ_D3D11_BEGIN

class PipelineState
{
    ComPtr<ID3D11BlendState>        blend_;
    float                           blendFactors_[4] = { 1, 1, 1, 1 };
    ComPtr<ID3D11DepthStencilState> depthStencil_;
    ComPtr<ID3D11RasterizerState>   rasterizer_;

public:

    static PipelineState getCurrent();

    void bind() const;

    void unbind() const;

    PipelineState &setBlendState(
        ComPtr<ID3D11BlendState> blendState, float blendFactors[4]) noexcept;

    PipelineState &setBlendState(
        bool enabled,
        D3D11_BLEND srcRGB,   D3D11_BLEND dstRGB,   D3D11_BLEND_OP opRGB,
        D3D11_BLEND srcAlpha, D3D11_BLEND dstAlpha, D3D11_BLEND_OP opAlpha,
        float blendFactors[4]);

    PipelineState &setDepthStencilState(
        ComPtr<ID3D11DepthStencilState> depthStencil) noexcept;

    PipelineState &setDepthStencilState(
        bool                   enableDepth,
        D3D11_DEPTH_WRITE_MASK depthWrite              = D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_FUNC  depthFunc               = D3D11_COMPARISON_LESS,
        bool                   enableStencil           = false,
        uint8_t                stencilReadMask         = D3D11_DEFAULT_STENCIL_READ_MASK,
        uint8_t                stencilWriteMask        = D3D11_DEFAULT_STENCIL_WRITE_MASK,
        D3D11_STENCIL_OP       frontStencilFailOp      = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP       frontStencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP       frontStencilPassOp      = D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_FUNC  frontStencilFunc        = D3D11_COMPARISON_ALWAYS,
        D3D11_STENCIL_OP       backStencilFailOp       = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP       backStencilDepthFailOp  = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP       backStencilPassOp       = D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_FUNC  backStencilFunc         = D3D11_COMPARISON_ALWAYS);

    PipelineState &setRasterizerState(
        ComPtr<ID3D11RasterizerState> rasterizer) noexcept;

    PipelineState &setRasterizerState(
        D3D11_FILL_MODE fillMode,
        D3D11_CULL_MODE cullMode,
        bool            isFrontClockwise,
        int             depthBias             = D3D11_DEFAULT_DEPTH_BIAS,
        float           depthBiasClamp        = D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        float           slopeScaledDepthBias  = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        bool            enableDepthClip       = true,
        bool            enableScissor         = false,
        bool            enableMultisample     = false,
        bool            enableAntialiasedLine = false);
};

inline PipelineState PipelineState::getCurrent()
{
    PipelineState ret;

    UINT sampleMask;
    deviceContext.d3dDeviceContext->OMGetBlendState(
        ret.blend_.GetAddressOf(), ret.blendFactors_, &sampleMask);

    UINT stencilRef;
    deviceContext.d3dDeviceContext->OMGetDepthStencilState(
        ret.depthStencil_.GetAddressOf(), &stencilRef);

    deviceContext.d3dDeviceContext->RSGetState(ret.rasterizer_.GetAddressOf());

    return ret;
}

inline void PipelineState::bind() const
{
    deviceContext.d3dDeviceContext->OMSetBlendState(
        blend_.Get(), blendFactors_, 0xffffffff);
    deviceContext.d3dDeviceContext->OMSetDepthStencilState(depthStencil_.Get(), 0);
    deviceContext.d3dDeviceContext->RSSetState(rasterizer_.Get());
}

inline void PipelineState::unbind() const
{
    deviceContext.d3dDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    deviceContext.d3dDeviceContext->OMSetDepthStencilState(nullptr, 0);
    deviceContext.d3dDeviceContext->RSSetState(nullptr);
}

inline PipelineState &PipelineState::setBlendState(
    ComPtr<ID3D11BlendState> blendState, float blendFactors[4]) noexcept
{
    blend_.Swap(blendState);
    if(blendFactors)
    {
        blendFactors_[0] = blendFactors[0];
        blendFactors_[1] = blendFactors[1];
        blendFactors_[2] = blendFactors[2];
        blendFactors_[3] = blendFactors[3];
    }
    else
    {
        blendFactors_[0] = 0;
        blendFactors_[1] = 0;
        blendFactors_[2] = 0;
        blendFactors_[3] = 0;
    }
    return *this;
}

inline PipelineState &PipelineState::setBlendState(
    bool enabled,
    D3D11_BLEND srcRGB,   D3D11_BLEND dstRGB,   D3D11_BLEND_OP opRGB,
    D3D11_BLEND srcAlpha, D3D11_BLEND dstAlpha, D3D11_BLEND_OP opAlpha,
    float blendFactors[4])
{
    D3D11_BLEND_DESC desc = {};
    desc.RenderTarget[0].BlendEnable           = enabled;
    desc.RenderTarget[0].SrcBlend              = srcRGB;
    desc.RenderTarget[0].DestBlend             = dstRGB;
    desc.RenderTarget[0].BlendOp               = opRGB;
    desc.RenderTarget[0].SrcBlendAlpha         = srcAlpha;
    desc.RenderTarget[0].DestBlendAlpha        = dstAlpha;
    desc.RenderTarget[0].BlendOpAlpha          = opAlpha;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ComPtr<ID3D11BlendState> newBlend = device.createBlendState(desc);
    
    if(blendFactors)
    {
        blendFactors_[0] = blendFactors[0];
        blendFactors_[1] = blendFactors[1];
        blendFactors_[2] = blendFactors[2];
        blendFactors_[3] = blendFactors[3];
    }
    else
    {
        blendFactors_[0] = 1;
        blendFactors_[1] = 1;
        blendFactors_[2] = 1;
        blendFactors_[3] = 1;
    }

    blend_.Swap(newBlend);
    return *this;
}

inline PipelineState &PipelineState::setDepthStencilState(
    ComPtr<ID3D11DepthStencilState> depthStencil) noexcept
{
    depthStencil_.Swap(depthStencil);
    return *this;
}

inline PipelineState &PipelineState::setDepthStencilState(
    bool                   enableDepth,
    D3D11_DEPTH_WRITE_MASK depthWrite,
    D3D11_COMPARISON_FUNC  depthFunc,
    bool                   enableStencil,
    uint8_t                stencilReadMask,
    uint8_t                stencilWriteMask,
    D3D11_STENCIL_OP       frontStencilFailOp,
    D3D11_STENCIL_OP       frontStencilDepthFailOp,
    D3D11_STENCIL_OP       frontStencilPassOp,
    D3D11_COMPARISON_FUNC  frontStencilFunc,
    D3D11_STENCIL_OP       backStencilFailOp,
    D3D11_STENCIL_OP       backStencilDepthFailOp,
    D3D11_STENCIL_OP       backStencilPassOp,
    D3D11_COMPARISON_FUNC  backStencilFunc)
{
    D3D11_DEPTH_STENCIL_DESC desc;
    desc.DepthEnable                  = enableDepth;
    desc.DepthWriteMask               = depthWrite;
    desc.DepthFunc                    = depthFunc;
    desc.StencilEnable                = enableStencil;
    desc.StencilReadMask              = stencilReadMask;
    desc.StencilWriteMask             = stencilWriteMask;
    desc.FrontFace.StencilFailOp      = frontStencilFailOp;
    desc.FrontFace.StencilDepthFailOp = frontStencilDepthFailOp;
    desc.FrontFace.StencilPassOp      = frontStencilPassOp;
    desc.FrontFace.StencilFunc        = frontStencilFunc;
    desc.BackFace.StencilFailOp       = backStencilFailOp;
    desc.BackFace.StencilDepthFailOp  = backStencilDepthFailOp;
    desc.BackFace.StencilPassOp       = backStencilPassOp;
    desc.BackFace.StencilFunc         = backStencilFunc;

    ComPtr<ID3D11DepthStencilState> newState =
        device.createDepthStencilState(desc);
    
    depthStencil_.Swap(newState);
    return *this;
}

inline PipelineState &PipelineState::setRasterizerState(
    ComPtr<ID3D11RasterizerState> rasterizer) noexcept
{
    rasterizer_.Swap(rasterizer);
    return *this;
}

inline PipelineState &PipelineState::setRasterizerState(
    D3D11_FILL_MODE fillMode,
    D3D11_CULL_MODE cullMode,
    bool            isFrontClockwise,
    int             depthBias,
    float           depthBiasClamp,
    float           slopeScaledDepthBias,
    bool            enableDepthClip,
    bool            enableScissor,
    bool            enableMultisample,
    bool            enableAntialiasedLine)
{
    D3D11_RASTERIZER_DESC desc;
    desc.FillMode              = fillMode;
    desc.CullMode              = cullMode;
    desc.FrontCounterClockwise = !isFrontClockwise;
    desc.DepthBias             = depthBias;
    desc.DepthBiasClamp        = depthBiasClamp;
    desc.SlopeScaledDepthBias  = slopeScaledDepthBias;
    desc.DepthClipEnable       = enableDepthClip;
    desc.ScissorEnable         = enableScissor;
    desc.MultisampleEnable     = enableMultisample;
    desc.AntialiasedLineEnable = enableAntialiasedLine;

    ComPtr<ID3D11RasterizerState> newState = device.createRasterizerState(desc);
    
    rasterizer_.Swap(newState);
    return *this;
}

AGZ_D3D11_END
