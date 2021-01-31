#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/sectionRuntime.h>

AGZ_D3D12_GRAPH_BEGIN

void SectionRuntime::setDevice(ID3D12Device *device)
{
    device_ = device;
}

void SectionRuntime::setQueue(int queueIndex)
{
    queueIndex_ = queueIndex;
}

void SectionRuntime::setFrameCount(int frameCount)
{
    perFrameCmdList_.resize(frameCount);
}

void SectionRuntime::setCommandListType(D3D12_COMMAND_LIST_TYPE type)
{
    cmdListType_ = type;
}

D3D12_COMMAND_LIST_TYPE SectionRuntime::getCommandListType() const
{
    return cmdListType_;
}

void SectionRuntime::addPass(PassRuntime pass)
{
    passes_.push_back(std::move(pass));
}

void SectionRuntime::addWaitFence(ComPtr<ID3D12Fence> fence)
{
    waitFences_.push_back(std::move(fence));
}

void SectionRuntime::addWaitFenceOfLastFrame(ComPtr<ID3D12Fence> fence)
{
    waitFencesOfLastFrame_.push_back(std::move(fence));
}

void SectionRuntime::setSignalFence(ComPtr<ID3D12Fence> fence)
{
    signalFence_ = std::move(fence);
}

void SectionRuntime::addOutput(SectionRuntime *out)
{
    out_.push_back(out);
}

void SectionRuntime::setDependencies(int externalDependenciesCount)
{
    dependencies_ = externalDependenciesCount + 1;
}

void SectionRuntime::resetFinishedDependencies()
{
    unfinishedDependencies_ = dependencies_;
}

void SectionRuntime::execute(
    int                         frameIndex,
    UINT64                      fenceValue,
    ID3D12CommandAllocator     *cmdAlloc,
    ComPtr<ID3D12CommandQueue> *allQueues,
    ID3D12DescriptorHeap      **GPUHeaps,
    int                         GPUHeapCount)
{
    auto &cmdList = perFrameCmdList_[frameIndex];
    if(!cmdList)
    {
        device_->CreateCommandList(
            0, cmdListType_, cmdAlloc, nullptr,
            IID_PPV_ARGS(cmdList.GetAddressOf()));
    }
    else
        cmdList->Reset(cmdAlloc, nullptr);

    if(GPUHeapCount)
        cmdList->SetDescriptorHeaps(static_cast<UINT>(GPUHeapCount), GPUHeaps);

    for(auto &p : passes_)
        p.execute(frameIndex, cmdList.Get());

    cmdList->Close();

    increaseFinishedDependenciesCount(frameIndex, allQueues, fenceValue);
}

void SectionRuntime::increaseFinishedDependenciesCount(
    int                         frameIndex,
    ComPtr<ID3D12CommandQueue> *allQueues,
    UINT64                      fenceValue)
{
    if(!--unfinishedDependencies_)
    {
        auto queue = allQueues[queueIndex_];

        for(auto &f : waitFences_)
            queue->Wait(f.Get(), fenceValue);

        for(auto &f : waitFencesOfLastFrame_)
            queue->Wait(f.Get(), fenceValue - 1);

        ID3D12CommandList *rawCmdList = perFrameCmdList_[frameIndex].Get();
        allQueues[queueIndex_]->ExecuteCommandLists(1, &rawCmdList);

        if(signalFence_)
            queue->Signal(signalFence_.Get(), fenceValue);

        for(auto o : out_)
        {
            o->increaseFinishedDependenciesCount(
                frameIndex, allQueues, fenceValue);
        }
    }
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
