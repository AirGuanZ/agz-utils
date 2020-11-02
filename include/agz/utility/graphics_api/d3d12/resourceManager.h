#pragma once

#include <D3D12MemAlloc.h>

#include <agz/utility/graphics_api/d3d12/buffer.h>
#include <agz/utility/graphics_api/d3d12/uniqueResource.h>
#include <agz/utility/misc.h>

AGZ_D3D12_BEGIN

class ResourceManager : public misc::uncopyable_t
{
public:

    ResourceManager();

    ResourceManager(ID3D12Device *device, IDXGIAdapter *adapter);

    ResourceManager(ResourceManager &&other) noexcept;

    ResourceManager &operator=(ResourceManager &&other) noexcept;

    ~ResourceManager();

    void swap(ResourceManager &other) noexcept;

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState);

    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState,
        const D3D12_CLEAR_VALUE   &clearValue);

    Buffer createDefaultBuffer(
        size_t                byteSize,
        D3D12_RESOURCE_STATES initState);

    Buffer createUploadBuffer(size_t byteSize);

private:
    
    UniqueResource create(
        D3D12_HEAP_TYPE            heapType,
        const D3D12_RESOURCE_DESC &desc,
        D3D12_RESOURCE_STATES      initialState,
        const D3D12_CLEAR_VALUE   *clearValue);

    D3D12MA::Allocator *allocator_;
};

AGZ_D3D12_END
