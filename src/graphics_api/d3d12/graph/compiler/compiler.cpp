#ifdef AGZ_ENABLE_D3D12

#include <tuple>

#include <agz-utils/graphics_api/d3d12/graph/compiler/compiler.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/viewComparer.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/viewSubresource.h>
#include <agz-utils/graphics_api/d3d12/mipmapGenerator.h>
#include <agz-utils/string.h>

AGZ_D3D12_GRAPH_BEGIN

namespace
{
    UINT getTotalMipLevelCount(const D3D12_RESOURCE_DESC &rsc)
    {
        if(rsc.MipLevels > 0)
            return rsc.MipLevels;

        if(rsc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
        {
            return (std::max)(
                MipmapGenerator::computeMipmapChainLength(
                    static_cast<UINT>(rsc.Width),
                    static_cast<UINT>(rsc.Height)),
                MipmapGenerator::computeMipmapChainLength(
                    static_cast<UINT>(rsc.DepthOrArraySize), 1));
        }

        return MipmapGenerator::computeMipmapChainLength(
            static_cast<UINT>(rsc.Width),
            static_cast<UINT>(rsc.Height));
    }

    UINT getSubresourceCount(const D3D12_RESOURCE_DESC &desc)
    {
        const UINT arraySize =
            desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ?
            1 : desc.DepthOrArraySize;

        const UINT mipSliceCount = getTotalMipLevelCount(desc);

        return arraySize * mipSliceCount;
    }

} // namespace anonymous

Compiler::Compiler()
    : threadCount_(0), queueCount_(0), frameCount_(0)
{

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
    auto resource = std::make_unique<InternalResource>(std::move(name), index);
    auto ret = resource.get();
    resources_.push_back(std::move(resource));
    return ret;
}

ExternalResource *Compiler::addExternalResource(std::string name)
{
    const int index = static_cast<int>(resources_.size());
    auto resource = std::make_unique<ExternalResource>(std::move(name), index);
    auto ret = resource.get();
    resources_.push_back(std::move(resource));
    return ret;
}

Pass *Compiler::addPass(std::string name, int thread, int queue)
{
    const int index = static_cast<int>(passes_.size());
    passes_.push_back(std::make_unique<Pass>(std::move(name), index));
    passes_.back()->setThread(thread);
    passes_.back()->setQueue(queue);
    return passes_.back().get();
}

PassAggregate *Compiler::addAggregate(
    std::string name, Vertex *entry, Vertex *exit)
{
    aggregates_.push_back(std::make_unique<PassAggregate>(std::move(name)));
    aggregates_.back()->setEntry(entry);
    aggregates_.back()->setExit(exit);
    return aggregates_.back().get();
}

void Compiler::addDependency(Vertex *head, Vertex *tail)
{
    addDependencyImpl(head, tail, false);
}

void Compiler::addCrossFrameDependency(Vertex *head, Vertex *tail)
{
    addDependencyImpl(head, tail, true);
}

void Compiler::compile(
    ID3D12Device                           *device,
    ResourceManager                        &resourceManager,
    DescriptorAllocator                    &descriptorAllocator,
    std::vector<ComPtr<ID3D12CommandQueue>> allQueues,
    Runtime                           &runtime)
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
            &Runtime::threadEntry, &runtime, ti);
    }
}

