#include <agz/utility/graphics_api/d3d11/device.h>

AGZ_D3D11_BEGIN

ComPtr<ID3D11Texture2D> Device::createTex2D(
    const D3D11_TEXTURE2D_DESC   &desc,
    const D3D11_SUBRESOURCE_DATA *subrscData)
{
    ComPtr<ID3D11Texture2D> ret;
    if(FAILED(d3dDevice->CreateTexture2D(
        &desc, subrscData, ret.GetAddressOf())))
        throw D3D11Exception("failed to create texture2d");
    return ret;
}

ComPtr<ID3D11Texture3D> Device::createTex3D(
    const D3D11_TEXTURE3D_DESC   &desc,
    const D3D11_SUBRESOURCE_DATA *subrscData)
{
    ComPtr<ID3D11Texture3D> ret;
    if(FAILED(d3dDevice->CreateTexture3D(
        &desc, subrscData, ret.GetAddressOf())))
        throw D3D11Exception("failed to create texture3d");
    return ret;
}

ComPtr<ID3D11Buffer> Device::createBuffer(
    const D3D11_BUFFER_DESC      &desc,
    const D3D11_SUBRESOURCE_DATA *subrscData)
{
    ComPtr<ID3D11Buffer> ret;
    if(FAILED(d3dDevice->CreateBuffer(
        &desc, subrscData, ret.GetAddressOf())))
        throw D3D11Exception("failed to create buffer");
    return ret;
}

ComPtr<ID3D11ShaderResourceView> Device::createSRV(
    ID3D11Resource                        *rsc,
    const D3D11_SHADER_RESOURCE_VIEW_DESC &desc)
{
    ComPtr<ID3D11ShaderResourceView> ret;
    if(FAILED(d3dDevice->CreateShaderResourceView(
        rsc, &desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create shader resource view");
    return ret;
}

ComPtr<ID3D11ShaderResourceView> Device::createSRV(
    const ComPtr<ID3D11Resource>          &rsc,
    const D3D11_SHADER_RESOURCE_VIEW_DESC &desc)
{
    return createSRV(rsc.Get(), desc);
}

ComPtr<ID3D11UnorderedAccessView> Device::createUAV(
    ID3D11Resource                         *rsc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    ComPtr<ID3D11UnorderedAccessView> ret;
    if(FAILED(d3dDevice->CreateUnorderedAccessView(
        rsc, &desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create unordered access view");
    return ret;
}

ComPtr<ID3D11UnorderedAccessView> Device::createUAV(
    const ComPtr<ID3D11Resource>           &rsc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    return createUAV(rsc.Get(), desc);
}

ComPtr<ID3D11RenderTargetView> Device::createRTV(
    ID3D11Resource                      *rsc,
    const D3D11_RENDER_TARGET_VIEW_DESC &desc)
{
    ComPtr<ID3D11RenderTargetView> ret;
    if(FAILED(d3dDevice->CreateRenderTargetView(
        rsc, &desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create render target view");
    return ret;
}

ComPtr<ID3D11RenderTargetView> Device::createRTV(
    const ComPtr<ID3D11Resource>        &rsc,
    const D3D11_RENDER_TARGET_VIEW_DESC &desc)
{
    return createRTV(rsc.Get(), desc);
}

ComPtr<ID3D11DepthStencilView> Device::createDSV(
    ID3D11Resource                      *rsc,
    const D3D11_DEPTH_STENCIL_VIEW_DESC &desc)
{
    ComPtr<ID3D11DepthStencilView> ret;
    if(FAILED(d3dDevice->CreateDepthStencilView(
        rsc, &desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create depth stencil view");
    return ret;
}

ComPtr<ID3D11DepthStencilView> Device::createDSV(
    const ComPtr<ID3D11Resource>        &rsc,
    const D3D11_DEPTH_STENCIL_VIEW_DESC &desc)
{
    return createDSV(rsc.Get(), desc);
}

ComPtr<ID3D11BlendState> Device::createBlendState(
    const D3D11_BLEND_DESC &desc)
{
    ComPtr<ID3D11BlendState> ret;
    if(FAILED(d3dDevice->CreateBlendState(&desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create blend state");
    return ret;
}

ComPtr<ID3D11DepthStencilState> Device::createDepthStencilState(
    const D3D11_DEPTH_STENCIL_DESC &desc)
{
    ComPtr<ID3D11DepthStencilState> ret;
    if(FAILED(d3dDevice->CreateDepthStencilState(&desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create depth stencil state");
    return ret;
}

ComPtr<ID3D11RasterizerState> Device::createRasterizerState(
    const D3D11_RASTERIZER_DESC &desc)
{
    ComPtr<ID3D11RasterizerState> ret;
    if(FAILED(d3dDevice->CreateRasterizerState(&desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create rasterizer state");
    return ret;
}

ComPtr<ID3D11SamplerState> Device::createSampler(
    const D3D11_SAMPLER_DESC &desc)
{
    ComPtr<ID3D11SamplerState> ret;
    if(FAILED(d3dDevice->CreateSamplerState(&desc, ret.GetAddressOf())))
        throw D3D11Exception("failed to create sampler");
    return ret;
}

ComPtr<ID3D11SamplerState> Device::createSampler(
    D3D11_FILTER               filter,
    D3D11_TEXTURE_ADDRESS_MODE addressU,
    D3D11_TEXTURE_ADDRESS_MODE addressV,
    D3D11_TEXTURE_ADDRESS_MODE addressW,
    float                      mipLODBias,
    UINT                       maxAnisotropy,
    D3D11_COMPARISON_FUNC      comparisonFunc,
    const Color4              &borderColor,
    float                      minLOD,
    float                      maxLOD)
{
    D3D11_SAMPLER_DESC desc;
    desc.Filter         = filter;
    desc.AddressU       = addressU;
    desc.AddressV       = addressV;
    desc.AddressW       = addressW;
    desc.MipLODBias     = mipLODBias;
    desc.MaxAnisotropy  = maxAnisotropy;
    desc.ComparisonFunc = comparisonFunc;
    desc.BorderColor[0] = borderColor.r;
    desc.BorderColor[1] = borderColor.g;
    desc.BorderColor[2] = borderColor.b;
    desc.BorderColor[3] = borderColor.a;
    desc.MinLOD         = minLOD;
    desc.MaxLOD         = maxLOD;

    return createSampler(desc);
}

AGZ_D3D11_END
