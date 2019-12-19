#pragma once

#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

class BlendStateBuilder
{
    D3D11_BLEND_DESC desc_;

public:

    explicit BlendStateBuilder(bool alphaToCoverage = false, bool independentBlend = false)
    {
        desc_.AlphaToCoverageEnable = alphaToCoverage;
        desc_.IndependentBlendEnable = independentBlend;

        for(auto &desc : desc_.RenderTarget)
        {
            desc.BlendEnable           = false;
            desc.SrcBlend              = D3D11_BLEND_ONE;
            desc.DestBlend             = D3D11_BLEND_ZERO;
            desc.BlendOp               = D3D11_BLEND_OP_ADD;
            desc.SrcBlendAlpha         = D3D11_BLEND_ONE;
            desc.DestBlendAlpha        = D3D11_BLEND_ZERO;
            desc.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
            desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
    }

    BlendStateBuilder &Set(
        int renderTargetIndex,
        bool enableBlend,
        D3D11_BLEND srcColor,
        D3D11_BLEND dstColor,
        D3D11_BLEND_OP opColor,
        D3D11_BLEND srcAlpha,
        D3D11_BLEND dstAlpha,
        D3D11_BLEND_OP opAlpha,
        UINT8 renderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL)
    {
        auto &desc = desc_.RenderTarget[renderTargetIndex];
        desc.BlendEnable           = enableBlend;
        desc.SrcBlend              = srcColor;
        desc.DestBlend             = dstColor;
        desc.BlendOp               = opColor;
        desc.SrcBlendAlpha         = srcAlpha;
        desc.DestBlendAlpha        = dstAlpha;
        desc.BlendOpAlpha          = opAlpha;
        desc.RenderTargetWriteMask = renderTargetWriteMask;

        return *this;
    }

    ComPtr<ID3D11BlendState> Build() const
    {
        ComPtr<ID3D11BlendState> blendState;
        HRESULT hr = gDevice->CreateBlendState(&desc_, blendState.GetAddressOf());
        if(FAILED(hr))
        {
            throw VRPGBaseException("failed to create d3d11 blend state");
        }
        return blendState;
    }
};

class BlendState
{
    ComPtr<ID3D11BlendState> blendState_;
    float blendFactor_[4] = { 1, 1, 1, 1 };
    UINT sampleMask_ = 0xffffffff;

public:

    static const float *_allOnes() noexcept
    {
        static const float ret[] = { 1, 1, 1, 1 };
        return ret;
    }

    BlendState() = default;

    explicit BlendState(ComPtr<ID3D11BlendState> blendState, const float blendFactor[] = _allOnes(), UINT sampleMask = 0xffffffff) noexcept
        : blendState_(std::move(blendState)),
          blendFactor_{ blendFactor[0], blendFactor[1], blendFactor[2], blendFactor[3] },
          sampleMask_(sampleMask)
    {
        
    }

    BlendState &operator=(ComPtr<ID3D11BlendState> blendState) noexcept
    {
        blendState_ = std::move(blendState);
        return *this;
    }

    bool IsAvailable() const noexcept
    {
        return blendState_ != nullptr;
    }

    void Destroy()
    {
        blendState_.Reset();
        blendFactor_[0] = blendFactor_[1] = blendFactor_[2] = blendFactor_[3] = 1;
        sampleMask_ = 0xffffffff;
    }

    void SetBlendFactor(const float blendFactor[]) noexcept
    {
        assert(blendFactor);
        blendFactor_[0] = blendFactor[0];
        blendFactor_[1] = blendFactor[1];
        blendFactor_[2] = blendFactor[2];
        blendFactor_[3] = blendFactor[3];
    }

    const float *GetBlendFactor() const noexcept
    {
        return blendFactor_;
    }

    void SetSampleMask(UINT mask) noexcept
    {
        sampleMask_ = mask;
    }

    UINT GetSampleMask() const noexcept
    {
        return sampleMask_;
    }

    ID3D11BlendState *GetBlendState() const noexcept
    {
        return blendState_.Get();
    }

    operator ID3D11BlendState*() const noexcept
    {
        return blendState_.Get();
    }

    void Bind() const
    {
        gDeviceContext->OMSetBlendState(blendState_.Get(), blendFactor_, sampleMask_);
    }

    void Unbind() const
    {
        gDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }
};

AGZ_D3D11_END