void Compiler::addDependencyImpl(
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

std::vector<int> Compiler::sortPasses() const
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

Compiler::Temps Compiler::assignSectionsToThreads() const
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

void Compiler::generateSectionDependencies(
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

void Compiler::generateResourceTransitions(Temps &temps)
{
    struct ResourceRecord
    {
        struct Usage
        {
            int pass;
            std::map<UINT, D3D12_RESOURCE_STATES> state;
        };

        std::vector<Usage> usages;
    };

    std::vector<ResourceRecord> resourceRecords(resources_.size());

    for(int pi : temps.linearPasses)
    {
        auto &p = passes_[pi];

        std::map<std::pair<int, UINT>, D3D12_RESOURCE_STATES> rscStates;

        for(auto &pair : p->states_)
        {
            if(pair.second.subresource != D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
            {
                rscStates.insert({
                    std::make_pair(
                        pair.first->getIndex(),
                        pair.second.subresource),
                    pair.second.state
                });
            }
            else
            {
                const UINT subrscCount = getSubresourceCount(
                    pair.first->getDescription());

                for(UINT i = 0; i < subrscCount; ++i)
                {
                    rscStates.insert({
                        std::make_pair(pair.first->getIndex(), i),
                        pair.second.state
                    });
                }
            }
        }

        auto addNewState = [&](
            const Resource *rsc, UINT subrsc, D3D12_RESOURCE_STATES state)
        {
            if(auto it = rscStates.find({ rsc->getIndex(), subrsc });
               it != rscStates.end())
                it->second |= state;
            else
            {
                if(subrsc != D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
                    rscStates.insert({ { rsc->getIndex(), subrsc }, state });
                else
                {
                    const UINT subrscCount = getSubresourceCount(
                        rsc->getDescription());
                    for(UINT i = 0; i < subrscCount; ++i)
                        rscStates.insert({ { rsc->getIndex(), i }, state });
                }
            }
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
            const Resource     *uavCounterResource,
            UINT                subrsc,
            const ResourceView &view,
            ShaderResourceType  shaderResourceType,
            DepthStencilType    depthStencilType)
        {
            match_variant(
                view,
                [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
            {
                addNewState(
                    resource, subrsc,
                    shaderResourceTypeToState(shaderResourceType));
            },
                [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
            {
                addNewState(
                    resource, subrsc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

                if(uavCounterResource)
                {
                    addNewState(
                        uavCounterResource, 0,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
                }
            },
                [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
            {
                addNewState(
                    resource, subrsc, D3D12_RESOURCE_STATE_RENDER_TARGET);
            },
                [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
            {
                addNewState(
                    resource, subrsc,
                    depthStencilTypeToState(depthStencilType));
            },
                [&](const std::monostate &)
            {
                misc::unreachable();
            });
        };

        for(auto &descDecl : p->descriptors_)
        {
            auto subrscs = viewToSubresources(
                descDecl->info_.resource->getDescription(), descDecl->info_.view);

            for(UINT s : subrscs)
            {
                addNewDescDecl(
                    descDecl->info_.resource,
                    descDecl->info_.uavCounterResource,
                    s,
                    descDecl->info_.view,
                    descDecl->info_.shaderResourceType,
                    descDecl->info_.depthStencilType);
            }
        }

        for(auto &descTable : p->descriptorTables_)
        {
            for(auto &descDecl : descTable->records_)
            {
                auto subrscs = viewToSubresources(
                    descDecl.resource->getDescription(), descDecl.view);

                for(UINT s : subrscs)
                {
                    addNewDescDecl(
                        descDecl.resource,
                        descDecl.uavCounterResource,
                        s,
                        descDecl.view,
                        descDecl.shaderResourceType,
                        descDecl.depthStencilType);
                }
            }
        }

        for(auto &r : rscStates)
        {
            auto &record = resourceRecords[r.first.first];

            if(record.usages.empty() ||
               record.usages.back().pass != p->getIndex())
            {
                record.usages.push_back(
                    { p->getIndex(), {} });
            }

            record.usages.back().state.insert({ r.first.second, r.second });
        }
    }

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto &usages = resourceRecords[ri].usages;
        auto &resource = *resources_[ri];

        if(usages.empty())
            continue;

        const UINT subrscCount = getSubresourceCount(resource.getDescription());

        const D3D12_RESOURCE_STATES initialState =
            resource.asInternal() ? resource.asInternal()->initialState_ :
            resource.asExternal()->initialState_;

        const D3D12_RESOURCE_STATES finalState =
            resource.asInternal() ? resource.asInternal()->initialState_ :
            resource.asExternal()->finalState_;

        std::vector<D3D12_RESOURCE_STATES> subrscToState(
            subrscCount, initialState);

        for(size_t i = 0; i < usages.size(); ++i)
        {
            auto newSubrscToState = subrscToState;

            auto &usage = usages[i];
            assert(!usage.state.empty());
            for(auto &p : usage.state)
            {
                if(newSubrscToState[p.first] != p.second)
                    newSubrscToState[p.first] = p.second;
            }

            bool isAllOldStateSame = true;
            for(size_t j = 1; j < subrscToState.size(); ++j)
            {
                if(subrscToState[j - 1] != subrscToState[j])
                {
                    isAllOldStateSame = false;
                    break;
                }
            }

            bool isAllNewStateSame = true;
            for(size_t j = 1; j < newSubrscToState.size(); ++j)
            {
                if(newSubrscToState[j - 1] != newSubrscToState[j])
                {
                    isAllNewStateSame = false;
                    break;
                }
            }

            if(isAllOldStateSame && isAllNewStateSame)
            {
                const auto beg = subrscToState[0];
                const auto mid = newSubrscToState[0];
                const auto end =
                    (i == usages.size() - 1) ? finalState : mid;

                if(beg != mid || mid != end)
                {
                    temps.passes[usage.pass].stateTransitions.push_back(
                        PassRuntime::StateTransition{
                            .resource = &resource,
                            .subrsc   = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                            .beg      = beg,
                            .mid      = mid,
                            .end      = end
                        });
                }
            }
            else
            {
                for(UINT sub = 0; sub < subrscCount; ++sub)
                {
                    const auto beg = subrscToState[sub];
                    const auto mid = newSubrscToState[sub];
                    const auto end = (i == usages.size() - 1) ? finalState : mid;

                    if(beg != mid || mid != end)
                    {
                        temps.passes[usage.pass].stateTransitions.push_back(
                            PassRuntime::StateTransition{
                                .resource = &resource,
                                .subrsc   = sub,
                                .beg      = beg,
                                .mid      = mid,
                                .end      = end
                            });
                    }
                }
            }

            subrscToState = newSubrscToState;
        }

        // TODO: UAV barrier
    }
}

void Compiler::generateDescriptorRecords(Temps &temps)
{
    // descriptors

    struct DescriptorKey
    {
        int thread = 0;
        DescriptorItem *item = nullptr;
        
        bool operator<(const DescriptorKey &rhs) const
        {
            return std::tie(thread, *item) < std::tie(rhs.thread, *rhs.item);
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

        temps.threads[key.thread].descs.push_back({ newSlot, key.item });

        return newSlot;
    };

    for(auto &p : passes_)
    {
        auto &passTemp = temps.passes[p->getIndex()];
        for(auto &u : p->descriptors_)
        {
            assert(!u->info_.view.is<std::monostate>());
            const int slot = allocateDescriptorSlot({ p->thread_, u.get() });
            passTemp.descriptors_.push_back({ u.get(), slot });
        }
    }

    // descriptor ranges

    struct DescriptorRangeKey
    {
        int thread = 0;
        DescriptorTable *table = nullptr;

        bool operator<(const DescriptorRangeKey &rhs) const
        {
            return std::tie(thread, *table) < std::tie(rhs.thread, *rhs.table);
        }
    };

    std::map<DescriptorRangeKey, int> descriptorRangeSlotCache;

    auto allocateDescriptorRangeSlot = [&](const DescriptorRangeKey &key)
    {
        const auto it = descriptorRangeSlotCache.find(key);
        if(it != descriptorRangeSlotCache.end())
            return it->second;

        const int newSlot = static_cast<int>(descriptorRangeSlotCache.size());
        descriptorRangeSlotCache.insert({ key, newSlot });

        temps.threads[key.thread].descRanges.push_back({ newSlot, key.table });

        return newSlot;
    };

    for(auto &p : passes_)
    {
        auto &passTemp = temps.passes[p->getIndex()];
        for(auto &t : p->descriptorTables_)
        {
            const int slot = allocateDescriptorRangeSlot({ p->thread_, t.get() });
            passTemp.descriptorRanges_.push_back({ t.get(), slot });
        }
    }
}

void Compiler::fillRuntimeResources(
    Runtime         &runtime,
    ResourceManager &rscMgr,
    Temps           &temps) const
{
    runtime.resources_.resize(resources_.size());

    // create actual resources

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto resource = resources_[ri].get();

        const int actualResourceCount =
            resource->isPerFrame() ? frameCount_ : 1;

        if(auto internal = resource->asInternal())
        {
            Runtime::InternalResourceRuntime resourceRuntime;
            resourceRuntime.isPerFrame = resource->isPerFrame();
            resourceRuntime.index = static_cast<int>(ri);
            resourceRuntime.resource.resize(actualResourceCount);

            for(int i = 0; i < actualResourceCount; ++i)
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
                resourceRuntime.resource[i] = std::move(actualResource);
            }

            runtime.resources_[ri] = std::move(resourceRuntime);
        }
        else
        {
            Runtime::ExternalResourceRuntime resourceRuntime;
            resourceRuntime.isPerFrame = resource->isPerFrame();
            resourceRuntime.index = static_cast<int>(ri);
            resourceRuntime.resource.resize(actualResourceCount);
            
            runtime.resources_[ri] = std::move(resourceRuntime);
        }
    }

    // fill resourceRuntime.descriptorSlots/descriptorRangeSlots

    for(int ti = 0; ti < threadCount_; ++ti)
    {
        static auto addDescSlot = [](
            Runtime::ResourceRuntime &r, int newSlot)
        {
            match_variant(
                r,
                [&](Runtime::InternalResourceRuntime &ir)
            {
                ir.descriptorSlots.push_back(newSlot);
            },
                [&](Runtime::ExternalResourceRuntime &er)
            {
                er.descriptorSlots.push_back(newSlot);
            });
        };

        static auto addDescRangeSlot = [](
            Runtime::ResourceRuntime &r, int newSlot)
        {
            match_variant(
                r,
                [&](Runtime::InternalResourceRuntime &ir)
            {
                ir.descriptorRangeSlots.push_back(newSlot);
            },
                [&](Runtime::ExternalResourceRuntime &er)
            {
                er.descriptorRangeSlots.push_back(newSlot);
            });
        };

        auto &thread = temps.threads[ti];

        for(auto &dr : thread.descs)
        {
            addDescSlot(
                runtime.resources_[dr.item->info_.resource->getIndex()], dr.slot);

            if(dr.item->info_.uavCounterResource)
            {
                const int index = dr.item->info_.uavCounterResource->getIndex();
                addDescSlot(runtime.resources_[index], dr.slot);
            }
        }

        for(auto &dr : thread.descRanges)
        {
            for(auto &r : dr.table->records_)
            {
                addDescRangeSlot(
                    runtime.resources_[r.resource->getIndex()], dr.slot);

                if(r.uavCounterResource)
                {
                    const int index = r.resource->getIndex();
                    addDescRangeSlot(runtime.resources_[index], dr.slot);
                }
            }
        }
    }
}

void Compiler::fillRuntimeDescriptors(
    ID3D12Device        *device,
    Runtime        &runtime,
    DescriptorAllocator &GPUDescAlloc,
    Temps               &temps) const
{
    // count descs

    uint32_t cpuDescCount = 0, gpuDescCount = 0, rtvCount = 0, dsvCount = 0;

    size_t descSlotCount = 0, descTableSlotCount = 0;

    for(auto &t : temps.threads)
    {
        descSlotCount      += t.descs.size();
        descTableSlotCount += t.descRanges.size();

        for(auto &descDecl : t.descs)
        {
            match_variant(
                descDecl.item->info_.view,
                [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
            {
                cpuDescCount += descDecl.item->cpu_;
                gpuDescCount += descDecl.item->gpu_;
            },
                [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
            {
                cpuDescCount += descDecl.item->cpu_;
                gpuDescCount += descDecl.item->gpu_;
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

        for(auto &d : threadTemp.descs)
        {
            threadData.descriptorSlots.push_back(d.slot);
            auto &slot = runtime.descriptorSlots_[d.slot];

            bool isPerFrame = d.item->info_.resource->isPerFrame();
            if(d.item->info_.uavCounterResource)
                isPerFrame |= d.item->info_.uavCounterResource->isPerFrame();

            slot.resourceIndex = d.item->info_.resource->getIndex();
            slot.cpu           = d.item->cpu_;
            slot.gpu           = d.item->gpu_;
            slot.view          = d.item->info_.view;
            slot.isPerFrame    = isPerFrame;
            
            slot.uavCounterResourceIndex =
                d.item->info_.uavCounterResource ?
                d.item->info_.uavCounterResource->getIndex() : -1;

            if(isPerFrame)
            {
                slot.frames.resize(frameCount_);
                for(auto &frame : slot.frames)
                {
                    frame.isDirty = true;

                    if(slot.view.is<D3D12_RENDER_TARGET_VIEW_DESC>())
                    {
                        frame.freeCPUQueue.push(allRTVs[rtvOffset++]);
                    }
                    else if(slot.view.is<D3D12_DEPTH_STENCIL_VIEW_DESC>())
                    {
                        frame.freeCPUQueue.push(allDSVs[dsvOffset++]);
                    }
                    else
                    {
                        if(slot.cpu)
                            frame.freeCPUQueue.push(allCPUDescs[cpuDescOffset++]);
                        if(slot.gpu)
                            frame.freeGPUQueue.push(allGPUDescs[gpuDescOffset++]);
                    }
                }
            }
            else
            {
                slot.frames.emplace_back();
                auto &frame = slot.frames.front();

                frame.isDirty = true;

                if(slot.view.is<D3D12_RENDER_TARGET_VIEW_DESC>())
                {
                    for(int i = 0; i < frameCount_; ++i)
                        frame.freeCPUQueue.push(allRTVs[rtvOffset++]);
                }
                else if(slot.view.is<D3D12_DEPTH_STENCIL_VIEW_DESC>())
                {
                    for(int i = 0; i < frameCount_; ++i)
                        frame.freeCPUQueue.push(allDSVs[dsvOffset++]);
                }
                else
                {
                    for(int i = 0; i < frameCount_; ++i)
                    {
                        if(slot.cpu)
                        {
                            frame.freeCPUQueue.push(
                                allCPUDescs[cpuDescOffset++]);
                        }

                        if(slot.gpu)
                        {
                            frame.freeGPUQueue.push(
                                allGPUDescs[gpuDescOffset++]);
                        }
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
            slot.uavCounterResourceIndices.resize(recordCount);
            slot.views.resize(recordCount);

            bool isRangePerFrame = false;
            for(uint32_t i = 0; i < recordCount; ++i)
            {
                slot.resourceIndices[i] = d.table->records_[i].resource->getIndex();
                slot.views[i]           = d.table->records_[i].view;

                slot.uavCounterResourceIndices[i] =
                    d.table->records_[i].uavCounterResource ?
                    d.table->records_[i].uavCounterResource->getIndex() : -1;

                if(d.table->records_[i].resource->isPerFrame())
                    isRangePerFrame = true;

                if(d.table->records_[i].uavCounterResource &&
                   d.table->records_[i].uavCounterResource->isPerFrame())
                    isRangePerFrame = true;
            }

            slot.heapType   = viewToHeapType(d.table->records_.front().view);
            slot.cpu        = d.table->cpu_;
            slot.gpu        = d.table->gpu_;
            slot.isPerFrame = isRangePerFrame;
            
            if(isRangePerFrame)
            {
                slot.frames.resize(frameCount_);
                for(auto &frame : slot.frames)
                {
                    frame.isDirty = true;

                    if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
                    {
                        frame.freeCPUQueue.push(
                            allRTVs.getSubRange(rtvOffset, recordCount));
                        rtvOffset += recordCount;
                    }
                    else if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
                    {
                        frame.freeCPUQueue.push(
                            allDSVs.getSubRange(dsvOffset, recordCount));
                        dsvOffset += recordCount;
                    }
                    else
                    {
                        if(slot.cpu)
                        {
                            frame.freeCPUQueue.push(
                                allCPUDescs.getSubRange(cpuDescOffset, recordCount));
                            cpuDescOffset += recordCount;
                        }

                        if(slot.gpu)
                        {
                            frame.freeGPUQueue.push(
                                allGPUDescs.getSubRange(gpuDescOffset, recordCount));
                            gpuDescOffset += recordCount;
                        }
                    }
                }
            }
            else
            {
                slot.frames.emplace_back();
                auto &frame = slot.frames.front();

                frame.isDirty = true;

                if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
                {
                    for(int i = 0; i < frameCount_; ++i)
                    {
                        frame.freeCPUQueue.push(
                            allRTVs.getSubRange(rtvOffset, recordCount));
                        rtvOffset += recordCount;
                    }
                }
                else if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
                {
                    for(int i = 0; i < frameCount_; ++i)
                    {
                        frame.freeCPUQueue.push(
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
                            frame.freeCPUQueue.push(
                                allCPUDescs.getSubRange(cpuDescOffset, recordCount));
                            cpuDescOffset += recordCount;
                        }

                        if(slot.gpu)
                        {
                            frame.freeGPUQueue.push(
                                allGPUDescs.getSubRange(gpuDescOffset, recordCount));
                            gpuDescOffset += recordCount;
                        }
                    }
                }
            }

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
        }
    }
}

void Compiler::fillRuntimeSections(
    ID3D12Device *device,
    Runtime &runtime,
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
                        d.item,
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

                auto passRuntime = PassRuntime(
                    &runtime,
                    pass->callback_,
                    passTemp.stateTransitions,
                    std::move(descriptorMap),
                    std::move(descriptorRangeMap));

#ifdef AGZ_DEBUG

                passRuntime.setNameAndIndex(pass->name_, pass->index_);

#endif

                section.addPass(std::move(passRuntime));
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

void Compiler::fillRuntimeCommandAllocators(
    ID3D12Device *device,
    Runtime &runtime) const
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
