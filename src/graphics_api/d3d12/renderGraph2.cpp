#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph2.h>

AGZ_D3D12_BEGIN

PassContext::PassContext(
    int                          frameIndex,
    ID3D12GraphicsCommandList   *cmdList,
    const ResourceRecord        *resources)
    : frameIndex_(frameIndex),
      cmdList_(cmdList),
      resources_(resources)
{
    
}

int PassContext::getFrameIndex() const noexcept
{
    return frameIndex_;
}

const ID3D12Resource *PassContext::getResource(int resourceIndex) const noexcept
{
    return resources_[resourceIndex].resource;
}

const ID3D12Resource *PassContext::getResource(Resource *resource) const noexcept
{
    return getResource(resource->getIndex());
}

Descriptor PassContext::getDescriptor(int resourceIndex) const noexcept
{
    return resources_[resourceIndex].descriptor;
}

Descriptor PassContext::getDescriptor(Resource *resource) const noexcept
{
    return getDescriptor(resource->getIndex());
}

ID3D12GraphicsCommandList *PassContext::getCommandList() noexcept
{
    return cmdList_;
}

ID3D12GraphicsCommandList *PassContext::operator->() noexcept
{
    return cmdList_;
}

Resource::Resource(RenderGraph *graph, std::string name, int index)
    : graph_(graph), name_(std::move(name)), index_(index)
{
    
}

int Resource::getIndex() const noexcept
{
    return index_;
}

const std::string &Resource::getName() const noexcept
{
    return name_;
}

RenderGraph *Resource::getGraph() const noexcept
{
    return graph_;
}

ID3D12Resource *Resource::getRawResource() const noexcept
{
    return graph_->getRawResource(getIndex());
}

const InternalResource *Resource::asInternal() const noexcept
{
    return nullptr;
}

InternalResource *Resource::asInternal() noexcept
{
    return nullptr;
}

const class ExternalResource *Resource::asExternal() const noexcept
{
    return nullptr;
}

ExternalResource *Resource::asExternal() noexcept
{
    return nullptr;
}

InternalResource::InternalResource(RenderGraph *graph, std::string name, int index)
    : Resource(graph, std::move(name), index),
      heapType_(D3D12_HEAP_TYPE_DEFAULT),
      desc_{}, clear_(false), clearValue_{},
      initialState_(D3D12_RESOURCE_STATE_COMMON)
{

}

void InternalResource::setResourceDescription(const D3D12_RESOURCE_DESC &desc)
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

const InternalResource *InternalResource::asInternal() const noexcept
{
    return this;
}

InternalResource *InternalResource::asInternal() noexcept
{
    return this;
}

ExternalResource::ExternalResource(
    RenderGraph *graph, std::string name, int index)
    : Resource(graph, std::move(name), index),
      initialState_(D3D12_RESOURCE_STATE_COMMON),
      finalState_(D3D12_RESOURCE_STATE_COMMON)
{
    
}

void ExternalResource::setResource(ComPtr<ID3D12Resource> rsc)
{
    getGraph()->setExternalResource(this, std::move(rsc));
}

void ExternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void ExternalResource::setFinalState(D3D12_RESOURCE_STATES state)
{
    finalState_ = state;
}

const class ExternalResource *ExternalResource::asExternal() const noexcept
{
    return this;
}

ExternalResource *ExternalResource::asExternal() noexcept
{
    return this;
}

void Vertex::setQueue(int queueIndex)
{
    queue_ = queueIndex;
}

Vertex::Vertex(std::string name, int index, int thread, int queue)
    : name_(std::move(name)), index_(index),
      thread_(thread), queue_(queue)
{
    
}

const std::string &Vertex::getName() const noexcept
{
    return name_;
}

int Vertex::getIndex() const noexcept
{
    return index_;
}

void Vertex::setThread(int threadIndex)
{
    thread_ = threadIndex;
}

void Vertex::setCallback(PassCallback callback)
{
    callback_ = std::make_shared<PassCallback>(std::move(callback));
}

void Vertex::useResource(Resource *rsc, D3D12_RESOURCE_STATES state)
{
    resourceUsages_.push_back({ rsc, state, {} });
}

void Vertex::useShaderResource(
    Resource                              *rsc,
    D3D12_RESOURCE_STATES                  state,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &SRVDesc)
{
    resourceUsages_.push_back({ rsc, state, SRVDesc });
}

