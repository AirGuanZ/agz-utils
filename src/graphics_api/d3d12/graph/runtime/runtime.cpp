#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/compiler.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/runtime.h>

AGZ_D3D12_GRAPH_BEGIN

Runtime::~Runtime()
{
    reset();
}

void Runtime::runAsync(int frameIndex)
{
    // launch_thread.set_latch_count   | 
    // launch_thread.set_trigger       | 
    // launch_thread.awake_thread_cond | 
    //                                 | worker_thread.init_frame_work
    //                                 | worker_thread.submit
    //                                 | worker_thread.count_down_latch
    // launch_thread.wait_launch       |

    ++fenceValue_;

    for(size_t ti = 0; ti < threads_.size(); ++ti)
    {
        auto &d = perThreadData_[ti];
        for(auto &s : d.sections)
            s.resetFinishedDependencies();
    }

    sync_.latch.set_counter(static_cast<int>(threads_.size()));
    {
        std::lock_guard lk(sync_.mutex);
        frameIndex_ = frameIndex;
        ++threadTrigger_;
    }
    sync_.cond.notify_all();
}

void Runtime::sync()
{
    sync_.latch.wait();
}

void Runtime::run(int frameIndex)
{
    runAsync(frameIndex);
    sync();
}

void Runtime::frameFunc(int threadIndex)
{
    auto &threadData = perThreadData_[threadIndex];

    // refresh descriptors

    for(int i : threadData.descriptorSlots)
        refreshDescriptor(*descSlotMgr_.getSlot(i));

    for(int i : threadData.descriptorRangeSlots)
        refreshDescriptorRange(*descSlotMgr_.getRangeSlot(i));

    // reset command allocators

    auto &cmdAlloc = threadData.perFrameCmdAlloc[frameIndex_];
    if(cmdAlloc.graphics)
        cmdAlloc.graphics->Reset();
    if(cmdAlloc.compute)
        cmdAlloc.compute->Reset();

    // execute sections

    ID3D12DescriptorHeap *heaps[2] = { descSlotMgr_.getGPUHeap(), nullptr };

    int heapCount = 1;
    if(samplerHeap_)
    {
        heaps[1] = samplerHeap_.Get();
        heapCount = 2;
    }

    for(auto &section : threadData.sections)
    {
        auto cmdListType = section.getCommandListType();

        auto tCmdAlloc =
            cmdListType == D3D12_COMMAND_LIST_TYPE_DIRECT ?
            cmdAlloc.graphics.Get() : cmdAlloc.compute.Get();

        assert(tCmdAlloc);
        section.execute(
            frameIndex_, fenceValue_, tCmdAlloc,
            queues_.data(), heaps, heapCount);
    }
}

void Runtime::threadEntry(int threadIndex)
{
    int trigger = -1;
    for(;;)
    {
        int newTrigger;
        {
            std::unique_lock lk(sync_.mutex);
            sync_.cond.wait(
                lk, [=, this] { return trigger != threadTrigger_; });
            newTrigger = threadTrigger_;
        }
    
        assert(trigger != newTrigger);
        trigger = newTrigger;
        if(trigger < 0)
            break;
    
        frameFunc(threadIndex);
    
        sync_.latch.count_down();
    }
}

ID3D12Resource *Runtime::getRawResource(const Resource *resource)
{
    return getRawResource(resource->getIndex());
}

ID3D12Resource *Runtime::getRawResource(int resourceIndex)
{
    return match_variant(
        resources_[resourceIndex],
        [this](const InternalResourceRuntime &r)
    {
        const int index = r.isPerFrame ? frameIndex_ : 0;
        return r.resource[index]->resource.Get();
    },
        [this](const ExternalResourceRuntime &r)
    {
        const int index = r.isPerFrame ? frameIndex_ : 0;
        return r.resource[index].Get();
    });
}

void Runtime::setSamplerHeap(ComPtr<ID3D12DescriptorHeap> heap)
{
    samplerHeap_ = std::move(heap);
}

void Runtime::setExternalResource(
    ExternalResource      *node,
    ComPtr<ID3D12Resource> resource)
{
    assert(!node->isPerFrame());
    setExternalResource(node, 0, std::move(resource));
}

