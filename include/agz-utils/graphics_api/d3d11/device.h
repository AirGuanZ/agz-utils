#pragma once

#include "common.h"

AGZ_D3D11_BEGIN

class Device
{
public:

    ID3D11Device *d3dDevice;

    ComPtr<ID3D11Texture2D> createTex2D(
        const D3D11_TEXTURE2D_DESC   &desc,
        const D3D11_SUBRESOURCE_DATA *subrscData = nullptr);

    ComPtr<ID3D11Texture3D> createTex3D(
        const D3D11_TEXTURE3D_DESC   &desc,
        const D3D11_SUBRESOURCE_DATA *subrscData = nullptr);

    ComPtr<ID3D11Buffer> createBuffer(
        const D3D11_BUFFER_DESC      &desc,
        const D3D11_SUBRESOURCE_DATA *subrscData = nullptr);

    ComPtr<ID3D11ShaderResourceView> createSRV(
        ID3D11Resource                        *rsc,
        const D3D11_SHADER_RESOURCE_VIEW_DESC &desc);

    ComPtr<ID3D11ShaderResourceView> createSRV(
        const ComPtr<ID3D11Resource>          &rsc,
        const D3D11_SHADER_RESOURCE_VIEW_DESC &desc);

    ComPtr<ID3D11UnorderedAccessView> createUAV(
        ID3D11Resource                         *rsc,
        const D3D11_UNORDERED_ACCESS_VIEW_DESC &desc);

    ComPtr<ID3D11UnorderedAccessView> createUAV(
        const ComPtr<ID3D11Resource>            &rsc,
        const D3D11_UNORDERED_ACCESS_VIEW_DESC  &desc);

    ComPtr<ID3D11RenderTargetView> createRTV(
        ID3D11Resource                       *rsc,
        const D3D11_RENDER_TARGET_VIEW_DESC  &desc);

    ComPtr<ID3D11RenderTargetView> createRTV(
        const ComPtr<ID3D11Resource>         &rsc,
        const D3D11_RENDER_TARGET_VIEW_DESC  &desc);
    
    ComPtr<ID3D11DepthStencilView> createDSV(
        ID3D11Resource                      *rsc,
        const D3D11_DEPTH_STENCIL_VIEW_DESC &desc);

    ComPtr<ID3D11DepthStencilView> createDSV(
        const ComPtr<ID3D11Resource>        &rsc,
        const D3D11_DEPTH_STENCIL_VIEW_DESC &desc);

    ComPtr<ID3D11BlendState> createBlendState(
        const D3D11_BLEND_DESC &desc);

    ComPtr<ID3D11DepthStencilState> createDepthStencilState(
        const D3D11_DEPTH_STENCIL_DESC &desc);

    ComPtr<ID3D11RasterizerState> createRasterizerState(
        const D3D11_RASTERIZER_DESC &desc);

    ComPtr<ID3D11SamplerState> createSampler(
        const D3D11_SAMPLER_DESC &desc);

    ComPtr<ID3D11SamplerState> createSampler(
        D3D11_FILTER               filter,
        D3D11_TEXTURE_ADDRESS_MODE addressU,
        D3D11_TEXTURE_ADDRESS_MODE addressV,
        D3D11_TEXTURE_ADDRESS_MODE addressW,
        float                      mipLODBias     = 0,
        UINT                       maxAnisotropy  = 1,
        D3D11_COMPARISON_FUNC      comparisonFunc = D3D11_COMPARISON_NEVER,
        const Color4              &borderColor    = { 0, 0, 0, 0 },
        float                      minLOD         = -FLT_MAX,
        float                      maxLOD         = +FLT_MAX);
};

inline Device device = { nullptr };

AGZ_D3D11_END
