#pragma once

#include <agz-utils/graphics_api/d3d12/queueWaiter.h>
#include <agz-utils/graphics_api/d3d12/immediateCommandList.h>
#include <agz-utils/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_BEGIN

class Device : public misc::uncopyable_t
{
public:

    Device(bool graphicsQueue, bool computeQueue);

    ~Device();

    void waitForIdle();

    IDXGIFactory *getFactory() noexcept { return factory_.Get(); }

    IDXGIAdapter *getAdapter() noexcept { return adapter_.Get(); }

    ID3D12Device *getDevice() noexcept { return device_.Get(); }

    operator ID3D12Device *() noexcept { return device_.Get(); }

    ID3D12CommandQueue *getGraphicsQueue() noexcept { return graphicsQueue_.Get(); }

    ID3D12CommandQueue *getComputeQueue()  noexcept { return computeQueue_.Get(); }

    ResourceManager createResourceManager() const { return ResourceManager(device_.Get(), adapter_.Get()); }

    ComPtr<ID3D12CommandQueue> createCopyQueue() const;

    ImmediateCommandList createImmediateCommandList(
        D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

private:

    ComPtr<IDXGIFactory> factory_;
    ComPtr<IDXGIAdapter> adapter_;
    ComPtr<ID3D12Device> device_;

    ComPtr<ID3D12CommandQueue> graphicsQueue_;
    ComPtr<ID3D12CommandQueue> computeQueue_;

    std::unique_ptr<CommandQueueWaiter> queueWaiter_;
};

AGZ_D3D12_END
