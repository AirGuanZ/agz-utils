#pragma once

#include <agz/utility/d3d11/D3D/ComObjectHolder.h>
#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

class Sampler : public ComObjectHolder<ID3D11SamplerState>
{
public:

    static const float *DefaultBorderColor() noexcept
    {
        static const float ret[] = { 0, 0, 0, 0 };
        return ret;
    }

    void Initialize(
        D3D11_FILTER               filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_MODE wrapU          = D3D11_TEXTURE_ADDRESS_CLAMP,
        D3D11_TEXTURE_ADDRESS_MODE wrapV          = D3D11_TEXTURE_ADDRESS_CLAMP,
        D3D11_TEXTURE_ADDRESS_MODE wrapW          = D3D11_TEXTURE_ADDRESS_CLAMP,
        float                      mipLODBias     = 0,
        UINT                       maxAnisotropic = 1,
        D3D11_COMPARISON_FUNC      comparisonFunc = D3D11_COMPARISON_NEVER,
        const float                borderColor[4] = DefaultBorderColor(),
        float                      minLOD         = -FLT_MAX,
        float                      maxLOD         = FLT_MAX)
    {
        D3D11_SAMPLER_DESC desc;
        desc.Filter         = filter;
        desc.AddressU       = wrapU;
        desc.AddressV       = wrapV;
        desc.AddressW       = wrapW;
        desc.MipLODBias     = mipLODBias;
        desc.MaxAnisotropy  = maxAnisotropic;
        desc.ComparisonFunc = comparisonFunc;
        desc.MinLOD         = minLOD;
        desc.MaxLOD         = maxLOD;
        memcpy(desc.BorderColor, borderColor, sizeof(float) * 4);

        obj_ = CreateSamplerState(desc);
        if(!obj_)
        {
            throw VRPGBaseException("failed to create d3d sampler state");
        }
    }
};

AGZ_D3D11_END
