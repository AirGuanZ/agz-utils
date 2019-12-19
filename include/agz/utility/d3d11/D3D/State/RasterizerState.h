#pragma once

#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

class RasterizerState
{
    ComPtr<ID3D11RasterizerState> rasterizerState_;

public:

    void Initialize(
        D3D11_FILL_MODE fillMode,
        D3D11_CULL_MODE cullMode,
        bool frontCounterClockwise,
        UINT depthBias              = 0,
        float depthBiasClamp       = 0,
        float slopeScaledDepthBias = 0,
        bool enableDepthClip       = true,
        bool enableScissor         = false,
        bool enableMultisample     = false,
        bool enableAntialiasedLine = false)
    {
        D3D11_RASTERIZER_DESC desc;
        desc.FillMode              = fillMode;
        desc.CullMode              = cullMode;
        desc.FrontCounterClockwise = frontCounterClockwise;
        desc.DepthBias             = depthBias;
        desc.DepthBiasClamp        = depthBiasClamp;
        desc.SlopeScaledDepthBias  = slopeScaledDepthBias;
        desc.DepthClipEnable       = enableDepthClip;
        desc.ScissorEnable         = enableScissor;
        desc.MultisampleEnable     = enableMultisample;
        desc.AntialiasedLineEnable = enableAntialiasedLine;

        rasterizerState_ = CreateRasterizerState(desc);
        if(!rasterizerState_)
        {
            throw VRPGBaseException("failed to create rasterizer state");
        }
    }

    bool IsAvailable() const noexcept
    {
        return rasterizerState_ != nullptr;
    }

    void Destroy()
    {
        rasterizerState_.Reset();
    }

    ID3D11RasterizerState *Get() const noexcept
    {
        return rasterizerState_.Get();
    }

    operator ID3D11RasterizerState*() const noexcept
    {
        return rasterizerState_.Get();
    }

    void Bind() const
    {
        gDeviceContext->RSSetState(rasterizerState_.Get());
    }

    void Unbind() const
    {
        gDeviceContext->RSSetState(nullptr);
    }
};

AGZ_D3D11_END
