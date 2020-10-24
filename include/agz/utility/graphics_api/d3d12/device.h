#pragma once

#include <agz/utility/graphics_api/d3d12/queueWaiter.h>

AGZ_D3D12_BEGIN

class Device : public misc::uncopyable_t
{
public:

    Device(bool graphicsQueue, bool computeQueue);

    ~Device();

    void waitForIdle();

    IDXGIFactory *getFactory() noexcept { return factory_.Get(); }

    ID3D12Device *getDevice() noexcept { return device_.Get(); }

    operator ID3D12Device *() noexcept { return device_.Get(); }

    ID3D12CommandQueue *getGraphicsQueue() noexcept { return graphicsQueue_.Get(); }

    ID3D12CommandQueue *getComputeQueue()  noexcept { return computeQueue_.Get(); }

private:

    ComPtr<IDXGIFactory> factory_;
    ComPtr<IDXGIAdapter> adapter_;
    ComPtr<ID3D12Device> device_;

    ComPtr<ID3D12CommandQueue> graphicsQueue_;
    ComPtr<ID3D12CommandQueue> computeQueue_;

    std::unique_ptr<CommandQueueWaiter> queueWaiter_;
};

AGZ_D3D12_END
