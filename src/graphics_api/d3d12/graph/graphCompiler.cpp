#ifdef AGZ_ENABLE_D3D12

#include <tuple>

#include <agz-utils/graphics_api/d3d12/graph/graphCompiler.h>
#include <agz-utils/string.h>

AGZ_D3D12_GRAPH_BEGIN

Resource::Resource(std::string name, int index)
    : name_(std::move(name)), index_(index), desc_{}
{
    
}

const std::string &Resource::getName() const
{
    return name_;
}

int Resource::getIndex() const
{
    return index_;
}

bool Resource::isInternal() const
{
    return asInternal() != nullptr;
}

bool Resource::isExternal() const
{
    return asExternal() != nullptr;
}

InternalResource *Resource::asInternal()
{
    return nullptr;
}

const InternalResource *Resource::asInternal() const
{
    return nullptr;
}

ExternalResource *Resource::asExternal()
{
    return nullptr;
}

const ExternalResource *Resource::asExternal() const
{
    return nullptr;
}

void Resource::setDescription(const D3D12_RESOURCE_DESC &desc)
{
    desc_ = desc;
}

const D3D12_RESOURCE_DESC &Resource::getDescription() const
{
    return desc_;
}

InternalResource::InternalResource(std::string name, int index)
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

InternalResource *InternalResource::asInternal()
{
    return this;
}

const InternalResource *InternalResource::asInternal() const
{
    return this;
}

ExternalResource::ExternalResource(std::string name, int index)
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

ExternalResource *ExternalResource::asExternal()
{
    return this;
}

const ExternalResource *ExternalResource::asExternal() const
{
    return this;
}

DescriptorTable::DescriptorTable(bool cpu, bool gpu)
    : cpu_(cpu), gpu_(gpu)
{
    
}

void DescriptorTable::addSRV(
    const Resource                        *resource,
    ShaderResourceType                     type,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, type, {} });
}

void DescriptorTable::addUAV(
    const Resource                         *resource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, {} });
}

void DescriptorTable::addRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, {} });
}

void DescriptorTable::addDSV(
    const Resource                      *resource,
    DepthStencilType                     type,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    records_.push_back({ resource, desc, {}, type });
}

bool Vertex::isAggregate() const
{
    return asAggregate() != nullptr;
}

Pass *Vertex::asPass()
{
    return nullptr;
}

const Pass *Vertex::asPass() const
{
    return nullptr;
}

PassAggregate *Vertex::asAggregate()
{
    return nullptr;
}

const PassAggregate *Vertex::asAggregate() const
{
    return nullptr;
}

Pass::Pass(std::string name, int index)
    : name_(std::move(name)), index_(index),
      thread_(0), queue_(0)
{
    
}

Pass *Pass::asPass()
{
    return this;
}

const Pass *Pass::asPass() const
{
    return this;
}

const std::string &Pass::getName() const
{
    return name_;
}

int Pass::getIndex() const
{
    return index_;
}

void Pass::setQueue(int queueIndex)
{
    queue_ = queueIndex;
}

void Pass::setThread(int threadIndex)
{
    thread_ = threadIndex;
}

void Pass::setCallback(PassCallback callback)
{
    callback_ = std::make_shared<PassCallback>(std::move(callback));
}

void Pass::addResourceState(
    const Resource       *resource,
    D3D12_RESOURCE_STATES state)
{
    if(states_.find(resource) != states_.end())
    {
        throw D3D12Exception(
            "repeated resource state declaretion of " +
            resource->getName() + "in pass " + name_);
    }
    states_.insert({ resource, state });
}

