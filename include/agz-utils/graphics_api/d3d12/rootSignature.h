#pragma once

#include <d3dx12.h>

#include <agz-utils/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class RootSignatureBuilder : public misc::uncopyable_t
{
    std::vector<D3D12_ROOT_PARAMETER>      parameters_;
    std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
    D3D12_ROOT_SIGNATURE_FLAGS             flags_;

public:

    RootSignatureBuilder();

    template<int N>
    void addParameters(const D3D12_ROOT_PARAMETER (&params)[N])
    {
        for(auto &p : params)
            this->addParameter(p);
    }

    template<int N>
    void addParameters(const CD3DX12_ROOT_PARAMETER (&params)[N])
    {
        for(auto &p : params)
            this->addParameter(p);
    }

    void addParameter(const D3D12_ROOT_PARAMETER &parameter);

    void addParameter(
        const D3D12_ROOT_DESCRIPTOR_TABLE &table,
        D3D12_SHADER_VISIBILITY            vis = D3D12_SHADER_VISIBILITY_ALL);

    void addParameter(
        const D3D12_ROOT_CONSTANTS &consts,
        D3D12_SHADER_VISIBILITY     vis = D3D12_SHADER_VISIBILITY_ALL);

    void addParameterCBV(
        const ShaderRegister   &reg,
        D3D12_SHADER_VISIBILITY vis = D3D12_SHADER_VISIBILITY_ALL);

    void addParameterSRV(
        const ShaderRegister   &reg,
        D3D12_SHADER_VISIBILITY vis = D3D12_SHADER_VISIBILITY_ALL);

    void addParameterUAV(
        const ShaderRegister   &reg,
        D3D12_SHADER_VISIBILITY vis = D3D12_SHADER_VISIBILITY_ALL);

    void addStaticSampler(const D3D12_STATIC_SAMPLER_DESC &staticSampler);

    void addStaticSampler(
        ShaderRegister             reg,
        D3D12_SHADER_VISIBILITY    vis,
        D3D12_FILTER               filter,
        D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    void addFlags(D3D12_ROOT_SIGNATURE_FLAGS flags);

    ComPtr<ID3D12RootSignature> build(ID3D12Device *device) const;
};

AGZ_D3D12_END