void Vertex::useUnorderedAccess(
    Resource                               *rsc,
    D3D12_RESOURCE_STATES                   state,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &UAVDesc)
{
    resourceUsages_.push_back({ rsc, state, UAVDesc });
}

void Vertex::useRenderTarget(
    Resource                            *rsc,
    D3D12_RESOURCE_STATES                state,
    const D3D12_RENDER_TARGET_VIEW_DESC &RTVDesc)
{
    resourceUsages_.push_back({ rsc, state, RTVDesc });
}

void Vertex::useDepthStencil(
    Resource                            *rsc,
    D3D12_RESOURCE_STATES                state,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &DSVDesc)
{
    resourceUsages_.push_back({ rsc, state, DSVDesc });
}

Pass::Pass(
    RenderGraph                  *graph,
    std::shared_ptr<PassCallback> callback,
    std::vector<ResourceUsage>    usages)
    : graph_(graph),
      callback_(std::move(callback)),
      resourceUsages_(std::move(usages))
{
    
}

Pass::Pass(Pass &&other) noexcept
    : graph_(nullptr)
{
    swap(other);
}

Pass &Pass::operator=(Pass &&other) noexcept
{
    swap(other);
    return *this;
}

void Pass::swap(Pass &other) noexcept
{
    std::swap(graph_,           other.graph_);
    std::swap(callback_,        other.callback_);
    std::swap(resourceUsages_,  other.resourceUsages_);
    std::swap(resourceRecords_, other.resourceRecords_);
    std::swap(beforeBarriers_,  other.beforeBarriers_);
    std::swap(afterBarriers_,   other.afterBarriers_);
}

void Pass::execute(
    ID3D12Device                    *device,
    TransientDescriptorAllocatorSet &descAlloc,
    int                              frameIndex,
    ID3D12GraphicsCommandList       *cmdList)
{
    // records

    resourceRecords_.clear();
    resourceRecords_.resize(graph_->getResourceCount());

    for(auto &usage : resourceUsages_)
    {
        auto &record = resourceRecords_[usage.resource->getIndex()];
        record.resource = usage.resource->getRawResource();

        match_variant(
            usage.view,
            [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &SRVDesc)
        {
            record.descriptor = descAlloc.CBV_SRV_UAV.alloc();
            device->CreateShaderResourceView(
                record.resource, &SRVDesc, record.descriptor);
        },
            [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &UAVDesc)
        {
            record.descriptor = descAlloc.CBV_SRV_UAV.alloc();
            device->CreateUnorderedAccessView(
                record.resource, nullptr, &UAVDesc, record.descriptor);
        },
            [&](const D3D12_RENDER_TARGET_VIEW_DESC &RTVDesc)
        {
            record.descriptor = descAlloc.RTV.alloc();
            device->CreateRenderTargetView(
                record.resource, &RTVDesc, record.descriptor);
        },
            [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &DSVDesc)
        {
            record.descriptor = descAlloc.DSV.alloc();
            device->CreateDepthStencilView(
                record.resource, &DSVDesc, record.descriptor);
        },
            [&](const auto &) { });
    }

    // barriers

    assert(beforeBarriers_.empty() && afterBarriers_.empty());

    for(auto &usage : resourceUsages_)
    {
        if(usage.beg != usage.mid)
        {
            beforeBarriers_.push_back(
                CD3DX12_RESOURCE_BARRIER::Transition(
                    usage.resource->getRawResource(),
                    usage.beg,
                    usage.mid));
        }

        if(usage.mid != usage.end)
        {
            afterBarriers_.push_back(
                CD3DX12_RESOURCE_BARRIER::Transition(
                    usage.resource->getRawResource(),
                    usage.mid,
                    usage.end));
        }
    }

    AGZ_SCOPE_GUARD({
        beforeBarriers_.clear();
        afterBarriers_.clear();
    });

    // execute

    if(!beforeBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(beforeBarriers_.size()), beforeBarriers_.data());
    }

    if(callback_)
    {
        PassContext passContext(frameIndex, cmdList, resourceRecords_.data());
        (*callback_)(passContext);
    }

    if(!afterBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(afterBarriers_.size()), afterBarriers_.data());
    }
}

