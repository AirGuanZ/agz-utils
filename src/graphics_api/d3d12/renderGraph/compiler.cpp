#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph/compiler.h>
#include <agz/utility/graphics_api/d3d12/renderGraph/viewComparer.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

Resource::Resource(std::string name, int index) noexcept
    : name_(std::move(name)), index_(index), desc_{}
{
    
}

const std::string &Resource::getName() const noexcept
{
    return name_;
}

int Resource::getIndex() const noexcept
{
    return index_;
}

bool Resource::isInternal() const noexcept
{
    return asInternal() != nullptr;
}

bool Resource::isExternal() const noexcept
{
    return asExternal() != nullptr;
}

ExternalResource *Resource::asExternal() noexcept
{
    return nullptr;
}

const ExternalResource *Resource::asExternal() const noexcept
{
    return nullptr;
}

InternalResource *Resource::asInternal() noexcept
{
    return nullptr;
}

const InternalResource *Resource::asInternal() const noexcept
{
    return nullptr;
}

void Resource::setDescription(const D3D12_RESOURCE_DESC &desc) noexcept
{
    desc_ = desc;
}

const D3D12_RESOURCE_DESC *Resource::getDescription() const noexcept
{
    return &desc_;
}

D3D12_RESOURCE_DESC *Resource::getDescription() noexcept
{
    return &desc_;
}

InternalResource::InternalResource(std::string name, int index) noexcept
    : Resource(std::move(name), index),
      heapType_(D3D12_HEAP_TYPE_DEFAULT),
      clear_(false), clearValue_{},
      initialState_(D3D12_RESOURCE_STATE_COMMON)
{
    
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

InternalResource *InternalResource::asInternal() noexcept
{
    return this;
}

const InternalResource *InternalResource::asInternal() const noexcept
{
    return this;
}

ExternalResource::ExternalResource(std::string name, int index) noexcept
    : Resource(std::move(name), index),
      initialState_(D3D12_RESOURCE_STATE_COMMON),
      finalState_(D3D12_RESOURCE_STATE_COMMON)
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

ExternalResource *ExternalResource::asExternal() noexcept
{
    return this;
}

const ExternalResource *ExternalResource::asExternal() const noexcept
{
    return this;
}

Vertex::Vertex(std::string name, int index, int thread, int queue)
    : name_(std::move(name)), index_(index), thread_(thread), queue_(queue)
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

void Vertex::setQueue(int queue)
{
    queue_ = queue;
}

void Vertex::setThread(int thread)
{
    thread_ = thread;
}

void Vertex::setCallback(PassCallback callback)
{
    callback_ = std::make_shared<PassCallback>(std::move(callback));
}

void Vertex::useResource(
    const Resource *resource, D3D12_RESOURCE_STATES state, ResourceView view)
{
    resourceUsages_.push_back({ resource, state, view });
}

Compiler::Compiler()
{
    threadCount_ = 0;
    queueCount_  = 0;
    frameCount_  = 0;
}

void Compiler::setThreadCount(int count)
{
    threadCount_ = count;
}

void Compiler::setQueueCount(int count)
{
    queueCount_ = count;
}

void Compiler::setFrameCount(int count)
{
    frameCount_ = count;
}

InternalResource *Compiler::addInternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());
    auto rsc = std::make_unique<InternalResource>(std::move(name), index);
    auto ret = rsc.get();
    resources_.push_back(std::move(rsc));
    return ret;
}

ExternalResource *Compiler::addExternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());
    auto rsc = std::make_unique<ExternalResource>(std::move(name), index);
    auto ret = rsc.get();
    resources_.push_back(std::move(rsc));
    return ret;
}

Vertex *Compiler::addVertex(std::string name, int thread, int queue)
{
    const int index = static_cast<int>(vertices_.size());
    vertices_.push_back(
        std::make_unique<Vertex>(std::move(name), index, thread, queue));
    return vertices_.back().get();
}

void Compiler::addArc(Vertex *head, Vertex *tail)
{
    head->out_.insert(tail);
    tail->in_.insert(head);
}