void Pass::addSRV(
    DescriptorType                         type,
    const Resource                        *resource,
    ShaderResourceType                     shaderResourceType,
    const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
{
    descriptors_.push_back({ resource, type, desc, shaderResourceType, {} });
}

void Pass::addUAV(
    DescriptorType                          type,
    const Resource                         *resource,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
{
    descriptors_.push_back({ resource, type, desc, {}, {} });
}

void Pass::addRTV(
    const Resource                      *resource,
    const D3D12_RENDER_TARGET_VIEW_DESC &desc)
{
    descriptors_.push_back({ resource, CPUOnly, desc, {}, {} });
}

void Pass::addDSV(
    const Resource                      *resource,
    DepthStencilType                     depthStencilType,
    const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
{
    descriptors_.push_back({ resource, CPUOnly, desc, {}, depthStencilType });
}

DescriptorTable *Pass::addDescriptorTable(DescriptorType type)
{
    const bool cpu = type != GPUOnly;
    const bool gpu = type != CPUOnly;
    descriptorTables_.push_back(std::make_unique<DescriptorTable>(cpu, gpu));
    return descriptorTables_.back().get();
}

bool Pass::DescriptorDeclaretion::operator<(
    const DescriptorDeclaretion &rhs) const
{
    auto L = std::tie(
        resource, type, shaderResourceType, depthStencilType);
    auto R = std::tie(
        rhs.resource, rhs.type, rhs.shaderResourceType, rhs.depthStencilType);
    return L < R || (L == R && view < rhs.view);
}

PassAggregate::PassAggregate(std::string name)
    : name_(std::move(name)), entry_(nullptr), exit_(nullptr)
{
    
}

const std::string &PassAggregate::getName() const
{
    return name_;
}

void PassAggregate::setEntry(Vertex *entry)
{
    entry_ = entry;
}

void PassAggregate::setExit(Vertex *exit)
{
    exit_ = exit;
}

PassAggregate *PassAggregate::asAggregate()
{
    return this;
}

const PassAggregate *PassAggregate::asAggregate() const
{
    return this;
}

GraphCompiler::GraphCompiler()
    : threadCount_(0), queueCount_(0), frameCount_(0)
{
    auto entryPass = addPass("main_queue_entry_pass#auto-generated");
    entryPass->setCallback([](rg::PassContext &) { });
}

void GraphCompiler::setThreadCount(int count)
{
    threadCount_ = count;
}

void GraphCompiler::setQueueCount(int count)
{
    queueCount_ = count;
}

void GraphCompiler::setFrameCount(int count)
{
    frameCount_ = count;
}

InternalResource *GraphCompiler::addInternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());
    auto resource = std::make_unique<InternalResource>(std::move(name), index);
    auto ret = resource.get();
    resources_.push_back(std::move(resource));
    return ret;
}

ExternalResource *GraphCompiler::addExternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());
    auto resource = std::make_unique<ExternalResource>(std::move(name), index);
    auto ret = resource.get();
    resources_.push_back(std::move(resource));
    return ret;
}

Pass *GraphCompiler::addPass(std::string name, int thread, int queue)
{
    const int index = static_cast<int>(passes_.size());
    passes_.push_back(std::make_unique<Pass>(std::move(name), index));
    passes_.back()->setThread(thread);
    passes_.back()->setQueue(queue);
    return passes_.back().get();
}

PassAggregate *GraphCompiler::addAggregate(
    std::string name, Vertex *entry, Vertex *exit)
{
    aggregates_.push_back(std::make_unique<PassAggregate>(std::move(name)));
    aggregates_.back()->setEntry(entry);
    aggregates_.back()->setExit(exit);
    return aggregates_.back().get();
}

void GraphCompiler::addDependency(Vertex *head, Vertex *tail)
{
    addDependencyImpl(head, tail, false);
}

void GraphCompiler::addCrossFrameDependency(Vertex *head, Vertex *tail)
{
    addDependencyImpl(head, tail, true);
}

void GraphCompiler::compile(
    ID3D12Device                           *device,
    ResourceManager                        &resourceManager,
    DescriptorAllocator                    &descriptorAllocator,
    std::vector<ComPtr<ID3D12CommandQueue>> allQueues,
    GraphRuntime                           &runtime)
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

    addEntryDependencyForComputePasses();

    Temps temps = assignSectionsToThreads();

    generateSectionDependencies(device, temps);
    generateResourceTransitions(temps);
    generateDescriptorRecords(temps);

    runtime.reset();
    runtime.device_ = device;
    runtime.queues_ = std::move(allQueues);
    runtime.perThreadData_.resize(threadCount_);
    
    fillRuntimeResources(runtime, resourceManager, temps);
    fillRuntimeDescriptors(device, runtime, descriptorAllocator, temps);
    fillRuntimeSections(device, runtime, temps);
    fillRuntimeCommandAllocators(device, runtime);

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        runtime.threads_.emplace_back(
            &GraphRuntime::threadEntry, &runtime, ti);
    }
}

