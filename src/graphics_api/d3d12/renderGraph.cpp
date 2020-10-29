#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph.h>

AGZ_D3D12_BEGIN

namespace rg
{

RenderGraph::RenderGraph(RenderGraph &&other) noexcept
    : RenderGraph()
{
    swap(other);
}

RenderGraph &RenderGraph::operator=(RenderGraph &&other) noexcept
{
    swap(other);
    return *this;
}

RenderGraph::~RenderGraph()
{
    threadTrigger_ = -10;
    sync_->cond.notify_all();
    for(auto &t : threads_)
        t.join();
}

void RenderGraph::swap(RenderGraph &other) noexcept
{
    std::swap(sync_,          other.sync_);
    std::swap(frameIndex_,    other.frameIndex_);
    std::swap(threadTrigger_, other.threadTrigger_);
    std::swap(threads_,       other.threads_);
    std::swap(perThreadData_, other.perThreadData_);
    std::swap(rscs_,          other.rscs_);
    std::swap(rawRscs_,       other.rawRscs_);
    std::swap(cmdQueues_,     other.cmdQueues_);
}

void RenderGraph::runAsync(int frameIndex)
{
    // in each frame:
    // launch_thread.set_latch_count   | 
    // launch_thread.set_trigger       | 
    // launch_thread.awake_thread_cond | 
    //                                 | worker_thread.init_frame_work
    //                                 | worker_thread.submit
    //                                 | worker_thread.count_down_latch
    // launch_thread.wait_launch       |

    ++newFenceValue_;

    for(auto &d : perThreadData_)
    {
        for(size_t i = 0; i < d.sectionCount; ++i)
        {
            auto &s = d.sections[i];
            s.restDependencyCount = s.targetDependencyCount;
        }
    }

    sync_->latch.set_counter(static_cast<int>(threads_.size()));
    {
        std::lock_guard lk(sync_->mutex);
        frameIndex_ = frameIndex;
        ++threadTrigger_;
    }
    sync_->cond.notify_all();
}

void RenderGraph::sync()
{
    sync_->latch.wait();
}

void RenderGraph::setExternalResource(int index, ComPtr<ID3D12Resource> rsc)
{
    assert(index < static_cast<int>(rscs_.size()));
    match_variant(
        rscs_[index],
        [&](const InternalResource &r)
    {
        throw D3D12Exception(
            "resource " + std::to_string(index) + " is not an external resource");
    },
        [&](ExternalResource &r)
    {
        r.rsc = std::move(rsc);
        rawRscs_[index] = r.rsc.Get();
    });
}

void RenderGraph::setExternalResource(
    std::string_view name, ComPtr<ID3D12Resource> rsc)
{
    const auto it = name2Rsc_.find(name);
    if(it == name2Rsc_.end())
        throw D3D12Exception("unknown external resource: " + std::string(name));
    setExternalResource(it->second, std::move(rsc));
}

void RenderGraph::clearExternalResources()
{
    for(auto &rsc : rscs_)
    {
        auto r = rsc.as_if<ExternalResource>();
        if(!r)
            continue;

        r->rsc.Reset();
        rawRscs_[r->idx] = nullptr;
    }
}

void RenderGraph::increaseNumOfFinishedDependency(SectionRecord *section)
{
    if(--section->restDependencyCount > 0)
        return;

    ID3D12CommandQueue *cmdQueue = cmdQueues_[section->queueIdx].Get();
    for(auto &f : section->waitFences)
        cmdQueue->Wait(f.Get(), newFenceValue_);
    
    ID3D12CommandList *cmdList = section->cmdListPerFrame[frameIndex_].Get();
    cmdQueue->ExecuteCommandLists(1, &cmdList);

    if(section->signalFence)
        cmdQueue->Signal(section->signalFence.Get(), newFenceValue_);

    for(auto o : section->outputs)
        increaseNumOfFinishedDependency(o);
}

void RenderGraph::workingFunc(int threadIndex)
{
    const int frameIndex = frameIndex_;

    auto &threadData = perThreadData_[threadIndex];
    auto &cmdAlloc   = threadData.cmdAllocPerFrame[frameIndex];

    if(cmdAlloc.graphics)
        cmdAlloc.graphics->Reset();
    if(cmdAlloc.compute)
        cmdAlloc.compute->Reset();

    for(size_t i = 0; i < threadData.sectionCount; ++i)
    {
        auto &s       = threadData.sections[i];
        auto *cmdList = s.cmdListPerFrame[frameIndex].Get();

        if(cmdList->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            assert(cmdAlloc.graphics);
            cmdList->Reset(cmdAlloc.graphics.Get(), nullptr);
        }
        else
        {
            assert(cmdList->GetType() == D3D12_COMMAND_LIST_TYPE_COMPUTE);
            assert(cmdAlloc.compute);
            cmdList->Reset(cmdAlloc.compute.Get(), nullptr);
        }

        s.section.execute(cmdList, rawRscs_.data());

        cmdList->Close();
        increaseNumOfFinishedDependency(&s);
    }
}

void RenderGraph::workingThread(int threadIndex)
{
    int trigger = -1;
    for(;;)
    {
        int newTrigger;
        {
            std::unique_lock lk(sync_->mutex);
            sync_->cond.wait(
                lk, [=] { return trigger != threadTrigger_; });
            newTrigger = threadTrigger_;
        }

        assert(trigger != newTrigger);
        trigger = newTrigger;
        if(trigger < 0)
            break;

        workingFunc(threadIndex);

        sync_->latch.count_down();
    }
}

} // namespace rg

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