void Compiler::compile(
    ID3D12Device                           *device,
    ResourceManager                        &rscMgr,
    DescriptorAllocator                    &GPUDescAlloc,
    std::vector<ComPtr<ID3D12CommandQueue>> queues,
    Runtime                                &runtime)
{
    if(frameCount_ <= 0)
    {
        throw D3D12Exception(
            "invalid frame count value: " + std::to_string(frameCount_));
    }

    if(queueCount_ <= 0)
    {
        throw D3D12Exception(
            "invalid queue count value: " + std::to_string(queueCount_));
    }

    if(threadCount_ <= 0)
    {
        throw D3D12Exception(
            "invalid thread count value: " + std::to_string(threadCount_));
    }

    auto temps = assignSectionsToThreads();

    generateSectionDependencies(device, temps);
    generateResourceTransitions(temps);
    generateDescriptorRecords  (temps);

    runtime.reset();

    runtime.device_ = device;
    runtime.queues_ = std::move(queues);
    runtime.perThreadData_ =
        std::make_unique<Runtime::PerThreadData[]>(threadCount_);

    fillRuntimeResources        (runtime, rscMgr, temps);
    fillRuntimeDescriptors      (device, runtime, GPUDescAlloc, temps);
    fillRuntimeSections         (device, runtime, temps);
    fillRuntimeCommandAllocators(device, runtime);

    for(int ti = 0; ti < threadCount_; ++ti)
        runtime.threads_.emplace_back(&Runtime::threadEntry, &runtime, ti);
}

std::vector<int> Compiler::sortPasses() const
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
            "failed to find a topological order for render graph");
    }

    return result;
}

Compiler::Temps Compiler::assignSectionsToThreads() const
{
    Temps result;

    // apply topology sort on all passes

    std::vector<int> linearPasses = sortPasses();

    // assign passes to threads

    result.threads.resize(threadCount_);
    for(int i : linearPasses)
        result.threads[vertices_[i]->thread_].passes.push_back(i);

    // generate mustSubmit flag

    result.passes.resize(vertices_.size());

    for(auto &v : vertices_)
    {
        for(auto o : v->out_)
        {
            if(v->queue_ != o->queue_ || v->thread_ != o->thread_)
                result.passes[v->index_].shouldSubmit = true;
        }
    }

    for(auto &t : result.threads)
    {
        if(!t.passes.empty())
            result.passes[t.passes.back()].shouldSubmit = true;
    }

    // assign passes to sections

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &t = result.threads[ti];

        bool needNewSection      = true;
        int sectionIndexInThread = 0;

        for(int pi : t.passes)
        {
            if(needNewSection)
            {
                t.sections.push_back(static_cast<int>(result.sections.size()));
                result.sections.push_back(
                    { {}, ti, sectionIndexInThread++, 0, {}, {}, {} });
                needNewSection = false;
            }

            auto &p = result.passes[pi];
            p.section = t.sections.back();

            auto &s = result.sections[p.section];
            s.passes.push_back(pi);

            if(p.shouldSubmit)
                needNewSection = true;
        }
    }

    return result;
}

void Compiler::generateSectionDependencies(
    ID3D12Device *device, Temps &temps) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &t = temps.threads[ti];
        for(int si : t.sections)
        {
            auto &s = temps.sections[si];

            std::set<int> outputSections;
            for(int p : s.passes)
            {
                for(auto o : vertices_[p]->out_)
                    outputSections.insert(temps.passes[o->index_].section);
            }
            outputSections.erase(si);

            if(outputSections.empty())
                continue;

            const int thisQueue = vertices_[s.passes.front()]->queue_;
            bool needSignalFence = false;
            for(int osi : outputSections)
            {
                auto     &os     = temps.sections[osi];
                const int oQueue = vertices_[os.passes.front()]->queue_;
                if(oQueue != thisQueue)
                {
                    needSignalFence = true;
                    break;
                }
            }

            if(needSignalFence)
            {
                AGZ_D3D12_CHECK_HR(
                    device->CreateFence(
                        0, D3D12_FENCE_FLAG_NONE,
                        IID_PPV_ARGS(s.signalFence.GetAddressOf())));
            }

            for(int osi : outputSections)
            {
                auto     &os     = temps.sections[osi];
                const int oQueue = vertices_[os.passes.front()]->queue_;

                if(oQueue != thisQueue)
                    os.waitFences.push_back(s.signalFence);

                ++os.externalDependencies;
            }

            s.outputs = { outputSections.begin(), outputSections.end() };
        }
    }
}