void GraphCompiler::addDependencyImpl(
    Vertex *head, Vertex *tail, bool crossFrame)
{
    auto getEntryPass = [](Vertex *base)
    {
        for(;;)
        {
            assert(base);
            if(auto p = base->asPass())
                return p;
            auto aggregate = base->asAggregate();
            base = aggregate->entry_;
            if(!base)
            {
                throw D3D12Exception(
                    "entry pass is not set in PassAggregate " +
                    aggregate->getName());
            }
        }
    };

    auto getExitPass = [](Vertex *base)
    {
        for(;;)
        {
            assert(base);
            if(auto p = base->asPass())
                return p;
            auto aggregate = base->asAggregate();
            base = aggregate->exit_;
            if(!base)
            {
                throw D3D12Exception(
                    "exit pass is not set in PassAggregate " +
                    aggregate->getName());
            }
        }
    };

    auto exitOfHead = getExitPass(head);
    auto entryOfTail = getEntryPass(tail);

    if(crossFrame)
    {
        exitOfHead->outToNextFrame_.insert(entryOfTail);
        entryOfTail->inFromLastFrame_.insert(exitOfHead);
    }
    else
    {
        exitOfHead->out_.insert(entryOfTail);
        entryOfTail->in_.insert(exitOfHead);
    }
}

std::vector<int> GraphCompiler::sortPasses() const
{
    // pass index -> number of prevs
    std::vector<int> inCount(passes_.size());

    // indices of unprocessed passes with no prevs
    std::queue<int> Q;

    for(size_t i = 0; i < passes_.size(); ++i)
    {
        const int c = static_cast<int>(passes_[i]->in_.size());
        inCount[i] = c;
        if(!c)
            Q.push(static_cast<int>(i));
    }

    std::vector<int> result;
    result.reserve(passes_.size());

    while(!Q.empty())
    {
        const int vIdx = Q.front();
        Q.pop();
        result.push_back(vIdx);

        for(auto out : passes_[vIdx]->out_)
        {
            if(!--inCount[out->index_])
                Q.push(out->index_);
        }
    }

    if(result.size() != passes_.size())
    {
        throw D3D12Exception(
            "failed to find a topological order for render graph");
    }

    return result;
}

void GraphCompiler::addEntryDependencyForComputePasses()
{
    for(auto &p : passes_)
    {
        if(p->queue_ != 0 && p->in_.empty())
            addDependency(passes_[0].get(), p.get());
    }
}

GraphCompiler::Temps GraphCompiler::assignSectionsToThreads() const
{
    Temps result;

    // apply topology sort on all passes

    result.linearPasses = sortPasses();

    // assign passes to threads

    result.threads.resize(threadCount_);
    result.passes.resize(passes_.size());

    for(int i : result.linearPasses)
    {
        result.passes[i].idxInThread =
            static_cast<int>(result.threads[passes_[i]->thread_].passes.size());
        result.threads[passes_[i]->thread_].passes.push_back(i);
    }

    // generate shouldSubmit flag

    for(auto &v : passes_)
    {
        for(auto o : v->out_)
        {
            if(v->queue_ != o->queue_ || v->thread_ != o->thread_)
            {
                result.passes[v->index_].shouldSubmit = true;
                
                const int idxInThread = result.passes[o->index_].idxInThread;
                if(idxInThread > 0)
                {
                    const int lastPassInThread =
                        result.threads[o->thread_].passes[idxInThread - 1];
                    result.passes[lastPassInThread].shouldSubmit = true;
                }
            }
        }

        for(auto o : v->outToNextFrame_)
        {
            if(v->queue_ != o->queue_ || v->thread_ != o->thread_)
            {
                result.passes[v->index_].shouldSubmit = true;

                const int idxInThread = result.passes[o->index_].idxInThread;
                if(idxInThread > 0)
                {
                    const int lastPassInThread =
                        result.threads[o->thread_].passes[idxInThread - 1];
                    result.passes[lastPassInThread].shouldSubmit = true;
                }
            }
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
                result.sections.push_back({
                  ti, sectionIndexInThread++, {}, 0, {}, {}, {}, {} });
                needNewSection = false;
            }

            auto &p = result.passes[pi];
            p.parentSection = t.sections.back();
            
            auto &s = result.sections[p.parentSection];
            s.passes.push_back(pi);

            if(p.shouldSubmit)
                needNewSection = true;
        }
    }

    return result;
}

