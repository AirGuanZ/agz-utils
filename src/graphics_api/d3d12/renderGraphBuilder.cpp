#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraphBuilder.h>

AGZ_D3D12_BEGIN

namespace rg
{

InternalResource::InternalResource(std::string name, int index)
    : name_(std::move(name)), index_(index),
      heapType_(D3D12_HEAP_TYPE_DEFAULT), desc_{},
      clear_(false), clearValue_{},
      initialState_(D3D12_RESOURCE_STATE_COMMON)
{
    
}

void InternalResource::setDescription(const D3D12_RESOURCE_DESC &desc)
{
    desc_ = desc;
}

void InternalResource::setClearValue(const D3D12_CLEAR_VALUE &clearValue)
{
    clear_      = true;
    clearValue_ = clearValue;
}

void InternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void InternalResource::setHeapType(D3D12_HEAP_TYPE heapType)
{
    heapType_ = heapType;
}

int InternalResource::getIndex() const noexcept
{
    return index_;
}

ExternalResource::ExternalResource(std::string name, int index)
    : name_(std::move(name)), index_(index),
      initialState_(D3D12_RESOURCE_STATE_COMMON),
      finalState_  (D3D12_RESOURCE_STATE_COMMON)
{
    
}

void ExternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void ExternalResource::setFinalState(D3D12_RESOURCE_STATES state)
{
    finalState_ = state;
}

int ExternalResource::getIndex() const noexcept
{
    return index_;
}

Vertex::Vertex(std::string name, int index)
    : name_(std::move(name)), index_(index),
      threadIndex_(0),
      queueIndex_(0)
{
    
}

void Vertex::useResource(ExternalResource *rsc, D3D12_RESOURCE_STATES state)
{
    rscUsages_.push_back({ rsc, state });
}

void Vertex::useResource(InternalResource *rsc, D3D12_RESOURCE_STATES state)
{
    rscUsages_.push_back({ rsc, state });
}

void Vertex::setQueue(int index)
{
    queueIndex_ = index;
}

void Vertex::setThread(int index)
{
    threadIndex_ = index;
}

void Vertex::setCallback(Callback callback)
{
    callback_ = std::make_unique<Callback>(std::move(callback));
}

RenderGraphBuilder::RenderGraphBuilder()
    : threadCount_(0), queueCount_(0), frameCount_(0)
{
    
}

void RenderGraphBuilder::setThreadCount(int count)
{
    threadCount_ = count;
}

void RenderGraphBuilder::setQueueCount(int count)
{
    queueCount_ = count;
}

void RenderGraphBuilder::setFrameCount(int count)
{
    frameCount_ = count;
}

InternalResource *RenderGraphBuilder::addInternalResource(std::string name)
{
    const int index = static_cast<int>(rscs_.size());
    rscs_.push_back(
        std::make_unique<Resource>(InternalResource(std::move(name), index)));
    return &rscs_.back()->as<InternalResource>();
}

ExternalResource *RenderGraphBuilder::addExternalResource(std::string name)
{
    const int index = static_cast<int>(rscs_.size());
    rscs_.push_back(
        std::make_unique<Resource>(ExternalResource(std::move(name), index)));
    return &rscs_.back()->as<ExternalResource>();
}

Vertex *RenderGraphBuilder::addVertex(
    std::string name,
    int         threadIndex,
    int         queueIndex)
{
    const int index = static_cast<int>(vtxs_.size());
    vtxs_.push_back(std::make_unique<Vertex>(std::move(name), index));

    auto ret = vtxs_.back().get();
    ret->setThread(threadIndex);
    ret->setQueue(queueIndex);

    return ret;
}

void RenderGraphBuilder::addArc(Vertex *head, Vertex *tail)
{
    head->out_.insert(tail);
    tail->in_ .insert(head);
}

std::unique_ptr<RenderGraph> RenderGraphBuilder::build(
    ID3D12Device                                     *device,
    std::vector<ComPtr<ID3D12CommandQueue>>           cmdQueues,
    ResourceManager                                  &rscMgr) const
{
    // build mapping from vertex to command list type

    std::vector<D3D12_COMMAND_QUEUE_DESC> cmdQueueDescs(cmdQueues.size());
    for(size_t i = 0; i < cmdQueues.size(); ++i)
        cmdQueueDescs[i] = cmdQueues[i]->GetDesc();

    auto vtx2CmdListType = [&](const Vertex *vtx)
    {
        return cmdQueueDescs[vtx->queueIndex_].Type;
    };

    // check thread/queue indices

    for(auto &v : vtxs_)
    {
        if(v->queueIndex_ >= queueCount_)
        {
            throw D3D12Exception(
                "'" + v->name_ + "'.queueIndex exceeds graph limits. "
                "max: " + std::to_string(queueCount_ - 1) + "; "
                "actial: " + std::to_string(v->queueIndex_));
        }

        if(v->threadIndex_ >= threadCount_)
        {
            throw D3D12Exception(
                "'" + v->name_ + "'.threadIndex exceeds graph limits. "
                "max: " + std::to_string(threadCount_ - 1) + "; "
                "actial: " + std::to_string(v->threadIndex_));
        }
    }

    // sort all passes

    std::vector<PassRecord> passRecords(vtxs_.size());
    const auto linearPasses = topologySortPasses();

    // assign passes to threads

    std::vector<ThreadRecord> threadRecords(threadCount_);
    for(int i : linearPasses)
    {
        const int threadIndex = vtxs_[i]->threadIndex_;
        threadRecords[threadIndex].passes.push_back(i);
    }

    // collect all users of each resource

    std::vector<ResourceRecord> resource2Usages(rscs_.size());
    for(int passIdx : linearPasses)
    {
        const Vertex *vtx = vtxs_[passIdx].get();
        for(auto &r : vtx->rscUsages_)
        {
            const int rscIdx = match_variant(
                r.rsc,
                [&](const InternalResource *rsc) { return rsc->index_; },
                [&](const ExternalResource *rsc) { return rsc->index_; });

            resource2Usages[rscIdx].usages.push_back(
                ResourceRecord::Usage{
                    .pass        = passIdx,
                    .state       = r.state,
                    .cmdListType = vtx2CmdListType(vtx)
                });
        }
    }

    // generate resource state barriers

    for(size_t rscIdx = 0; rscIdx < rscs_.size(); ++rscIdx)
    {
        const auto &usages = resource2Usages[rscIdx].usages;
        const auto &rsc    = *rscs_[rscIdx];

        if(usages.empty())
            continue;

        const D3D12_RESOURCE_STATES initialState = match_variant(
            rsc,
            [&](const InternalResource &r) { return r.initialState_; },
            [&](const ExternalResource &r) { return r.initialState_; });

        const D3D12_RESOURCE_STATES finalState = match_variant(
            rsc,
            [&](const InternalResource &r) { return r.initialState_; },
            [&](const ExternalResource &r) { return r.finalState_; });

        D3D12_RESOURCE_STATES lastState = initialState;
        for(size_t i = 0; i < usages.size(); ++i)
        {
            auto &usage = usages[i];

            const D3D12_RESOURCE_STATES beg = lastState;
            const D3D12_RESOURCE_STATES mid = usage.state;
            const D3D12_RESOURCE_STATES end =
                (i == usages.size() - 1) ? finalState : mid;

            if(beg != mid || mid != end)
            {
                passRecords[usage.pass].transitions.push_back({
                       static_cast<int>(rscIdx), beg, mid, end });
            }

            // TODO: UAV barrier

            lastState = end;
        }
    }

    // generate fences & submission flags

    std::vector<ComPtr<ID3D12Fence>> allFences;
    int totalSectionCount = 0;

    for(auto &vtx : vtxs_)
    {
        ComPtr<ID3D12Fence> fence;

        for(auto out : vtx->out_)
        {
            if(vtx->queueIndex_ != out->queueIndex_)
            {
                if(!fence)
                {
                    AGZ_D3D12_CHECK_HR_MSG(
                        "failed to create fence object for render graph",
                        device->CreateFence(
                            0, D3D12_FENCE_FLAG_NONE,
                            IID_PPV_ARGS(fence.GetAddressOf())));

                    passRecords[vtx->index_].signalFence = fence;
                    allFences.push_back(fence);
                }

                passRecords[out->index_].waitFences.push_back(fence);
            }

            if(vtx->queueIndex_  != out->queueIndex_ ||
               vtx->threadIndex_ != out->threadIndex_)
            {
                passRecords[vtx->index_].mustSubmit = true;
                ++totalSectionCount;
            }
        }
    }

    for(auto &tr : threadRecords)
    {
        if(!tr.passes.empty())
        {
            passRecords[tr.passes.back()].mustSubmit = true;
            ++totalSectionCount;
        }
    }

    // generate sections

    std::vector<SectionRecord> sectionRecords;
    bool needNewSection = true;

    for(auto &tp : threadRecords)
    {
        for(int passIdx : tp.passes)
        {
            auto &pass = passRecords[passIdx];

            if(needNewSection)
            {
                auto &threadRecord = threadRecords[vtxs_[passIdx]->threadIndex_];

                const int sectionIndex =
                    static_cast<int>(sectionRecords.size());

                const int sectionIndexInThread =
                    static_cast<int>(threadRecord.sections.size());

                sectionRecords.push_back(
                    SectionRecord{
                        .passes        = {},
                        .threadIndex   = vtxs_[passIdx]->threadIndex_,
                        .queueIndex    = vtxs_[passIdx]->queueIndex_,
                        .indexInThread = sectionIndexInThread
                    });

                threadRecord.sections.push_back(sectionIndex);

                needNewSection = false;
            }

            pass.sectionIndex = static_cast<int>(sectionRecords.size() - 1);
            sectionRecords.back().passes.push_back(passIdx);

            if(pass.mustSubmit)
                needNewSection = true;
        }
    }

    // generate render graph

    auto renderGraph = std::make_unique<RenderGraph>();

    // allocate and fill per-thread data

    renderGraph->perThreadData_.resize(threadCount_);
    for(int threadIdx = 0; threadIdx < threadCount_; ++threadIdx)
    {
        auto &threadData   = renderGraph->perThreadData_[threadIdx];
        auto &threadRecord = threadRecords[threadIdx];

        threadData.sectionCount = threadRecord.sections.size();
        threadData.sections =
            std::make_unique<RenderGraph::SectionRecord[]>(
                threadData.sectionCount);
    }

    for(int threadIdx = 0; threadIdx < threadCount_; ++threadIdx)
    {
        auto &threadData   = renderGraph->perThreadData_[threadIdx];
        auto &threadRecord = threadRecords[threadIdx];

        // sections

        bool hasGraphicsSection = false;
        bool hasComputeSection  = false;

        for(size_t secIdxInThread = 0;
            secIdxInThread < threadRecord.sections.size();
            ++secIdxInThread)
        {
            const int secIdx    = threadRecord.sections[secIdxInThread];
            auto &section       = threadData.sections[secIdxInThread];
            auto &sectionRecord = sectionRecords[secIdx];

            // queue index

            section.queueIdx = sectionRecord.queueIndex;

            // self dependency

            ++section.targetDependencyCount;

            // output sections

            std::set<RenderGraph::SectionRecord*> outputs;
            for(int passIdx : sectionRecord.passes)
            {
                for(auto outVtx : vtxs_[passIdx]->out_)
                {
                    auto &outPass      = passRecords[outVtx->index_];
                    auto &outSecRecord = sectionRecords[outPass.sectionIndex];

                    auto &outSec =
                        renderGraph->perThreadData_[outSecRecord.threadIndex]
                                    .sections[outSecRecord.indexInThread];

                    if(&outSec != &section)
                        outputs.insert(&outSec);
                }
            }

            for(auto o : outputs)
                ++o->targetDependencyCount;

            std::copy(
                outputs.begin(),
                outputs.end(),
                std::back_inserter(section.outputs));

            // need graphics/compute command allocator?

            const D3D12_COMMAND_LIST_TYPE cmdListType =
                vtx2CmdListType(vtxs_[sectionRecord.passes.front()].get());

            if(cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT)
                hasGraphicsSection = true;
            else if(cmdListType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
                hasComputeSection = true;
            else
            {
                throw D3D12Exception(
                    "unsupported command list type in render graph");
            }

            // wait/signal fences

            for(int passIdx : sectionRecord.passes)
            {
                auto &pass = passRecords[passIdx];

                std::copy(
                    pass.waitFences.begin(),
                    pass.waitFences.end(),
                    std::back_inserter(section.waitFences));

                if(pass.signalFence)
                {
                    // a submission always exists with a signal fence
                    // so any section has no more than one signal fence
                    assert(!section.signalFence);
                    section.signalFence = pass.signalFence;
                }
            }

            // passes

            for(int passIdx : sectionRecord.passes)
            {
                Pass pass;
                pass.setCallback(vtxs_[passIdx]->callback_);

                for(auto &t : passRecords[passIdx].transitions)
                    pass.addResourceStateTransition(t.idx, t.beg, t.mid, t.end);

                section.section.addPass(std::move(pass));
            }
        }

        // command allocators

        threadData.cmdAllocPerFrame.resize(frameCount_);
        for(auto &cmdAlloc : threadData.cmdAllocPerFrame)
        {
            if(hasGraphicsSection)
            {
                AGZ_D3D12_CHECK_HR_MSG(
                    "failed to create command allocator for render graph",
                    device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(cmdAlloc.graphics.GetAddressOf())));
            }

            if(hasComputeSection)
            {
                AGZ_D3D12_CHECK_HR_MSG(
                    "failed to create command allocator for render graph",
                    device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_COMPUTE,
                        IID_PPV_ARGS(cmdAlloc.compute.GetAddressOf())));
            }
        }
    }

