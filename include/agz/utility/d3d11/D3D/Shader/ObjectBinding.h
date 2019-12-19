#pragma once

#include <d3d11.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

enum class ShaderStage
{
    VS = 0,
    HS = 1,
    DS = 2,
    GS = 3,
    PS = 4
};

constexpr ShaderStage SS_VS = ShaderStage::VS;
constexpr ShaderStage SS_HS = ShaderStage::HS;
constexpr ShaderStage SS_DS = ShaderStage::DS;
constexpr ShaderStage SS_GS = ShaderStage::GS;
constexpr ShaderStage SS_PS = ShaderStage::PS;

namespace ObjectBinding
{

    template<ShaderStage STAGE>
    void BindConstantBuffer(UINT slot, ID3D11Buffer *buffer);

    template<ShaderStage STAGE>
    void BindShaderResourceView(UINT slot, ID3D11ShaderResourceView *srv);

    template<ShaderStage STAGE>
    void BindShaderResourceViewArray(UINT slot, UINT count, ID3D11ShaderResourceView **srvs);

    template<ShaderStage STAGE>
    void BindSampler(UINT slot, ID3D11SamplerState *sampler);

    template<>
    inline void BindConstantBuffer<ShaderStage::VS>(UINT slot, ID3D11Buffer *buffer)
    {
        gDeviceContext->VSSetConstantBuffers(slot, 1, &buffer);
    }

    template<>
    inline void BindConstantBuffer<ShaderStage::HS>(UINT slot, ID3D11Buffer *buffer)
    {
        gDeviceContext->HSSetConstantBuffers(slot, 1, &buffer);
    }

    template<>
    inline void BindConstantBuffer<ShaderStage::DS>(UINT slot, ID3D11Buffer *buffer)
    {
        gDeviceContext->DSSetConstantBuffers(slot, 1, &buffer);
    }

    template<>
    inline void BindConstantBuffer<ShaderStage::GS>(UINT slot, ID3D11Buffer *buffer)
    {
        gDeviceContext->GSSetConstantBuffers(slot, 1, &buffer);
    }

    template<>
    inline void BindConstantBuffer<ShaderStage::PS>(UINT slot, ID3D11Buffer *buffer)
    {
        gDeviceContext->PSSetConstantBuffers(slot, 1, &buffer);
    }

    template<>
    inline void BindShaderResourceView<ShaderStage::VS>(UINT slot, ID3D11ShaderResourceView *srv)
    {
        gDeviceContext->VSSetShaderResources(slot, 1, &srv);
    }

    template<>
    inline void BindShaderResourceView<ShaderStage::HS>(UINT slot, ID3D11ShaderResourceView *srv)
    {
        gDeviceContext->HSSetShaderResources(slot, 1, &srv);
    }

    template<>
    inline void BindShaderResourceView<ShaderStage::DS>(UINT slot, ID3D11ShaderResourceView *srv)
    {
        gDeviceContext->DSSetShaderResources(slot, 1, &srv);
    }

    template<>
    inline void BindShaderResourceView<ShaderStage::GS>(UINT slot, ID3D11ShaderResourceView *srv)
    {
        gDeviceContext->GSSetShaderResources(slot, 1, &srv);
    }

    template<>
    inline void BindShaderResourceView<ShaderStage::PS>(UINT slot, ID3D11ShaderResourceView *srv)
    {
        gDeviceContext->PSSetShaderResources(slot, 1, &srv);
    }

    template<>
    inline void BindShaderResourceViewArray<ShaderStage::VS>(UINT slot, UINT count, ID3D11ShaderResourceView **srvs)
    {
        gDeviceContext->VSSetShaderResources(slot, count, srvs);
    }

    template<>
    inline void BindShaderResourceViewArray<ShaderStage::HS>(UINT slot, UINT count, ID3D11ShaderResourceView **srvs)
    {
        gDeviceContext->HSSetShaderResources(slot, count, srvs);
    }

    template<>
    inline void BindShaderResourceViewArray<ShaderStage::DS>(UINT slot, UINT count, ID3D11ShaderResourceView **srvs)
    {
        gDeviceContext->DSSetShaderResources(slot, count, srvs);
    }

    template<>
    inline void BindShaderResourceViewArray<ShaderStage::GS>(UINT slot, UINT count, ID3D11ShaderResourceView **srvs)
    {
        gDeviceContext->GSSetShaderResources(slot, count, srvs);
    }

    template<>
    inline void BindShaderResourceViewArray<ShaderStage::PS>(UINT slot, UINT count, ID3D11ShaderResourceView **srvs)
    {
        gDeviceContext->PSSetShaderResources(slot, count, srvs);
    }

    template<>
    inline void BindSampler<ShaderStage::VS>(UINT slot, ID3D11SamplerState *sampler)
    {
        gDeviceContext->VSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindSampler<ShaderStage::HS>(UINT slot, ID3D11SamplerState *sampler)
    {
        gDeviceContext->HSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindSampler<ShaderStage::DS>(UINT slot, ID3D11SamplerState *sampler)
    {
        gDeviceContext->DSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindSampler<ShaderStage::GS>(UINT slot, ID3D11SamplerState *sampler)
    {
        gDeviceContext->GSSetSamplers(slot, 1, &sampler);
    }

    template<>
    inline void BindSampler<ShaderStage::PS>(UINT slot, ID3D11SamplerState *sampler)
    {
        gDeviceContext->PSSetSamplers(slot, 1, &sampler);
    }

} // namespace ObjectBinding

AGZ_D3D11_END