void Section::setQueue(int queue)
{
    queue_ = queue;
}

void Section::initializeCommandList(
    ID3D12Device           *device,
    int                     frameCount,
    ID3D12CommandAllocator *cmdAlloc,
    D3D12_COMMAND_LIST_TYPE cmdListType)
{
    cmdListType_ = cmdListType;
    frame2CmdList_.resize(frameCount);
    for(auto &c : frame2CmdList_)
    {
        AGZ_D3D12_CHECK_HR(
            device->CreateCommandList(
                0, cmdListType, cmdAlloc, nullptr,
                IID_PPV_ARGS(c.GetAddressOf())));
        c->Close();
    }
}

void Section::addPass(Pass pass)
{
    passes_.push_back(std::move(pass));
}

void Section::addOut(Section *out)
{
    out_.push_back(out);
}

void Section::addWaitFence(ComPtr<ID3D12Fence> fence)
{
    waitFences_.push_back(std::move(fence));
}

void Section::addSignalFence(ComPtr<ID3D12Fence> fence)
{
    signalFences_.push_back(std::move(fence));
}

void Section::execute(
    ID3D12Device                    *device,
    UINT64                           fenceValue,
    TransientDescriptorAllocatorSet &descAlloc,
    int                              frameIndex,
    ID3D12CommandAllocator          *cmdAlloc,
    ComPtr<ID3D12CommandQueue>      *queues)
{
    auto &queue = queues[queue_];
    for(auto &f : waitFences_)
        queue->Wait(f.Get(), fenceValue);

    auto cmdList = frame2CmdList_[frameIndex].Get();
    cmdList->Reset(cmdAlloc, nullptr);

    for(auto &p : passes_)
        p.execute(device, descAlloc, frameIndex, cmdList);

    cmdList->Close();

    for(auto &f : signalFences_)
        queue->Signal(f.Get(), fenceValue);

    increaseNumberOfFinishedDependencies(frameIndex, queues);
}

void Section::increaseNumebrOfAllDependencies()
{
    ++allDependencies_;
}

void Section::resetNumberOfUnfinishedDependencies()
{
    unfinishedDependencies_ = allDependencies_;
}

void Section::increaseNumberOfFinishedDependencies(
    int                         frameIndex,
    ComPtr<ID3D12CommandQueue> *queues)
{
    auto queue = queues[queue_].Get();

    if(!--unfinishedDependencies_)
    {
        ID3D12CommandList *rawCmdList = frame2CmdList_[frameIndex].Get();
        queue->ExecuteCommandLists(1, &rawCmdList);

        for(auto out : out_)
            out->increaseNumberOfFinishedDependencies(frameIndex, queues);
    }
}

D3D12_COMMAND_LIST_TYPE Section::getCommandListType() const noexcept
{
    return cmdListType_;
}

RenderGraph::RenderGraph(
    ID3D12Device           *device,
    ResourceManager        &rscMgr,
    DescriptorAllocatorSet &descAlloc)
    : device_(device), rscMgr_(rscMgr), descAlloc_(descAlloc),
      threadCount_(0), queueCount_(0), frameCount_(0),
      frameIndex_(0), threadTrigger_(-1), fenceValue_(0)
{

}

RenderGraph::~RenderGraph()
{
    threadTrigger_ = -10;
    cond_.notify_all();
    for(auto &t : threads_)
        t.join();
}

void RenderGraph::setThreadCount(int count)
{
    assert(count > 0);
    threadCount_ = count;
}

void RenderGraph::setQueueCount(int count)
{
    assert(count > 0);
    queueCount_ = count;
    queues_.resize(count);
}

void RenderGraph::setFrameCount(int count)
{
    assert(count > 0);
    frameCount_ = count;
}

void RenderGraph::setQueue(int index, ComPtr<ID3D12CommandQueue> queue)
{
    queues_[index] = std::move(queue);
}

InternalResource *RenderGraph::addInternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());

    auto rsc = std::make_unique<InternalResource>(this, std::move(name), index);
    auto ret = rsc.get();

    resources_.push_back(std::move(rsc));
    return ret;
}

ExternalResource *RenderGraph::addExternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());

    auto rsc = std::make_unique<ExternalResource>(this, std::move(name), index);
    auto ret = rsc.get();

    resources_.push_back(std::move(rsc));
    return ret;
}

