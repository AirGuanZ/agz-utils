#pragma once

#include <d3d11.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

inline ComPtr<ID3D11Buffer> CreateD3D11Buffer(
    const D3D11_BUFFER_DESC &desc, const D3D11_SUBRESOURCE_DATA *initData = nullptr);

inline ComPtr<ID3D11Texture2D> CreateTexture2D(
    const D3D11_TEXTURE2D_DESC &desc, const D3D11_SUBRESOURCE_DATA *initData = nullptr);

inline ComPtr<ID3D11ShaderResourceView> CreateShaderResourceView(
    const D3D11_SHADER_RESOURCE_VIEW_DESC &desc, ID3D11Resource *tex);

inline ComPtr<ID3D11SamplerState> CreateSamplerState(const D3D11_SAMPLER_DESC &desc);

inline ComPtr<ID3D11RasterizerState> CreateRasterizerState(const D3D11_RASTERIZER_DESC &desc);

inline ComPtr<ID3D11BlendState> CreateBlendState(const D3D11_BLEND_DESC &desc);

inline ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC &desc);

//############################# impl #############################

inline ComPtr<ID3D11Buffer> CreateD3D11Buffer(
    const D3D11_BUFFER_DESC &desc, const D3D11_SUBRESOURCE_DATA *initData)
{
    ComPtr<ID3D11Buffer> buffer;
    HRESULT hr = gDevice->CreateBuffer(&desc, initData, buffer.GetAddressOf());
    return SUCCEEDED(hr) ? buffer : nullptr;
}

inline ComPtr<ID3D11Texture2D> CreateTexture2D(
    const D3D11_TEXTURE2D_DESC &desc, const D3D11_SUBRESOURCE_DATA *initData)
{
    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = gDevice->CreateTexture2D(&desc, initData, texture.GetAddressOf());
    return SUCCEEDED(hr) ? texture : nullptr;
}

inline ComPtr<ID3D11ShaderResourceView> CreateShaderResourceView(
    const D3D11_SHADER_RESOURCE_VIEW_DESC &desc, ID3D11Resource *tex)
{
    ComPtr<ID3D11ShaderResourceView> srv;
    HRESULT hr = gDevice->CreateShaderResourceView(tex, &desc, srv.GetAddressOf());
    return SUCCEEDED(hr) ? srv : nullptr;
}

inline ComPtr<ID3D11SamplerState> CreateSamplerState(const D3D11_SAMPLER_DESC &desc)
{
    ComPtr<ID3D11SamplerState> sampler;
    HRESULT hr = gDevice->CreateSamplerState(&desc, sampler.GetAddressOf());
    return SUCCEEDED(hr) ? sampler : nullptr;
}

inline ComPtr<ID3D11RasterizerState> CreateRasterizerState(const D3D11_RASTERIZER_DESC &desc)
{
    ComPtr<ID3D11RasterizerState> state;
    HRESULT hr = gDevice->CreateRasterizerState(&desc, state.GetAddressOf());
    return SUCCEEDED(hr) ? state : nullptr;
}

inline ComPtr<ID3D11BlendState> CreateBlendState(const D3D11_BLEND_DESC &desc)
{
    ComPtr<ID3D11BlendState> state;
    HRESULT hr = gDevice->CreateBlendState(&desc, state.GetAddressOf());
    return SUCCEEDED(hr) ? state : nullptr;
}

inline ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC &desc)
{
    ComPtr<ID3D11DepthStencilState> state;
    HRESULT hr = gDevice->CreateDepthStencilState(&desc, state.GetAddressOf());
    return SUCCEEDED(hr) ? state : nullptr;
}

AGZ_D3D11_END