    // create per-frame command list

    for(int threadIdx = 0; threadIdx < threadCount_; ++threadIdx)
    {
        auto &threadData   = renderGraph->perThreadData_[threadIdx];
        auto &threadRecord = threadRecords[threadIdx];

        for(size_t secIdxInThread = 0;
            secIdxInThread < threadRecord.sections.size();
            ++secIdxInThread)
        {
            const int secIdx    = threadRecord.sections[secIdxInThread];
            auto &section       = threadData.sections[secIdxInThread];
            auto &sectionRecord = sectionRecords[secIdx];

            section.cmdListPerFrame.resize(frameCount_);
            for(auto &cmdList : section.cmdListPerFrame)
            {
                const D3D12_COMMAND_LIST_TYPE cmdListType =
                    vtx2CmdListType(vtxs_[sectionRecord.passes.front()].get());

                auto cmdAlloc =
                    cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT ?
                    threadData.cmdAllocPerFrame[0].graphics.Get() :
                    threadData.cmdAllocPerFrame[0].compute.Get();

                AGZ_D3D12_CHECK_HR_MSG(
                    "failed to create command list for render graph",
                    device->CreateCommandList(
                        0, cmdListType, cmdAlloc, nullptr,
                        IID_PPV_ARGS(cmdList.GetAddressOf())));

                cmdList->Close();
            }
        }
    }