Vertex *RenderGraph::addVertex(std::string name, int thread, int queue)
{
    const int index = static_cast<int>(vertices_.size());

    auto vtx = std::make_unique<Vertex>(std::move(name), index, thread, queue);
    auto ret = vtx.get();

    vertices_.push_back(std::move(vtx));
    return ret;
}

void RenderGraph::addArc(Vertex *head, Vertex *tail)
{
    head->out_.insert(tail);
    tail->in_.insert(head);
}

int RenderGraph::getResourceCount() const noexcept
{
    return static_cast<int>(resources_.size());
}

int RenderGraph::getVertexCount() const noexcept
{
    return static_cast<int>(vertices_.size());
}

void RenderGraph::compile()
{
    // clear

    threadTrigger_ = -10;
    cond_.notify_all();
    for(auto &t : threads_)
        t.join();
    threads_.clear();

    frameIndex_    = 0;
    threadTrigger_ = -1;
    fenceValue_    = 0;

    perThread_.clear();
    runtimeResources_.clear();

    checkThreadAndQueueIndices();

    // assign passes to threads

    const std::vector<int> linearPasses = sortPasses();

    std::vector<ThreadTemp> threadTemps(threadCount_);
    for(int i : linearPasses)
    {
        const int thread = vertices_[i]->thread_;
        threadTemps[thread].passes.push_back(i);
    }

    // collect resource usages

    auto resourceTemps = collectResourceUsages(linearPasses);

    // generate resource state barriers & views

    std::vector<PassTemp> passTemps;
    passTemps.resize(vertices_.size());
    generateResourceUsagesForPasses(resourceTemps, passTemps);

    // generate fences & submission flags

    generateFencesAndSubmissionFlags(
        threadTemps, passTemps);

    // generate section temps

    auto sectionTemps = generateSectionTemps(passTemps, threadTemps);

    // create per-thread data

    perThread_.resize(threadCount_);
    for(int i = 0; i < threadCount_; ++i)
    {
        auto &threadData = perThread_[i];
        auto &threadTemp = threadTemps[i];

        threadData.sectionCount = threadTemp.sections.size();
        threadData.sections =
            std::make_unique<Section[]>(threadData.sectionCount);
    }

    // fill per-thread data

    const auto vtx2queueType = getQueueTypeMapping();

    for(int threadIdx = 0; threadIdx < threadCount_; ++threadIdx)
    {
        auto &threadData = perThread_[threadIdx];
        auto &threadTemp = threadTemps[threadIdx];

        // sections

        bool hasGraphicsSection = false;
        bool hasComputeSection  = false;

        for(size_t secIdxInThread = 0;
            secIdxInThread < threadTemp.sections.size();
            ++secIdxInThread)
        {
            const int secIdx  = threadTemp.sections[secIdxInThread];
            auto &section     = threadData.sections[secIdxInThread];
            auto &sectionTemp = sectionTemps[secIdx];

            section.setQueue(sectionTemp.queue);

            section.increaseNumebrOfAllDependencies();

            // output sections

            std::set<Section*> outputs;
            for(int passIdx : sectionTemp.passes)
            {
                for(auto outVtx : vertices_[passIdx]->out_)
                {
                    auto &outPass    = passTemps[outVtx->index_];
                    auto &outSecTemp = sectionTemps[outPass.sectionIndex];

                    auto &outSec = perThread_[outSecTemp.thread]
                                  .sections[outSecTemp.indexInThread];

                    if(&outSec != &section)
                        outputs.insert(&outSec);
                }
            }

            for(auto o : outputs)
            {
                o->increaseNumebrOfAllDependencies();
                section.addOut(o);
            }

            // need graphics/compute command allocator?

            const D3D12_COMMAND_LIST_TYPE cmdListType =
                vtx2queueType(vertices_[sectionTemp.passes.front()].get());

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

            for(int passIdx : sectionTemp.passes)
            {
                auto &pass = passTemps[passIdx];

                for(auto &f : pass.waitFences)
                    section.addWaitFence(f);

                if(pass.signalFence)
                    section.addSignalFence(pass.signalFence);
            }

            // passes

            for(int passIdx : sectionTemp.passes)
            {
                section.addPass(Pass(
                    this,
                    vertices_[passIdx]->callback_,
                    passTemps[passIdx].usages));
            }
        }

        // command allocators

        threadData.frame2CmdAlloc.resize(frameCount_);
        for(auto &cmdAlloc : threadData.frame2CmdAlloc)
        {
            if(hasGraphicsSection)
            {
                AGZ_D3D12_CHECK_HR_MSG(
                    "failed to create command allocator for render graph",
                    device_->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(cmdAlloc.graphics.GetAddressOf())));
            }

            if(hasComputeSection)
            {
                AGZ_D3D12_CHECK_HR_MSG(
                    "failed to create command allocator for render graph",
                    device_->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_COMPUTE,
                        IID_PPV_ARGS(cmdAlloc.compute.GetAddressOf())));
            }
        }
    }

    // per-frame command list

    for(int threadIdx = 0; threadIdx < threadCount_; ++threadIdx)
    {
        auto &threadData = perThread_[threadIdx];
        auto &threadTemp = threadTemps[threadIdx];

        for(size_t secIdxInThread = 0;
            secIdxInThread < threadTemp.sections.size();
            ++secIdxInThread)
        {
            const int secIdx  = threadTemp.sections[secIdxInThread];
            auto &section     = threadData.sections[secIdxInThread];
            auto &sectionTemp = sectionTemps[secIdx];

            const D3D12_COMMAND_LIST_TYPE cmdListType =
                vtx2queueType(vertices_[sectionTemp.passes.front()].get());

            auto cmdAlloc =
                cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT ?
                threadData.frame2CmdAlloc[0].graphics.Get() :
                threadData.frame2CmdAlloc[0].compute.Get();

            section.initializeCommandList(
                device_, frameCount_, cmdAlloc, cmdListType);
        }
    }

    // resources

    runtimeResources_.resize(resources_.size());
    for(size_t rscIdx = 0; rscIdx < resources_.size(); ++rscIdx)
    {
        if(auto internal = resources_[rscIdx]->asInternal())
        {
            UniqueResource rsc;
            if(internal->clear_)
            {
                rsc = rscMgr_.create(
                    internal->heapType_, internal->desc_,
                    internal->initialState_, internal->clearValue_);
            }
            else
            {
                rsc = rscMgr_.create(
                    internal->heapType_, internal->desc_,
                    internal->initialState_);
            }

            runtimeResources_[rscIdx] = RuntimeInternalResource{
                .index    = static_cast<int>(rscIdx),
                .resource = std::move(rsc)
            };
        }
        else
        {
            auto external = resources_[rscIdx]->asExternal();
            assert(external);

            runtimeResources_[rscIdx] = RuntimeExternalResource{
                .index    = static_cast<int>(rscIdx),
                .resource = {}
            };
        }
    }

    // threads

    for(int i = 0; i < threadCount_; ++i)
        threads_.emplace_back(&RenderGraph::workingThread, this, i);
}

