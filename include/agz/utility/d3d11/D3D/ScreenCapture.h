#pragma once

#include <agz/utility/d3d11/Common.h>
#include <agz/utility/texture.h>

AGZ_D3D11_BEGIN

inline ComPtr<ID3D11Texture2D> CaptureScreenToD3DTexture(
    ID3D11DeviceContext *deviceContext, ID3D11Texture2D *source);

inline ComPtr<ID3D11Texture2D> CaptureScreenToD3DTexture(
    ID3D11DeviceContext *deviceContext, ID3D11Texture2D *source)
{
    D3D11_TEXTURE2D_DESC desc;
    source->GetDesc(&desc);

    ComPtr<ID3D11Device> device;
    deviceContext->GetDevice(device.GetAddressOf());

    // msaa case

    if(desc.SampleDesc.Count > 1)
    {
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        ComPtr<ID3D11Texture2D> IT;
        HRESULT hr = device->CreateTexture2D(&desc, nullptr, IT.GetAddressOf());
        if(FAILED(hr))
        {
            throw VRPGBaseException(
                "failed to create temp texture2d for resolving msaa texture");
        }

        for(UINT i = 0; i < desc.ArraySize; ++i)
        {
            for(UINT j = 0; j < desc.MipLevels; ++j)
            {
                const UINT index = D3D11CalcSubresource(j, i, desc.MipLevels);
                deviceContext->ResolveSubresource(
                    IT.Get(), index, source, index, desc.Format);
            }
        }

        desc.BindFlags      = 0;
        desc.MiscFlags     &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage          = D3D11_USAGE_STAGING;

        ComPtr<ID3D11Texture2D> ret;
        hr = device->CreateTexture2D(&desc, nullptr, ret.GetAddressOf());
        if(FAILED(hr))
            throw VRPGBaseException("failed to create captured texture");

        deviceContext->CopyResource(ret.Get(), IT.Get());

        return ret;
    }

    // directly usable case

    if(desc.Usage == D3D11_USAGE_STAGING &&
       (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
        return source;

    // normal case

    desc.BindFlags      = 0;
    desc.MiscFlags     &= D3D11_RESOURCE_MISC_TEXTURECUBE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage          = D3D11_USAGE_STAGING;

    ComPtr<ID3D11Texture2D> ret;
    const HRESULT hr = device->CreateTexture2D(&desc, nullptr, ret.GetAddressOf());
    if(FAILED(hr))
        throw VRPGBaseException("failed to create captured texture");

    deviceContext->CopyResource(ret.Get(), source);

    return ret;
}

inline texture::texture2d_t<math::color3b> CaptureScreenToTexture(
    ID3D11DeviceContext *deviceContext, ID3D11Texture2D *source)
{
    auto d3d11Texture = CaptureScreenToD3DTexture(deviceContext, source);

}

AGZ_D3D11_END