void Runtime::setExternalResource(
    ExternalResource *node, int frameIndex, ComPtr<ID3D12Resource> resource)
{
    AGZ_WHEN_DEBUG({
        auto toTie = [](const D3D12_RESOURCE_DESC &L)
        {
            return std::tie(
                L.Dimension,
                L.Alignment,
                L.Width,
                L.Height,
                L.DepthOrArraySize,
                L.MipLevels,
                L.Format,
                L.SampleDesc.Count,
                L.SampleDesc.Quality,
                L.Layout,
                L.Flags);
        };

        auto isEqual = [&toTie](
            const D3D12_RESOURCE_DESC &L,
            const D3D12_RESOURCE_DESC &R)
        {
            return toTie(L) == toTie(R);
        };

        auto &desc = node->getDescription();
        assert(desc.Width <= 0 || isEqual(desc, resource->GetDesc()));
    });

    auto &external = resources_[node->getIndex()].as<ExternalResourceRuntime>();
    external.resource[frameIndex] = std::move(resource);

    for(auto ds : external.descriptorSlots)
        descSlotMgr_.getSlot(ds)->frames[frameIndex].isDirty = true;

    for(auto ds : external.descriptorRangeSlots)
        descSlotMgr_.getRangeSlot(ds)->frames[frameIndex].isDirty = true;
}

void Runtime::clearExternalResources()
{
    for(auto &r : resources_)
    {
        if(auto external = r.as_if<ExternalResourceRuntime>())
        {
            for(auto &d3drsc : external->resource)
                d3drsc.Reset();
        }
    }
}

void Runtime::reset()
{
    {
        std::lock_guard lk(sync_.mutex);
        threadTrigger_ = -10;
    }
    sync_.cond.notify_all();
    for(auto &t : threads_)
        t.join();

    device_ = nullptr;
    queues_.clear();

    resources_.clear();
    descSlotMgr_.reset();

    perThreadData_.clear();
    threads_.clear();

    frameIndex_    = 0;
    threadTrigger_ = -1;
    fenceValue_    = 0;

    samplerHeap_.Reset();
}