void RenderGraph::setExternalResource(
    ExternalResource *rscNode, ComPtr<ID3D12Resource> rsc)
{
    auto &r = runtimeResources_[rscNode->getIndex()];
    r.as<RuntimeExternalResource>().resource = std::move(rsc);
}

void RenderGraph::clearExternalResources()
{
    for(auto &r : runtimeResources_)
    {
        if(auto external = r.as_if<RuntimeExternalResource>())
            external->resource.Reset();
    }
}

ID3D12Resource *RenderGraph::getRawResource(int index)
{
    auto &r = runtimeResources_[index];
    if(auto internal = r.as_if<RuntimeInternalResource>())
        return internal->resource->resource.Get();
    return r.as<RuntimeExternalResource>().resource.Get();
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

    ++fenceValue_;

    for(auto &d : perThread_)
    {
        for(size_t i = 0; i < d.sectionCount; ++i)
        {
            auto &s = d.sections[i];
            s.resetNumberOfUnfinishedDependencies();
        }
    }

    latch_.set_counter(static_cast<int>(threads_.size()));
    {
        std::lock_guard lk(mutex_);
        frameIndex_ = frameIndex;
        ++threadTrigger_;
    }
    cond_.notify_all();
}

void RenderGraph::sync()
{
    latch_.wait();
}

void RenderGraph::run(int frameIndex)
{
    runAsync(frameIndex);
    sync();
}

