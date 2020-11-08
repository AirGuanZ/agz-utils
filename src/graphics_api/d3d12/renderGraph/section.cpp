#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph/section.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

void Section::setDevice(ID3D12Device *device)
{
    device_ = device;
}

void Section::setQueue(int queue)
{
    queue_ = queue;
}

void Section::setFrameCount(int frameCount)
{
    perFrameCmdList_.resize(frameCount);
}

void Section::setCommandListType(D3D12_COMMAND_LIST_TYPE cmdListType)
{
    cmdListType_ = cmdListType;
}

void Section::addPass(Pass pass)
{
    passes_.push_back(std::move(pass));
}

void Section::addWaitFence(ComPtr<ID3D12Fence> fence)
{
    waitFences_.push_back(std::move(fence));
}

void Section::setSignalFence(ComPtr<ID3D12Fence> fence)
{
    signalFence_ = std::move(fence);
}

void Section::addOutput(Section *out)
{
    out_.push_back(out);
}

void Section::setDependencies(int externalDependencies)
{
    dependencies_ = externalDependencies + 1;
}

void Section::resetFinishedDependencies()
{
    unfinishedDependencies_ = dependencies_;
}

D3D12_COMMAND_LIST_TYPE Section::getCommandListType() const noexcept
{
    return cmdListType_;
}

void Section::execute(
    int                         frameIndex,
    UINT64                      fenceValue,
    ID3D12CommandAllocator     *cmdAlloc,
    ComPtr<ID3D12CommandQueue> *queues)
{
    auto queue = queues[queue_].Get();

    for(auto &f : waitFences_)
        queue->Wait(f.Get(), fenceValue);

    auto &cmdList = perFrameCmdList_[frameIndex];
    if(!cmdList)
    {
        device_->CreateCommandList(
            0, cmdListType_, cmdAlloc, nullptr,
            IID_PPV_ARGS(cmdList.GetAddressOf()));
    }
    else
        cmdList->Reset(cmdAlloc, nullptr);

    for(auto &p : passes_)
        p.execute(frameIndex, cmdList.Get());

    cmdList->Close();

    if(signalFence_)
        queue->Signal(signalFence_.Get(), fenceValue);

    increaseFinishedDependencies(frameIndex, queues);
}

void Section::increaseFinishedDependencies(
    int frameIndex, ComPtr<ID3D12CommandQueue> *queues)
{
    if(!--unfinishedDependencies_)
    {
        ID3D12CommandList *rawCmdList = perFrameCmdList_[frameIndex].Get();
        queues[queue_]->ExecuteCommandLists(1, &rawCmdList);

        for(auto o : out_)
            o->increaseFinishedDependencies(frameIndex, queues);
    }
}

AGZ_D3D12_RENDERGRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