    // resources

    renderGraph->rawRscs_.resize(rscs_.size());

    for(size_t rscIdx = 0; rscIdx < rscs_.size(); ++rscIdx)
    {
        match_variant(
            *rscs_[rscIdx],
            [&](const InternalResource &r)
        {
            UniqueResource rsc;
            if(r.clear_)
            {
                rsc = rscMgr.create(
                    r.heapType_, r.desc_,
                    r.initialState_, r.clearValue_);
            }
            else
            {
                rsc = rscMgr.create(
                    r.heapType_, r.desc_,
                    r.initialState_);
            }

            renderGraph->rawRscs_[rscIdx] = rsc->resource.Get();

            renderGraph->rscs_.emplace_back(
                RenderGraph::InternalResource{
                    .idx = static_cast<int>(rscIdx),
                    .rsc = std::move(rsc)
                });

            renderGraph->name2Rsc_.insert(
                { r.name_, static_cast<int>(rscIdx) });
        },
            [&](const ExternalResource &r)
        {
            renderGraph->rscs_.emplace_back(
                RenderGraph::ExternalResource{
                    .idx = static_cast<int>(rscIdx),
                    .rsc = {}
                });

            renderGraph->name2Rsc_.insert(
                { r.name_, static_cast<int>(rscIdx) });
        });
    }