void RenderGraph::workingFunc(int threadIndex)
{
    TransientDescriptorAllocatorSet descAlloc;
    descAlloc.CBV_SRV_UAV = descAlloc_.CBV_SRV_UAV->createTransientAllocator(32);
    descAlloc.Sampler     = descAlloc_.Sampler->createTransientAllocator(16);
    descAlloc.RTV         = descAlloc_.RTV->createTransientAllocator(8);
    descAlloc.DSV         = descAlloc_.DSV->createTransientAllocator(8);

    const int frameIndex = frameIndex_;

    auto &threadData = perThread_[threadIndex];
    auto &cmdAlloc   = threadData.frame2CmdAlloc[frameIndex];

    if(cmdAlloc.graphics)
        cmdAlloc.graphics->Reset();
    if(cmdAlloc.compute)
        cmdAlloc.compute->Reset();

    for(size_t i = 0; i < threadData.sectionCount; ++i)
    {
        auto &s          = threadData.sections[i];
        auto cmdListType = s.getCommandListType();

        if(cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            assert(cmdAlloc.graphics);
            s.execute(
                device_, fenceValue_, descAlloc,
                frameIndex, cmdAlloc.graphics.Get(), queues_.data());
        }
        else
        {
            assert(cmdAlloc.compute);
            s.execute(
                device_, fenceValue_, descAlloc,
                frameIndex, cmdAlloc.compute.Get(), queues_.data());
        }
    }
}

void RenderGraph::workingThread(int threadIndex)
{
    int trigger = -1;
    for(;;)
    {
        int newTrigger;
        {
            std::unique_lock lk(mutex_);
            cond_.wait(
                lk, [=] { return trigger != threadTrigger_; });
            newTrigger = threadTrigger_;
        }

        assert(trigger != newTrigger);
        trigger = newTrigger;
        if(trigger < 0)
            break;

        workingFunc(threadIndex);

        latch_.count_down();
    }
}

std::function<D3D12_COMMAND_LIST_TYPE(Vertex *)>
    RenderGraph::getQueueTypeMapping() const
{
    std::vector<D3D12_COMMAND_LIST_TYPE> types(queues_.size());
    for(size_t i = 0; i < queues_.size(); ++i)
        types[i] = queues_[i]->GetDesc().Type;

    return [types = std::move(types)](Vertex *vtx)
    {
        return types[vtx->queue_];
    };
}

std::vector<int> RenderGraph::sortPasses() const
{
    // vertex index -> number of prevs
    std::vector<int> inCount(vertices_.size());

    // indices of unprocessed vertices with no prevs
    std::queue<int> Q;

    for(size_t i = 0; i < vertices_.size(); ++i)
    {
        const int c = static_cast<int>(vertices_[i]->in_.size());
        inCount[i] = c;
        if(!c)
            Q.push(static_cast<int>(i));
    }

    std::vector<int> result;
    result.reserve(vertices_.size());

    while(!Q.empty())
    {
        const int vIdx = Q.front();
        Q.pop();
        result.push_back(vIdx);

        for(auto out : vertices_[vIdx]->out_)
        {
            if(!--inCount[out->index_])
                Q.push(out->index_);
        }
    }

    if(result.size() != vertices_.size())
    {
        throw D3D12Exception(
            "failed to find a topological order in render graph");
    }

    return result;
}

void RenderGraph::checkThreadAndQueueIndices() const
{
    for(auto &v : vertices_)
    {
        if(v->queue_ >= queueCount_)
        {
            throw D3D12Exception(
                "'" + v->name_ + "'.queueIndex exceeds graph limits. "
                "max: " + std::to_string(queueCount_ - 1) + "; "
                "actial: " + std::to_string(v->queue_));
        }

        if(v->thread_ >= threadCount_)
        {
            throw D3D12Exception(
                "'" + v->name_ + "'.threadIndex exceeds graph limits. "
                "max: " + std::to_string(threadCount_ - 1) + "; "
                "actial: " + std::to_string(v->thread_));
        }
    }
}

