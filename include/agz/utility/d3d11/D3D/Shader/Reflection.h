#pragma once

#include <cassert>
#include <map>
#include <string>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

namespace Reflection
{

    inline ComPtr<ID3D11ShaderReflection> GetShaderReflection(ID3D10Blob *byteCode)
    {
        assert(byteCode);
        ComPtr<ID3D11ShaderReflection> reflection;
        HRESULT hr = D3DReflect(
            byteCode->GetBufferPointer(), byteCode->GetBufferSize(),
            IID_ID3D11ShaderReflection, (void **)reflection.GetAddressOf());
        return SUCCEEDED(hr) ? reflection : nullptr;
    }

    struct ConstantBufferInfo
    {
        UINT slot;
    };

    struct ShaderResourceInfo
    {
        UINT slot;
        UINT count;
    };

    struct ShaderSamplerInfo
    {
        UINT slot;
    };

    inline void GetShaderInfo(
        ID3D11ShaderReflection *reflection,
        std::map<std::string, ConstantBufferInfo> &constantBufferInfo,
        std::map<std::string, ShaderResourceInfo> &shaderResourceInfo,
        std::map<std::string, ShaderSamplerInfo> &shaderSamplerInfo)
    {
        assert(reflection);

        D3D11_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        for(UINT rsc = 0; rsc < shaderDesc.BoundResources; ++rsc)
        {
            D3D11_SHADER_INPUT_BIND_DESC bdDesc;
            reflection->GetResourceBindingDesc(rsc, &bdDesc);
            if(bdDesc.Type == D3D_SIT_CBUFFER)
            {
                constantBufferInfo.insert(std::make_pair(
                    std::string(bdDesc.Name), ConstantBufferInfo{ bdDesc.BindPoint }));
            }
            else if(bdDesc.Type == D3D_SIT_TEXTURE    ||
                    bdDesc.Type == D3D_SIT_STRUCTURED ||
                    bdDesc.Type == D3D_SIT_BYTEADDRESS)
            {
                shaderResourceInfo.insert(std::make_pair(
                    std::string(bdDesc.Name), ShaderResourceInfo{ bdDesc.BindPoint, bdDesc.BindCount }));
            }
            else if(bdDesc.Type == D3D_SIT_SAMPLER)
            {
                shaderSamplerInfo.insert(std::make_pair(
                    std::string(bdDesc.Name), ShaderSamplerInfo{ bdDesc.BindPoint }));
            }
        }
    }

} // namespace Reflection

AGZ_D3D11_END
