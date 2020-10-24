#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_BEGIN

ResourceManager::ResourceManager(ID3D12Device *device, IDXGIAdapter *adapter)
    : allocator_(nullptr)
{
    D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
    allocatorDesc.pDevice  = device;
    allocatorDesc.pAdapter = adapter;

    AGZ_WHEN_DEBUG({
        allocatorDesc.Flags |= D3D12MA::ALLOCATOR_FLAG_ALWAYS_COMMITTED;
    });

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create d3d12 memory allocator",
        CreateAllocator(&allocatorDesc, &allocator_));
}

ResourceManager::~ResourceManager()
{
    assert(allocator_);
    allocator_->Release();
}

ResourceManager::UniqueResource ResourceManager::create(
    D3D12_HEAP_TYPE            heapType,
    const D3D12_RESOURCE_DESC &desc,
    D3D12_RESOURCE_STATES      initialState)
{
    return create(heapType, desc, initialState, nullptr);
}

ResourceManager::UniqueResource ResourceManager::create(
    D3D12_HEAP_TYPE            heapType,
    const D3D12_RESOURCE_DESC &desc,
    D3D12_RESOURCE_STATES      initialState,
    const D3D12_CLEAR_VALUE   &clearValue)
{
    return create(heapType, desc, initialState, &clearValue);
}

ResourceManager::UniqueResource ResourceManager::create(
    D3D12_HEAP_TYPE            heapType,
    const D3D12_RESOURCE_DESC &desc,
    D3D12_RESOURCE_STATES      initialState,
    const D3D12_CLEAR_VALUE   *clearValue)
{
    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = heapType;

    ComPtr<ID3D12Resource> rsc;
    D3D12MA::Allocation *allocation;

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create d3d12 resource",
        allocator_->CreateResource(
            &allocDesc, &desc, initialState, clearValue,
            &allocation, IID_PPV_ARGS(rsc.GetAddressOf())));

    return UniqueResource(
        { std::move(rsc), allocation }, ResourceDeleter{});
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