    // command queues

    renderGraph->cmdQueues_ = std::move(cmdQueues);

    // sync

    renderGraph->sync_ = std::make_unique<RenderGraph::Sync>();

    // threads

    for(int i = 0; i < threadCount_; ++i)
    {
        renderGraph->threads_.emplace_back(
            &RenderGraph::workingThread, renderGraph.get(), i);
    }

    return renderGraph;
}

std::vector<int> RenderGraphBuilder::topologySortPasses() const
{
    // vertex index -> number of prevs
    std::vector<size_t> inCount(vtxs_.size());

    // indices of unprocessed vertices with no prevs
    std::queue<size_t> Q;

    for(size_t i = 0; i < vtxs_.size(); ++i)
    {
        const size_t c = vtxs_[i]->in_.size();
        inCount[i] = c;
        if(!c)
            Q.push(i);
    }

    std::vector<int> result;
    result.reserve(vtxs_.size());

    while(!Q.empty())
    {
        const int vIdx = static_cast<int>(Q.front());
        Q.pop();
        result.push_back(vIdx);

        for(auto out : vtxs_[vIdx]->out_)
        {
            if(!--inCount[out->index_])
                Q.push(out->index_);
        }
    }

    if(result.size() != vtxs_.size())
    {
        throw D3D12Exception(
            "failed to find a topological order in render graph");
    }

    return result;
}

} // namespace rg

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
