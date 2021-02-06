#ifdef AGZ_ENABLE_D3D12

#include <tuple>

#include <agz-utils/graphics_api/d3d12/graph/compiler/compiler.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/resourceStatesTracker.h>
#include <agz-utils/graphics_api/d3d12/graph/compiler/viewComparer.h>
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
    std::vector<ResourceStatesTracker> trackers(resources_.size());
    for(size_t i = 0; i < resources_.size(); ++i)
        trackers[i] = ResourceStatesTracker(resources_[i].get());

    for(int pi : temps.linearPasses)
    {
        auto &p = passes_[pi];

        for(auto &pair : p->states_)
        {
            trackers[pair.first->getIndex()].addResourceState(
                p->getIndex(), pair.second.subresource, pair.second.state);
        }

        for(auto &desc : p->descriptors_)
        {
            trackers[desc->getResource()->getIndex()].addDescriptor(
                p->getIndex(), desc->getInfo());

            if(desc->getInfo().uavCounterResource)
            {
                trackers[desc->getInfo().uavCounterResource->getIndex()]
                    .addResourceState(
                        p->getIndex(), 0, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            }
        }

        for(auto &table : p->descriptorTables_)
        {
            for(auto &descInfo : table->getRecords())
            {
                trackers[descInfo.resource->getIndex()].addDescriptor(
                    p->getIndex(), descInfo);
            }
        }
    }

    std::vector<std::vector<ResourceStatesTracker::Usage>> resourceRecords(
        resources_.size());
    for(size_t i = 0; i < resources_.size(); ++i)
        resourceRecords[i] = trackers[i].getUsages();

    for(size_t ri = 0; ri < resources_.size(); ++ri)
    {
        auto &usages = resourceRecords[ri];
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
    temps.descSlotAllocator = DescriptorSlotAssigner(threadCount_);

    for(auto &p : passes_)
    {
        auto &passTemp = temps.passes[p->getIndex()];

        for(auto &desc : p->descriptors_)
        {
            const int slot = temps.descSlotAllocator.allocateDescriptorSlot(
                { p->thread_, desc.get() });
            passTemp.descriptors_.push_back( { desc.get(), slot } );
        }

        for(auto &table : p->descriptorTables_)
        {
            const int slot = temps.descSlotAllocator.allocateDescriptorRangeSlot(
                { p->thread_, table.get() });
            passTemp.descriptorRanges_.push_back({ table.get(), slot });
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

        for(auto &record : temps.descSlotAllocator.getDescriptorSlotRecords(ti))
        {
            addDescSlot(
                runtime.resources_[record.item->info_.resource->getIndex()],
                record.slot);

            if(record.item->info_.uavCounterResource)
            {
                const int index = record.item->info_.uavCounterResource->getIndex();
                addDescSlot(runtime.resources_[index], record.slot);
            }
        }

        for(auto &record : temps.descSlotAllocator.getDescriptorRangeSlotRecords(ti))
        {
            for(auto &descInfo : record.table->records_)
            {
                addDescRangeSlot(
                    runtime.resources_[descInfo.resource->getIndex()],
                    record.slot);

                if(descInfo.uavCounterResource)
                {
                    const int index = descInfo.resource->getIndex();
                    addDescRangeSlot(runtime.resources_[index], record.slot);
                }
            }
        }
    }
}

void Compiler::fillRuntimeDescriptors(
    ID3D12Device        *device,
    Runtime             &runtime,
    DescriptorAllocator &GPUDescAlloc,
    Temps               &temps) const
{
    for(int ti = 0; ti < threadCount_; ++ti)
    {
        auto &threadData = runtime.perThreadData_[ti];

        auto &descs =
            temps.descSlotAllocator.getDescriptorSlotRecords(ti);
        auto &descTables =
            temps.descSlotAllocator.getDescriptorRangeSlotRecords(ti);

        for(auto &descDecl : descs)
        {
            runtime.descSlotMgr_.addDescriptor(descDecl.item);
            threadData.descriptorSlots.push_back(descDecl.slot);
        }

        for(auto &tableDecl : descTables)
        {
            runtime.descSlotMgr_.addDescriptorTable(tableDecl.table);
            threadData.descriptorRangeSlots.push_back(tableDecl.slot);
        }
    }

    runtime.descSlotMgr_.initializeDescriptorSlots(
        device,
        frameCount_,
        &GPUDescAlloc,
        &temps.descSlotAllocator);
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
                        runtime.descSlotMgr_.getSlot(d.descriptorSlot)
                    });
                }

                for(auto &d : passTemp.descriptorRanges_)
                {
                    descriptorRangeMap.insert({
                        d.table,
                        runtime.descSlotMgr_.getRangeSlot(d.descriptorRangeSlot)
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
