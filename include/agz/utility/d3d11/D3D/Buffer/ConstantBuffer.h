#pragma once

#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

template<typename Struct>
class ConstantBuffer
{
    ComPtr<ID3D11Buffer> buffer_;

public:

    void Initialize(bool dynamic, const Struct *initData)
    {
        D3D11_USAGE usage;
        UINT cpuAccessFlag;
        if(dynamic)
        {
            usage = D3D11_USAGE_DYNAMIC;
            cpuAccessFlag = D3D11_CPU_ACCESS_WRITE;
        }
        else
        {
            if(!initData)
            {
                throw VRPGBaseException("initData is nullptr for immutable constant buffer");
            }
            usage = D3D11_USAGE_IMMUTABLE;
            cpuAccessFlag = 0;
        }

        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = usage;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.ByteWidth = sizeof(Struct);
        bufferDesc.CPUAccessFlags = cpuAccessFlag;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subrscData;
        subrscData.pSysMem = initData;
        subrscData.SysMemPitch = 0;
        subrscData.SysMemSlicePitch = 0;

        buffer_ = CreateD3D11Buffer(bufferDesc, initData ? &subrscData : nullptr);
        if(!buffer_)
        {
            throw VRPGBaseException("failed to create constant buffer");
        }
    }

    bool IsAvailable() const noexcept
    {
        return buffer_ != nullptr;
    }

    void Destroy()
    {
        buffer_.Reset();
    }

    ID3D11Buffer *Get() const noexcept
    {
        return buffer_.Get();
    }

    ID3D11Buffer *const *GetAddressOf() const noexcept
    {
        return buffer_.GetAddressOf();
    }

    operator ID3D11Buffer*() const noexcept
    {
        return buffer_.Get();
    }

    void SetValue(const Struct &data)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubrsc;
        HRESULT hr = gDeviceContext->Map(buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubrsc);
        if(FAILED(hr))
        {
            throw VRPGBaseException("failed to map constant buffer to memory");
        }
        std::memcpy(mappedSubrsc.pData, &data, sizeof(Struct));
        gDeviceContext->Unmap(buffer_.Get(), 0);
    }
};

AGZ_D3D11_END
