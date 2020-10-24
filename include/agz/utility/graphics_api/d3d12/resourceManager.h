#pragma once

#include <D3D12MemAlloc.h>

#include <agz/utility/graphics_api/d3d12/common.h>
#include <agz/utility/misc.h>

AGZ_D3D12_BEGIN

class ResourceManager : public misc::uncopyable_t
{
public:

    struct Resource
    {
        ComPtr<ID3D12Resource> resource;
        D3D12MA::Allocation   *allocation = nullptr;
    };

    struct ResourceDeleter
    {
        void operator()(Resource &rsc) const
        {
            rsc.resource.Reset();
            rsc.allocation->Release();
        }
    };

    using UniqueResource = misc::unique_resource_t<Resource, ResourceDeleter>;

    ResourceManager(ID3D12Device *device, IDXGIAdapter *adapter);

    ~ResourceManager();

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState);

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState,
        const D3D12_CLEAR_VALUE   &clearValue);

private:
    
    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState,
        const D3D12_CLEAR_VALUE   *clearValue);

    D3D12MA::Allocator *allocator_;
};

AGZ_D3D12_END