void GraphCompiler::generateSectionDependencies(
    ID3D12Device *device,
    Temps        &temps) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &thread = temps.threads[ti];
        for(int si : thread.sections)
        {
            auto &section = temps.sections[si];

            std::set<int> outputSections;
            std::set<int> outputToNextFrameSections;
            for(int pi : section.passes)
            {
                for(auto o : passes_[pi]->out_)
                {
                    outputSections.insert(
                        temps.passes[o->getIndex()].parentSection);
                }

                for(auto o : passes_[pi]->outToNextFrame_)
                {
                    outputToNextFrameSections.insert(
                        temps.passes[o->getIndex()].parentSection);
                }
            }
            outputSections.erase(si);
            outputToNextFrameSections.erase(si);

            const int thisQueue = passes_[section.passes.front()]->queue_;
            bool needSignalFence = false;

            for(int outputSi : outputSections)
            {
                auto &outputSection = temps.sections[outputSi];
                const int outputQueue =
                    passes_[outputSection.passes.front()]->queue_;
                if(outputQueue != thisQueue)
                {
                    needSignalFence = true;
                    break;
                }
            }

            if(!needSignalFence)
            {
                for(int outputSi : outputToNextFrameSections)
                {
                    auto &outputSection = temps.sections[outputSi];
                    const int outputQueue =
                        passes_[outputSection.passes.front()]->queue_;
                    if(outputQueue != thisQueue)
                    {
                        needSignalFence = true;
                        break;
                    }
                }
            }

            if(needSignalFence)
            {
                AGZ_D3D12_CHECK_HR(
                    device->CreateFence(
                        0, D3D12_FENCE_FLAG_NONE,
                        IID_PPV_ARGS(section.signalFence.GetAddressOf())));
            }

            for(int outputSi : outputSections)
            {
                auto &outputSection = temps.sections[outputSi];
                const int outputQueue =
                    passes_[outputSection.passes.front()]->queue_;

                if(outputQueue != thisQueue)
                    outputSection.waitFences.push_back(section.signalFence);

                ++outputSection.externalDependenciesCount;
            }

            for(int outputSi : outputToNextFrameSections)
            {
                auto &outputSection = temps.sections[outputSi];
                const int outputQueue =
                    passes_[outputSection.passes.front()]->queue_;

                if(outputQueue != thisQueue)
                {
                    outputSection.waitFencesOfLastFrame.push_back(
                        section.signalFence);
                }
            }

            section.outputs = { outputSections.begin(), outputSections.end() };
        }
    }
}

