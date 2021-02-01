#pragma once

#include <agz-utils/graphics_api/d3d12/graph/runtime/pass.h>

AGZ_D3D12_GRAPH_BEGIN

class SectionRuntime
{
public:

    SectionRuntime() = default;

    SectionRuntime(const SectionRuntime &) { misc::unreachable(); }

    void setDevice(ID3D12Device *device);

    void setQueue(int queueIndex);

    void setFrameCount(int frameCount);

    void setCommandListType(D3D12_COMMAND_LIST_TYPE type);

    D3D12_COMMAND_LIST_TYPE getCommandListType() const;

    void addPass(PassRuntime pass);

    void addWaitFence(ComPtr<ID3D12Fence> fence);

    void addWaitFenceOfLastFrame(ComPtr<ID3D12Fence> fence);

    void setSignalFence(ComPtr<ID3D12Fence> fence);

    void addOutput(SectionRuntime *out);

    void setDependencies(int externalDependenciesCount);

    void resetFinishedDependencies();

    void execute(
        int                         frameIndex,
        UINT64                      fenceValue,
        ID3D12CommandAllocator     *cmdAlloc,
        ComPtr<ID3D12CommandQueue> *allQueues,
        ID3D12DescriptorHeap      **GPUHeaps,
        int                         GPUHeapCount);

private:

    void increaseFinishedDependenciesCount(
        int                         frameIndex,
        ComPtr<ID3D12CommandQueue> *allQueues,
        UINT64                      fenceValue);

    ID3D12Device *device_     = nullptr;
    int           queueIndex_ = 0;

    D3D12_COMMAND_LIST_TYPE cmdListType_ = D3D12_COMMAND_LIST_TYPE_DIRECT;
    std::vector<ComPtr<RawGraphicsCommandList>> perFrameCmdList_;

    int                           dependencies_           = 1;
    std::atomic<int>              unfinishedDependencies_ = 0;
    std::vector<SectionRuntime *> out_;

    std::vector<PassRuntime> passes_;

    std::vector<ComPtr<ID3D12Fence>> waitFencesOfLastFrame_;
    std::vector<ComPtr<ID3D12Fence>> waitFences_;
    ComPtr<ID3D12Fence>              signalFence_;
};

AGZ_D3D12_GRAPH_END
