#pragma once

#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

class DepthState
{
    ComPtr<ID3D11DepthStencilState> state_;

public:

    void Initialize(
        bool enableDepthTest,
        D3D11_DEPTH_WRITE_MASK writeMask = D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_LESS)
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        desc.DepthEnable                  = enableDepthTest;
        desc.DepthWriteMask               = writeMask;
        desc.DepthFunc                    = comparisonFunc;
        desc.StencilEnable                = false;
        desc.StencilReadMask              = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask             = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;

        state_ = CreateDepthStencilState(desc);
        if(!state_)
        {
            throw VRPGBaseException("failed to create d3d11 depth stencil state");
        }
    }

    bool IsAvailable() const noexcept
    {
        return state_ != nullptr;
    }

    void Destroy()
    {
        state_.Reset();
    }

    ID3D11DepthStencilState *Get() const noexcept
    {
        return state_.Get();
    }

    operator ID3D11DepthStencilState*() const noexcept
    {
        return state_.Get();
    }

    void Bind() const
    {
        gDeviceContext->OMSetDepthStencilState(state_.Get(), 0);
    }

    void Unbind() const
    {
        gDeviceContext->OMSetDepthStencilState(nullptr, 0);
    }
};

AGZ_D3D11_END