void Runtime::refreshDescriptor(DescriptorSlot &slot)
{
    DescriptorSlot::PerFrame *perFrame =
        slot.isPerFrame ? &slot.frames[frameIndex_] : &slot.frames.front();

    if(!perFrame->isDirty)
        return;
    
    assert(!slot.view.is<std::monostate>());

    auto resource = getRawResource(slot.resourceIndex);

    assert(
        resources_[slot.resourceIndex].is<ExternalResourceRuntime>() ||
        (!perFrame->cpu && !perFrame->gpu));

    match_variant(
        slot.view,
        [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        if(perFrame->cpu)
            perFrame->freeCPUQueue.push(perFrame->cpu);
        if(perFrame->gpu)
            perFrame->freeGPUQueue.push(perFrame->gpu);

        if(slot.cpu)
        {
            assert(!perFrame->freeCPUQueue.empty());
            perFrame->cpu = perFrame->freeCPUQueue.front();
            perFrame->freeCPUQueue.pop();

            device_->CreateShaderResourceView(
                resource, &desc, perFrame->cpu);
        }

        if(slot.gpu)
        {
            assert(!perFrame->freeGPUQueue.empty());
            perFrame->gpu = perFrame->freeGPUQueue.front();
            perFrame->freeGPUQueue.pop();

            if(slot.cpu)
            {
                device_->CopyDescriptorsSimple(
                    1, perFrame->gpu, perFrame->cpu,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
            else
            {
                device_->CreateShaderResourceView(
                    resource, &desc, perFrame->gpu);
            }
        }
    },
        [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
    {
        if(perFrame->cpu)
            perFrame->freeCPUQueue.push(perFrame->cpu);
        if(perFrame->gpu)
            perFrame->freeGPUQueue.push(perFrame->gpu);

        ID3D12Resource *uavCounterResource = nullptr;
        if(slot.uavCounterResourceIndex >= 0)
            uavCounterResource = getRawResource(slot.uavCounterResourceIndex);

        if(slot.cpu)
        {
            assert(!perFrame->freeCPUQueue.empty());
            perFrame->cpu = perFrame->freeCPUQueue.front();
            perFrame->freeCPUQueue.pop();

            device_->CreateUnorderedAccessView(
                resource, uavCounterResource, &desc, perFrame->cpu);
        }

        if(slot.gpu)
        {
            assert(!perFrame->freeGPUQueue.empty());
            perFrame->gpu = perFrame->freeGPUQueue.front();
            perFrame->freeGPUQueue.pop();

            if(slot.cpu)
            {
                device_->CopyDescriptorsSimple(
                    1, perFrame->gpu, perFrame->cpu,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
            else
            {
                device_->CreateUnorderedAccessView(
                    resource, uavCounterResource, &desc, perFrame->gpu);
            }
        }
    },
        [&](const D3D12_RENDER_TARGET_VIEW_DESC &desc)
    {
        assert(slot.cpu && !slot.gpu);

        if(perFrame->cpu)
            perFrame->freeCPUQueue.push(perFrame->cpu);

        assert(!perFrame->freeCPUQueue.empty());
        perFrame->cpu = perFrame->freeCPUQueue.front();
        perFrame->freeCPUQueue.pop();

        device_->CreateRenderTargetView(resource, &desc, perFrame->cpu);
    },
        [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        assert(slot.cpu && !slot.gpu);

        if(perFrame->cpu)
            perFrame->freeCPUQueue.push(perFrame->cpu);

        assert(!perFrame->freeCPUQueue.empty());
        perFrame->cpu = perFrame->freeCPUQueue.front();
        perFrame->freeCPUQueue.pop();

        device_->CreateDepthStencilView(resource, &desc, perFrame->cpu);
    },
        [&](const std::monostate &)
    {
        misc::unreachable();
    });

    perFrame->isDirty = false;
}

void Runtime::refreshDescriptorRange(DescriptorRangeSlot &slot)
{
    DescriptorRangeSlot::PerFrame *perFrame =
        slot.isPerFrame ? &slot.frames[frameIndex_] : &slot.frames.front();

    if(!perFrame->isDirty)
        return;

    if(perFrame->cpu.getCount())
        perFrame->freeCPUQueue.push(perFrame->cpu);
    if(perFrame->gpu.getCount())
        perFrame->freeGPUQueue.push(perFrame->gpu);

    auto createView = [&](
        ID3D12Resource   *resource,
        int               uavCounterResourceIndex,
        const Descriptor &descriptor,
        ResourceView     &view)
    {
        match_variant(
            view,
            [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
        {
            assert(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            device_->CreateShaderResourceView(
                resource, &desc, descriptor);
        },
            [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
        {
            assert(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            ID3D12Resource *uavCounterResource = nullptr;
            if(uavCounterResourceIndex >= 0)
                uavCounterResource = getRawResource(uavCounterResourceIndex);

            device_->CreateUnorderedAccessView(
                resource, uavCounterResource, &desc, descriptor);
        },
            [&](const D3D12_RENDER_TARGET_VIEW_DESC &desc)
        {
            assert(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            device_->CreateRenderTargetView(
                resource, &desc, descriptor);
        },
            [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
        {
            assert(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            device_->CreateDepthStencilView(
                resource, &desc, descriptor);
        },
            [&](const std::monostate &)
        {
            misc::unreachable();
        });
    };

    if(slot.cpu)
    {
        perFrame->cpu= perFrame->freeCPUQueue.front();
        perFrame->freeCPUQueue.pop();

        for(uint32_t i = 0; i < slot.views.size(); ++i)
        {
            ID3D12Resource *resource = getRawResource(slot.resourceIndices[i]);
            createView(
                resource,
                slot.uavCounterResourceIndices[i],
                perFrame->cpu[i],
                slot.views[i]);
        }
    }

    if(slot.gpu)
    {
        perFrame->gpu = perFrame->freeGPUQueue.front();
        perFrame->freeGPUQueue.pop();

        if(slot.cpu)
        {
            device_->CopyDescriptorsSimple(
                perFrame->gpu.getCount(),
                perFrame->gpu[0],
                perFrame->cpu[0],
                slot.heapType);
        }
        else
        {
            for(uint32_t i = 0; i < slot.views.size(); ++i)
            {
                ID3D12Resource *resource = getRawResource(slot.resourceIndices[i]);
                createView(
                    resource,
                    slot.uavCounterResourceIndices[i],
                    perFrame->gpu[i],
                    slot.views[i]);
            }
        }
    }

    perFrame->isDirty = false;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