std::vector<RenderGraph::ResourceTemp> RenderGraph::collectResourceUsages(
    const std::vector<int> &linearPasses) const
{
    std::vector<ResourceTemp> ret(resources_.size());

    for(int passIdx : linearPasses)
    {
        const Vertex *vtx = vertices_[passIdx].get();
        for(auto &r : vtx->resourceUsages_)
        {
            const int rscIdx = r.resource->getIndex();

            ret[rscIdx].usages.push_back(ResourceTemp::Usage{
                .pass  = passIdx,
                .state = r.state,
                .view  = r.view
            });
        }
    }

    return ret;
}

void RenderGraph::generateResourceUsagesForPasses(
    const std::vector<ResourceTemp> &resourceTemps,
    std::vector<PassTemp>           &passTemps) const
{
    for(size_t rscIdx = 0; rscIdx < resources_.size(); ++rscIdx)
    {
        const auto &usages = resourceTemps[rscIdx].usages;
        const auto &rsc    = *resources_[rscIdx];

        if(usages.empty())
            continue;

        const D3D12_RESOURCE_STATES initialState =
            rsc.asInternal() ? rsc.asInternal()->initialState_ :
                               rsc.asExternal()->initialState_;

        const D3D12_RESOURCE_STATES finalState =
            rsc.asInternal() ? rsc.asInternal()->initialState_ :
                               rsc.asExternal()->finalState_;

        D3D12_RESOURCE_STATES lastState = initialState;
        for(size_t i = 0; i < usages.size(); ++i)
        {
            auto &usage = usages[i];
            const D3D12_RESOURCE_STATES beg = lastState;
            const D3D12_RESOURCE_STATES mid = usage.state;
            const D3D12_RESOURCE_STATES end =
                (i == usages.size() - 1) ? finalState : mid;

            if(beg != mid || mid != end || !usage.view.is<std::monostate>())
            {
                passTemps[usage.pass].usages.push_back(Pass::ResourceUsage{
                    .resource = resources_[rscIdx].get(),
                    .beg      = beg,
                    .mid      = mid,
                    .end      = end,
                    .view     = usage.view
                });
            }

            // TODO: UAV barrier

            lastState = end;
        }
    }
}

void RenderGraph::generateFencesAndSubmissionFlags(
    const std::vector<ThreadTemp> &threadTemps,
    std::vector<PassTemp>         &passTemps) const
{
    for(auto &vtx : vertices_)
    {
        ComPtr<ID3D12Fence> fence;

        for(auto out : vtx->out_)
        {
            if(vtx->queue_ != out->queue_)
            {
                if(!fence)
                {
                    AGZ_D3D12_CHECK_HR(
                        device_->CreateFence(
                            0, D3D12_FENCE_FLAG_NONE,
                            IID_PPV_ARGS(fence.GetAddressOf())));

                    passTemps[vtx->index_].signalFence = fence;
                }
            }

            if(vtx->queue_ != out->queue_ || vtx->thread_ != out->thread_)
                passTemps[vtx->index_].mustSubmit = true;
        }
    }

    for(auto &tr : threadTemps)
    {
        if(!tr.passes.empty())
            passTemps[tr.passes.back()].mustSubmit = true;
    }
}

std::vector<RenderGraph::SectionTemp> RenderGraph::generateSectionTemps(
    std::vector<PassTemp>   &passTemps,
    std::vector<ThreadTemp> &threadTemps) const
{
    std::vector<SectionTemp> sectionTemps;

    bool needNewSection = true;

    for(auto &tp : threadTemps)
    {
        for(int passIdx : tp.passes)
        {
            auto &pass = passTemps[passIdx];

            if(needNewSection)
            {
                auto &threadTemp = threadTemps[vertices_[passIdx]->thread_];

                const int sectionIndex =
                    static_cast<int>(sectionTemps.size());
                const int sectionIndexInThread =
                    static_cast<int>(threadTemp.sections.size());

                sectionTemps.push_back(
                    SectionTemp{
                        .passes        = {},
                        .thread        = vertices_[passIdx]->thread_,
                        .queue         = vertices_[passIdx]->queue_,
                        .indexInThread = sectionIndexInThread
                    });

                threadTemp.sections.push_back(sectionIndex);

                needNewSection = false;
            }

            pass.sectionIndex = static_cast<int>(sectionTemps.size() - 1);
            sectionTemps.back().passes.push_back(passIdx);

            if(pass.mustSubmit)
                needNewSection = true;
        }
    }

    return sectionTemps;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
