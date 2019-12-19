#pragma once

#include <agz/utility/d3d11/D3D/ComObjectHolder.h>
#include <agz/utility/d3d11/D3D/D3DCreate.h>

AGZ_D3D11_BEGIN

namespace Impl
{

    template<typename Index>
    struct IndexToDXGIFormat;

    template<>
    struct IndexToDXGIFormat<std::uint16_t>
    {
        static constexpr DXGI_FORMAT value = DXGI_FORMAT_R16_UINT;
    };

    template<>
    struct IndexToDXGIFormat<std::uint32_t>
    {
        static constexpr DXGI_FORMAT value = DXGI_FORMAT_R32_UINT;
    };

} // namespace Impl

template<typename Index>
class IndexBuffer : public ComObjectHolder<ID3D11Buffer>
{
    UINT indexCount_ = 0;

public:

    void Initialize(UINT indexCount, bool dynamic, const Index *initData)
    {
        assert(indexCount);

        D3D11_USAGE usage;
        UINT cpuFlag;
        if(dynamic)
        {
            usage = D3D11_USAGE_DEFAULT;
            cpuFlag = 0;
        }
        else
        {
            if(!initData)
            {
                throw VRPGBaseException("initData is nullptr for immutable index buffer");
            }
            usage = D3D11_USAGE_IMMUTABLE;
            cpuFlag = 0;
        }

        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage               = usage;
        bufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.ByteWidth           = UINT(sizeof(Index) * indexCount);
        bufferDesc.CPUAccessFlags      = cpuFlag;
        bufferDesc.MiscFlags           = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subrscData;
        subrscData.pSysMem          = initData;
        subrscData.SysMemPitch      = 0;
        subrscData.SysMemSlicePitch = 0;

        obj_ = D3D::CreateD3D11Buffer(bufferDesc, initData ? &subrscData : nullptr);
        if(!obj_)
        {
            throw VRPGBaseException("failed to create index buffer");
        }
        indexCount_ = indexCount;
    }

    void Destroy()
    {
        obj_.Reset();
        indexCount_ = 0;
    }

    void SetValue(const Index *data)
    {
        gDeviceContext->UpdateSubresource(obj_.Get(), 0, nullptr, data, 0, 0);
    }

    UINT GetIndexCount() const noexcept
    {
        return indexCount_;
    }

    void Bind() const
    {
        constexpr DXGI_FORMAT format = Impl::IndexToDXGIFormat<Index>::value;
        gDeviceContext->IASetIndexBuffer(obj_.Get(), format, 0);
    }

    void Unbind() const
    {
        gDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }
};

AGZ_D3D11_END