void Compiler::generateResourceTransitions(Temps &temps)
{
    struct ResourceRecord
    {
        struct Usage
        {
            int                   pass;
            D3D12_RESOURCE_STATES state;
        };

        std::vector<Usage> usages;
    };

    std::vector<ResourceRecord> resourceRecords(resources_.size());

    for(auto &v : vertices_)
    {
        for(auto &r : v->resourceUsages_)
        {
            resourceRecords[r.resource->getIndex()].usages.push_back(
                { v->getIndex(), r.state });
        }
    }

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto &usages = resourceRecords[ri].usages;
        auto &rsc    = *resources_[ri];

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

            if(beg != mid || mid != end)
            {
                temps.passes[usage.pass].resourceTransitions.push_back(
                    Pass::ResourceTransition{
                        .resource = &rsc,
                        .beg      = beg,
                        .mid      = mid,
                        .end      = end
                    });
            }

            // TODO: UAV barrier

            lastState = end;
        }
    }
}

void Compiler::generateDescriptorRecords(Temps &temps)
{
    struct DescriptorKey
    {
        int          thread   = 0;
        int          resource = 0;
        ResourceView view     = {};

        bool operator<(const DescriptorKey &other) const noexcept
        {
            if(thread < other.thread)     return true;
            if(thread > other.thread)     return false;
            if(resource < other.resource) return true;
            if(resource > other.resource) return false;
            return ResourceViewComparer()(view, other.view);
        }
    };

    std::map<DescriptorKey, int> descriptorSlotCache;

    auto allocateDescriptorSlot = [&](const DescriptorKey &key)
    {
        const auto it = descriptorSlotCache.find(key);
        if(it != descriptorSlotCache.end())
            return it->second;

        const int newSlot = static_cast<int>(descriptorSlotCache.size());
        descriptorSlotCache.insert({ key, newSlot });

        match_variant(
            key.view,
            [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
        {
            temps.threads[key.thread].GPUDescRecords.push_back(
                { resources_[key.resource].get(), key.view, newSlot });
        },
            [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
        {
            temps.threads[key.thread].GPUDescRecords.push_back(
                { resources_[key.resource].get(), key.view, newSlot });
        },
            [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
        {
            temps.threads[key.thread].RTVRecords.push_back(
                { resources_[key.resource].get(), key.view, newSlot });
        },
            [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
        {
            temps.threads[key.thread].DSVRecords.push_back(
                { resources_[key.resource].get(), key.view, newSlot });
        },
            [&](const std::monostate &)
        {
            misc::unreachable();
        });

        return newSlot;
    };

    for(auto &vtx : vertices_)
    {
        auto &pass = temps.passes[vtx->index_];

        for(auto &usage : vtx->resourceUsages_)
        {
            if(usage.view.is<std::monostate>())
                continue;

            const int slot = allocateDescriptorSlot(
                { vtx->thread_, usage.resource->getIndex(), usage.view });

            pass.contextUsages.push_back(
                { usage.resource->getIndex(), slot });
        }
    }
}

void Compiler::fillRuntimeResources(
    Runtime         &runtime,
    ResourceManager &rscMgr,
    Temps           &temps) const
{
    runtime.resources_.resize(resources_.size());
    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto resource = resources_[ri].get();
        if(auto internal = resource->asInternal())
        {
            UniqueResource actualResource;
            if(internal->clear_)
            {
                actualResource = rscMgr.create(
                    internal->heapType_, *internal->getDescription(),
                    internal->initialState_, internal->clearValue_);
            }
            else
            {
                actualResource = rscMgr.create(
                    internal->heapType_, *internal->getDescription(),
                    internal->initialState_);
            }

            runtime.resources_[ri] = Runtime::RuntimeInternalResource{
                .index            = static_cast<int>(ri),
                .resource         = std::move(actualResource),
                .descriptorSlots  = {}
            };
        }
        else
        {
            runtime.resources_[ri] = Runtime::RuntimeExternalResource{
                .index            = static_cast<int>(ri),
                .resource         = {},
                .descriptorSlots  = {}
            };
        }
    }

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        static auto addDescSlot = [](Runtime::RuntimeResource &r, int newSlot)
        {
            match_variant(
                r,
                [&](Runtime::RuntimeInternalResource &ir)
            {
                ir.descriptorSlots.push_back(newSlot);
            },
                [&](Runtime::RuntimeExternalResource &er)
            {
                er.descriptorSlots.push_back(newSlot);
            });
        };

        auto &thread = temps.threads[ti];

        for(auto &dr : thread.GPUDescRecords)
        {
            addDescSlot(
                runtime.resources_[dr.resource->getIndex()], dr.desceiptorSlot);
        }

        for(auto &dr : thread.RTVRecords)
        {
            addDescSlot(
                runtime.resources_[dr.resource->getIndex()], dr.desceiptorSlot);
        }

        for(auto &dr : thread.DSVRecords)
        {
            addDescSlot(
                runtime.resources_[dr.resource->getIndex()], dr.desceiptorSlot);
        }
    }
}

void Compiler::fillRuntimeDescriptors(
    ID3D12Device        *device,
    Runtime             &runtime,
    DescriptorAllocator &GPUDescAlloc,
    Temps               &temps) const
{
    // count descs

    uint32_t GPUDescCount = 0, RTVCount = 0, DSVCount = 0;
    for(auto &t : temps.threads)
    {
        GPUDescCount += static_cast<uint32_t>(t.GPUDescRecords.size());
        RTVCount     += static_cast<uint32_t>(t.RTVRecords.size());
        DSVCount     += static_cast<uint32_t>(t.DSVRecords.size());
    }

    // allocate raw desc ranges

    runtime.GPUDescAlloc_ = &GPUDescAlloc;
    if(GPUDescCount)
    {
        runtime.GPUDescRange_ = GPUDescAlloc.allocStaticRange(
            GPUDescCount * frameCount_);
    }

    if(RTVCount)
    {
        runtime.RTVDescriptorHeap_.initialize(
            device, RTVCount * frameCount_,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
    }

    if(RTVCount)
    {
        runtime.DSVDescriptorHeap_.initialize(
            device, DSVCount * frameCount_,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);
    }

    // assign descs to threads

    uint32_t GPUDescOffset = 0;

    runtime.descriptorSlots_.resize(GPUDescCount + RTVCount + DSVCount);
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadTemp = temps.threads[ti];
        auto &threadData = runtime.perThreadData_[ti];

        // GPU desc

        if(!threadTemp.GPUDescRecords.empty())
        {
            threadData.GPUDescQueue = DescriptorQueue(
                runtime.GPUDescRange_.getSubRange(
                    GPUDescOffset,
                    static_cast<uint32_t>(
                        threadTemp.GPUDescRecords.size() * frameCount_)));

            GPUDescOffset += static_cast<uint32_t>(
                threadTemp.GPUDescRecords.size() * frameCount_);
        }

        for(auto &dr : threadTemp.GPUDescRecords)
        {
            runtime.descriptorSlots_[dr.desceiptorSlot] = DescriptorSlot{
                .resourceIndex = dr.resource->getIndex(),
                .isDirty       = true,
                .view          = dr.view,
                .descriptor    = {}
            };
            threadData.descriptorSlots.push_back(dr.desceiptorSlot);
        }

        // RTV

        if(!threadTemp.RTVRecords.empty())
        {
            threadData.RTVDescQueue = DescriptorQueue(
                runtime.RTVDescriptorHeap_.allocRange(
                    static_cast<uint32_t>(
                        threadTemp.RTVRecords.size() * frameCount_)));
        }

        for(auto &dr : threadTemp.RTVRecords)
        {
            runtime.descriptorSlots_[dr.desceiptorSlot] = DescriptorSlot{
                .resourceIndex = dr.resource->getIndex(),
                .isDirty       = true,
                .view          = dr.view,
                .descriptor    = {}
            };
            threadData.descriptorSlots.push_back(dr.desceiptorSlot);
        }

        // DSV

        if(!threadTemp.DSVRecords.empty())
        {
            threadData.DSVDescQueue = DescriptorQueue(
                runtime.DSVDescriptorHeap_.allocRange(
                    static_cast<uint32_t>(
                        threadTemp.DSVRecords.size() * frameCount_)));
        }

        for(auto &dr : threadTemp.DSVRecords)
        {
            runtime.descriptorSlots_[dr.desceiptorSlot] = DescriptorSlot{
                .resourceIndex = dr.resource->getIndex(),
                .isDirty       = true,
                .view          = dr.view,
                .descriptor    = {}
            };
            threadData.descriptorSlots.push_back(dr.desceiptorSlot);
        }
    }
}

void Compiler::fillRuntimeSections(
    ID3D12Device *device,
    Runtime      &runtime,
    Temps        &temps) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadTemp = temps.threads[ti];
        auto &threadData = runtime.perThreadData_[ti];

        threadData.sectionCount = threadTemp.sections.size();
        threadData.sections = std::make_unique<Section[]>(
            threadData.sectionCount);
    }

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadTemp = temps.threads[ti];
        auto &threadData = runtime.perThreadData_[ti];

        for(size_t secInThread = 0;
            secInThread < threadTemp.sections.size();
            ++secInThread)
        {
            auto &sectionTemp = temps.sections[threadTemp.sections[secInThread]];
            auto &section     = threadData.sections[secInThread];

            const int queue = vertices_[sectionTemp.passes.front()]->queue_;

            section.setDevice(device);
            section.setQueue(queue);
            section.setFrameCount(frameCount_);
            section.setCommandListType(runtime.queues_[queue]->GetDesc().Type);

            for(int pi : sectionTemp.passes)
            {
                auto &passTemp = temps.passes[pi];
                auto vtx       = vertices_[pi].get();

                std::map<int, PassContext::ResourceUsage> contextResources;
                for(auto &usage : passTemp.contextUsages)
                {
                    contextResources.insert({
                        usage.resource,
                        {
                            resources_[usage.resource].get(),
                            &runtime.descriptorSlots_[usage.descriptorSlot]
                        }
                    });
                }

                section.addPass(Pass(
                    &runtime,
                    vtx->callback_,
                    passTemp.resourceTransitions,
                    std::move(contextResources)));
            }

            for(auto &f : sectionTemp.waitFences)
                section.addWaitFence(f);

            section.setSignalFence(sectionTemp.signalFence);

            for(auto out : sectionTemp.outputs)
            {
                auto &outTemp = temps.sections[out];
                section.addOutput(
                    &runtime.perThreadData_[outTemp.thread]
                            .sections[outTemp.indexInThread]);
            }

            section.setDependencies(sectionTemp.externalDependencies);
        }
    }
}

void Compiler::fillRuntimeCommandAllocators(
    ID3D12Device *device,
    Runtime      &runtime) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadData = runtime.perThreadData_[ti];

        bool hasGraphics = false;
        bool hasCompute  = false;

        for(size_t si = 0; si < threadData.sectionCount; ++si)
        {
            const auto cmdListType =
                threadData.sections[si].getCommandListType();

            hasGraphics |= (cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT);
            hasCompute  |= (cmdListType == D3D12_COMMAND_LIST_TYPE_COMPUTE);
        }

        threadData.perFrameCmdAlloc.resize(frameCount_);

        for(auto &cmdAlloc : threadData.perFrameCmdAlloc)
        {
            if(hasGraphics)
            {
                AGZ_D3D12_CHECK_HR(
                    device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(cmdAlloc.graphics.GetAddressOf())));
            }

            if(hasCompute)
            {
                AGZ_D3D12_CHECK_HR(
                    device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_COMPUTE,
                        IID_PPV_ARGS(cmdAlloc.compute.GetAddressOf())));
            }
        }
    }
}

AGZ_D3D12_RENDERGRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
