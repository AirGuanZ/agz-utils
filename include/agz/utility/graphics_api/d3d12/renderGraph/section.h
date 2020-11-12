#pragma once

#include <agz/utility/graphics_api/d3d12/renderGraph/pass.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

class Section : public misc::uncopyable_t
{
public:

    void setDevice(ID3D12Device *device);

    void setQueue(int queue);

    void setFrameCount(int frameCount);

    void setCommandListType(D3D12_COMMAND_LIST_TYPE cmdListType);

    void addPass(Pass pass);

    void addWaitFence(ComPtr<ID3D12Fence> fence);

    void setSignalFence(ComPtr<ID3D12Fence> fence);

    void addOutput(Section *out);

    void setDependencies(int externalDependencies);

    void resetFinishedDependencies();

    D3D12_COMMAND_LIST_TYPE getCommandListType() const noexcept;

    void execute(
        int                         frameIndex,
        UINT64                      fenceValue,
        ID3D12CommandAllocator     *cmdAlloc,
        ComPtr<ID3D12CommandQueue> *queues,
        ID3D12DescriptorHeap      **GPUHeaps,
        int                         heapCount);

private:

    void increaseFinishedDependencies(
        int frameIndex, ComPtr<ID3D12CommandQueue> *queues);

    ID3D12Device *device_ = nullptr;
    int           queue_  = 0;

    D3D12_COMMAND_LIST_TYPE cmdListType_ = D3D12_COMMAND_LIST_TYPE_DIRECT;
    std::vector<ComPtr<ID3D12GraphicsCommandList>> perFrameCmdList_;

    int                    dependencies_           = 1;
    std::atomic<int>       unfinishedDependencies_ = 0;
    std::vector<Section *> out_;

    std::vector<Pass> passes_;

    std::vector<ComPtr<ID3D12Fence>> waitFences_;
    ComPtr<ID3D12Fence>              signalFence_;
};

AGZ_D3D12_RENDERGRAPH_END