void GraphCompiler::generateResourceTransitions(Temps &temps)
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

    for(int pi : temps.linearPasses)
    {
        auto &p = passes_[pi];

        std::map<int, D3D12_RESOURCE_STATES> rscStates;
        std::transform(
            p->states_.begin(), p->states_.end(),
            misc::direct_inserter(rscStates),
            [](const auto &pair)
        {
            return std::make_pair(pair.first->getIndex(), pair.second);
        });

        auto addNewState = [&](const Resource *rsc, D3D12_RESOURCE_STATES state)
        {
            if(auto it = rscStates.find(rsc->getIndex()); it != rscStates.end())
                it->second |= state;
            else
                rscStates.insert({ rsc->getIndex(), state });
        };

        auto shaderResourceTypeToState = [](ShaderResourceType type)
        {
            switch(type)
            {
            case ShaderResourceType::PixelOnly:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            case ShaderResourceType::NonPixelOnly:
                return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            case ShaderResourceType::PixelAndNonPixel:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            }
            misc::unreachable();
        };

        auto depthStencilTypeToState = [](DepthStencilType type)
        {
            switch(type)
            {
            case DepthStencilType::ReadOnly:
                return D3D12_RESOURCE_STATE_DEPTH_READ;
            case DepthStencilType::ReadAndWrite:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            }
            misc::unreachable();
        };

        auto addNewDescDecl = [&](
            const Resource     *resource,
            const ResourceView &view,
            ShaderResourceType  shaderResourceType,
            DepthStencilType    depthStencilType)
        {
            match_variant(
                view,
                [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
            {
                addNewState(
                    resource,
                    shaderResourceTypeToState(shaderResourceType));
            },
                [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
            {
                addNewState(
                    resource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            },
                [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
            {
                addNewState(
                    resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
            },
                [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
            {
                addNewState(
                    resource,
                    depthStencilTypeToState(depthStencilType));
            },
                [&](const std::monostate &)
            {
                misc::unreachable();
            });
        };

        for(auto &descDecl : p->descriptors_)
        {
            addNewDescDecl(
                descDecl.resource,
                descDecl.view,
                descDecl.shaderResourceType,
                descDecl.depthStencilType);
        }

        for(auto &descTable : p->descriptorTables_)
        {
            for(auto &descDecl : descTable->records_)
            {
                addNewDescDecl(
                    descDecl.resource,
                    descDecl.view,
                    descDecl.shaderResourceType,
                    descDecl.depthStencilType);
            }
        }

        for(auto &r : rscStates)
        {
            resourceRecords[r.first].usages.push_back(
                { p->getIndex(), r.second });
        }
    }

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto &usages = resourceRecords[ri].usages;
        auto &resource = *resources_[ri];

        if(usages.empty())
            continue;

        if(resource.isInternal() &&
           resource.asInternal()->initialState_ == D3D12_RESOURCE_STATE_COMMON)
        {
            resource.asInternal()->initialState_ = usages[0].state;
        }

        const D3D12_RESOURCE_STATES initialState =
            resource.asInternal() ? resource.asInternal()->initialState_ :
            resource.asExternal()->initialState_;

        const D3D12_RESOURCE_STATES finalState =
            resource.asInternal() ? resource.asInternal()->initialState_ :
            resource.asExternal()->finalState_;

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
                temps.passes[usage.pass].stateTransitions.push_back(
                    PassRuntime::StateTransition{ &resource, beg, mid, end });
            }

            // TODO: UAV barrier

            lastState = end;
        }
    }
}

void GraphCompiler::generateDescriptorRecords(Temps &temps)
{
    // descriptors

    struct DescriptorKey
    {
        int thread = 0;
        Pass::DescriptorDeclaretion decl;

        bool operator<(const DescriptorKey &rhs) const
        {
            return std::tie(thread, decl) < std::tie(rhs.thread, rhs.decl);
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

        temps.threads[key.thread].descDecls.push_back({ newSlot, key.decl });

        return newSlot;
    };

    for(auto &p : passes_)
    {
        auto &passTemp = temps.passes[p->getIndex()];
        for(auto &u : p->descriptors_)
        {
            assert(!u.view.is<std::monostate>());
            const int slot = allocateDescriptorSlot({ p->thread_, u });
            passTemp.descriptors_.push_back({ u.resource, slot });
        }
    }

    // descriptor ranges

    int rangeSlotCount = 0;
    auto allocateDescriptorRangeSlot = [&](
        int thread, const DescriptorTable *table)
    {
        const int newSlot = rangeSlotCount++;
        temps.threads[thread].descRanges.push_back({ newSlot, table });
        return newSlot;
    };

    for(auto &p : passes_)
    {
        auto &passTemp = temps.passes[p->getIndex()];
        for(auto &t : p->descriptorTables_)
        {
            const int slot = allocateDescriptorRangeSlot(p->thread_, t.get());
            passTemp.descriptorRanges_.push_back({ t.get(), slot });
        }
    }
}

void GraphCompiler::fillRuntimeResources(
    GraphRuntime    &runtime,
    ResourceManager &rscMgr,
    Temps           &temps) const
{
    runtime.resources_.resize(resources_.size());

    // create actual resources

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto resource = resources_[ri].get();
        if(auto internal = resource->asInternal())
        {
            UniqueResource actualResource;
            if(internal->clear_)
            {
                actualResource = rscMgr.create(
                    internal->heapType_, internal->getDescription(),
                    internal->initialState_, internal->clearValue_);
            }
            else
            {
                actualResource = rscMgr.create(
                    internal->heapType_, internal->getDescription(),
                    internal->initialState_);
            }

            AGZ_WHEN_DEBUG({
                actualResource.get().resource->SetName(
                    stdstr::u8_to_wstr(resource->getName()).c_str());
            });

            GraphRuntime::InternalResourceRuntime resourceRuntime;
            resourceRuntime.resource = std::move(actualResource);
            resourceRuntime.index    = static_cast<int>(ri);

            runtime.resources_[ri] = std::move(resourceRuntime);
        }
        else
        {
            GraphRuntime::ExternalResourceRuntime resourceRuntime;
            resourceRuntime.index = static_cast<int>(ri);
            
            runtime.resources_[ri] = std::move(resourceRuntime);
        }
    }

    // fill resourceRuntime.descriptorSlots/descriptorRangeSlots

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        static auto addDescSlot = [](
            GraphRuntime::ResourceRuntime &r, int newSlot)
        {
            match_variant(
                r,
                [&](GraphRuntime::InternalResourceRuntime &ir)
            {
                ir.descriptorSlots.push_back(newSlot);
            },
                [&](GraphRuntime::ExternalResourceRuntime &er)
            {
                er.descriptorSlots.push_back(newSlot);
            });
        };

        static auto addDescRangeSlot = [](
            GraphRuntime::ResourceRuntime &r, int newSlot)
        {
            match_variant(
                r,
                [&](GraphRuntime::InternalResourceRuntime &ir)
            {
                ir.descriptorRangeSlots.push_back(newSlot);
            },
                [&](GraphRuntime::ExternalResourceRuntime &er)
            {
                er.descriptorRangeSlots.push_back(newSlot);
            });
        };

        auto &thread = temps.threads[ti];

        for(auto &dr : thread.descDecls)
        {
            addDescSlot(
                runtime.resources_[dr.decl.resource->getIndex()], dr.slot);
        }

        for(auto &dr : thread.descRanges)
        {
            for(auto &r : dr.table->records_)
            {
                addDescRangeSlot(
                    runtime.resources_[r.resource->getIndex()], dr.slot);
            }
        }
    }
}

void GraphCompiler::fillRuntimeDescriptors(
    ID3D12Device        *device,
    GraphRuntime        &runtime,
    DescriptorAllocator &GPUDescAlloc,
    Temps               &temps) const
{
    // count descs

    auto isCPUDesc = [](auto type) { return type != Pass::GPUOnly; };
    auto isGPUDesc = [](auto type) { return type != Pass::CPUOnly; };

    uint32_t cpuDescCount = 0, gpuDescCount = 0, rtvCount = 0, dsvCount = 0;

    size_t descSlotCount = 0, descTableSlotCount = 0;

    for(auto &t : temps.threads)
    {
        descSlotCount      += t.descDecls.size();
        descTableSlotCount += t.descRanges.size();

        for(auto &descDecl : t.descDecls)
        {
            match_variant(
                descDecl.decl.view,
                [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
            {
                cpuDescCount += isCPUDesc(descDecl.decl.type);
                gpuDescCount += isGPUDesc(descDecl.decl.type);
            },
                [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
            {
                cpuDescCount += isCPUDesc(descDecl.decl.type);
                gpuDescCount += isGPUDesc(descDecl.decl.type);
            },
                [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
            {
                rtvCount++;
            },
                [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
            {
                dsvCount++;
            },
                [](const std::monostate &)
            {
                misc::unreachable();
            });
        }

        for(auto &tableDecl : t.descRanges)
        {
            for(auto &record : tableDecl.table->records_)
            {
                match_variant(
                    record.view,
                    [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
                {
                    cpuDescCount += tableDecl.table->cpu_;
                    gpuDescCount += tableDecl.table->gpu_;
                },
                    [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
                {
                    cpuDescCount += tableDecl.table->cpu_;
                    gpuDescCount += tableDecl.table->gpu_;
                },
                    [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
                {
                    rtvCount++;
                },
                    [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
                {
                    dsvCount++;
                },
                    [](const std::monostate &)
                {
                    misc::unreachable();
                });
            }
        }
    }

    // allocate desc ranges

    DescriptorRange allCPUDescs, allGPUDescs, allRTVs, allDSVs;
    
    if(cpuDescCount)
    {
        runtime.CPUDescriptorHeap_.initialize(
            device,
            cpuDescCount * frameCount_,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            false);
        allCPUDescs =
            runtime.CPUDescriptorHeap_.allocRange(cpuDescCount * frameCount_);
    }

    runtime.GPUDescAlloc_ = &GPUDescAlloc;
    if(gpuDescCount)
    {
        runtime.GPUDescRange_ = GPUDescAlloc.allocStaticRange(
            gpuDescCount * frameCount_);
        allGPUDescs = runtime.GPUDescRange_;
    }

    if(rtvCount)
    {
        runtime.RTVDescriptorHeap_.initialize(
            device,
            rtvCount * frameCount_,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            false);
        allRTVs = runtime.RTVDescriptorHeap_.allocRange(rtvCount * frameCount_);
    }

    if(dsvCount)
    {
        runtime.DSVDescriptorHeap_.initialize(
            device,
            dsvCount * frameCount_,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            false);
        allDSVs = runtime.DSVDescriptorHeap_.allocRange(dsvCount * frameCount_);
    }

    // assign descs to slots

    runtime.descriptorSlots_     .resize(descSlotCount);
    runtime.descriptorRangeSlots_.resize(descTableSlotCount);

    uint32_t cpuDescOffset = 0, gpuDescOffset = 0, rtvOffset = 0, dsvOffset = 0;

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadTemp = temps.threads[ti];
        auto &threadData = runtime.perThreadData_[ti];

        // cpu desc

        for(auto &d : threadTemp.descDecls)
        {
            threadData.descriptorSlots.push_back(d.slot);
            auto &slot = runtime.descriptorSlots_[d.slot];

            slot.resourceIndex = d.decl.resource->getIndex();
            slot.cpu           = isCPUDesc(d.decl.type);
            slot.gpu           = isGPUDesc(d.decl.type);
            slot.view          = d.decl.view;
            slot.isDirty       = true;

            if(slot.view.is<D3D12_RENDER_TARGET_VIEW_DESC>())
            {
                for(int i = 0; i < frameCount_; ++i)
                    slot.freeCPUDescriptorQueue.push(allRTVs[rtvOffset++]);
            }
            else if(slot.view.is<D3D12_DEPTH_STENCIL_VIEW_DESC>())
            {
                for(int i = 0; i < frameCount_; ++i)
                    slot.freeCPUDescriptorQueue.push(allDSVs[dsvOffset++]);
            }
            else
            {
                for(int i = 0; i < frameCount_; ++i)
                {
                    if(slot.cpu)
                    {
                        slot.freeCPUDescriptorQueue.push(
                            allCPUDescs[cpuDescOffset++]);
                    }

                    if(slot.gpu)
                    {
                        slot.freeGPUDescriptorQueue.push(
                            allGPUDescs[gpuDescOffset++]);
                    }
                }
            }
        }

        // assign desc ranges to slots

        static auto viewToHeapType = [](const ResourceView &view)
        {
            return match_variant(
                view,
                [](const D3D12_SHADER_RESOURCE_VIEW_DESC &d)
            {
                return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            },
                [](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
            {
                return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            },
                [](const D3D12_RENDER_TARGET_VIEW_DESC &)
            {
                return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            },
                [](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
            {
                return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            },
                [](const std::monostate &)
            {
                return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            });
        };

        for(auto &d : threadTemp.descRanges)
        {
            threadData.descriptorRangeSlots.push_back(d.slot);
            auto &slot = runtime.descriptorRangeSlots_[d.slot];

            const uint32_t recordCount =
                static_cast<uint32_t>(d.table->records_.size());
            slot.resourceIndices.resize(recordCount);
            slot.views          .resize(recordCount);

            for(uint32_t i = 0; i < recordCount; ++i)
            {
                slot.resourceIndices[i] = d.table->records_[i].resource->getIndex();
                slot.views[i]           = d.table->records_[i].view;
            }

            slot.heapType = viewToHeapType(d.table->records_.front().view);
            slot.cpu      = d.table->cpu_;
            slot.gpu      = d.table->gpu_;
            slot.isDirty  = true;

            AGZ_WHEN_DEBUG({
                for(uint32_t i = 1; i < recordCount; ++i)
                {
                    if(slot.heapType != viewToHeapType(d.table->records_[i].view))
                    {
                        throw D3D12Exception(
                            "unmatched descriptor heap type"
                            "in a descriptor table declaretion");
                    }
                }
            });

            if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
            {
                for(int i = 0; i < frameCount_; ++i)
                {
                    slot.freeCPUDescriptorRangeQueue.push(
                        allRTVs.getSubRange(rtvOffset, recordCount));
                    rtvOffset += recordCount;
                }
            }
            else if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
            {
                for(int i = 0; i < frameCount_; ++i)
                {
                    slot.freeCPUDescriptorRangeQueue.push(
                        allDSVs.getSubRange(dsvOffset, recordCount));
                    dsvOffset += recordCount;
                }
            }
            else
            {
                for(int i = 0; i < frameCount_; ++i)
                {
                    if(slot.cpu)
                    {
                        slot.freeCPUDescriptorRangeQueue.push(
                            allCPUDescs.getSubRange(cpuDescOffset, recordCount));
                        cpuDescOffset += recordCount;
                    }

                    if(slot.gpu)
                    {
                        slot.freeGPUDescriptorRangeQueue.push(
                            allGPUDescs.getSubRange(gpuDescOffset, recordCount));
                        gpuDescOffset += recordCount;
                    }
                }
            }
        }
    }
}

void GraphCompiler::fillRuntimeSections(
    ID3D12Device *device,
    GraphRuntime &runtime,
    Temps        &temps) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadTemp = temps.threads[ti];
        auto &threadData = runtime.perThreadData_[ti];
        threadData.sections.resize(threadTemp.sections.size());
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

            const int queue = passes_[sectionTemp.passes.front()]->queue_;

            section.setDevice(device);
            section.setQueue(queue);
            section.setFrameCount(frameCount_);
            section.setCommandListType(runtime.queues_[queue]->GetDesc().Type);

            for(int pi : sectionTemp.passes)
            {
                auto &passTemp = temps.passes[pi];
                auto pass      = passes_[pi].get();

                PassContext::DescriptorMap      descriptorMap;
                PassContext::DescriptorRangeMap descriptorRangeMap;

                for(auto &d : passTemp.descriptors_)
                {
                    descriptorMap.insert({
                        d.resource,
                        &runtime.descriptorSlots_[d.descriptorSlot]
                    });
                }

                for(auto &d : passTemp.descriptorRanges_)
                {
                    descriptorRangeMap.insert({
                        d.table,
                        &runtime.descriptorRangeSlots_[d.descriptorRangeSlot]
                    });
                }

                section.addPass(PassRuntime(
                    &runtime,
                    pass->callback_,
                    passTemp.stateTransitions,
                    std::move(descriptorMap),
                    std::move(descriptorRangeMap)));
            }

            for(auto &f : sectionTemp.waitFences)
                section.addWaitFence(f);

            for(auto &f : sectionTemp.waitFencesOfLastFrame)
                section.addWaitFenceOfLastFrame(f);

            section.setSignalFence(sectionTemp.signalFence);

            for(auto out : sectionTemp.outputs)
            {
                auto &outTemp = temps.sections[out];
                section.addOutput(
                    &runtime.perThreadData_[outTemp.parentThread]
                            .sections[outTemp.indexInParentThread]);
            }

            section.setDependencies(sectionTemp.externalDependenciesCount);
        }
    }
}

void GraphCompiler::fillRuntimeCommandAllocators(
    ID3D12Device *device,
    GraphRuntime &runtime) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadData = runtime.perThreadData_[ti];

        bool hasGraphics = false;
        bool hasCompute  = false;

        for(size_t si = 0; si < threadData.sections.size(); ++si)
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

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
