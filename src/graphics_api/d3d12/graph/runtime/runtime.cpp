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
        refreshDescriptor(descriptorSlots_[i]);

    for(int i : threadData.descriptorRangeSlots)
        refreshDescriptorRange(descriptorRangeSlots_[i]);

    // reset command allocators

    auto &cmdAlloc = threadData.perFrameCmdAlloc[frameIndex_];
    if(cmdAlloc.graphics)
        cmdAlloc.graphics->Reset();
    if(cmdAlloc.compute)
        cmdAlloc.compute->Reset();

    // execute sections

    ID3D12DescriptorHeap *heaps[2] = { GPUDescAlloc_->getHeap(), nullptr };

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
        [](const InternalResourceRuntime &r)
    {
        return r.resource->resource.Get();
    },
        [](const ExternalResourceRuntime &r)
    {
        return r.resource.Get();
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
    external.resource = std::move(resource);

    for(auto ds : external.descriptorSlots)
        descriptorSlots_[ds].isDirty = true;

    for(auto ds : external.descriptorRangeSlots)
        descriptorRangeSlots_[ds].isDirty = true;
}

void Runtime::clearExternalResources()
{
    for(auto &r : resources_)
    {
        if(auto external = r.as_if<ExternalResourceRuntime>())
            external->resource.Reset();
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
    descriptorSlots_.clear();
    descriptorRangeSlots_.clear();

    perThreadData_.clear();
    threads_.clear();

    frameIndex_    = 0;
    threadTrigger_ = -1;
    fenceValue_    = 0;

    if(GPUDescAlloc_)
    {
        if(GPUDescRange_.getCount())
        {
            GPUDescAlloc_->freeRangeStatic(GPUDescRange_);
            GPUDescRange_ = {};
        }
        GPUDescAlloc_ = nullptr;
    }

    samplerHeap_.Reset();

    CPUDescriptorHeap_.destroy();
    RTVDescriptorHeap_.destroy();
    DSVDescriptorHeap_.destroy();
}

void Runtime::refreshDescriptor(DescriptorSlot &slot)
{
    if(!slot.isDirty)
        return;

    assert(!slot.view.is<std::monostate>());

    auto resource = getRawResource(slot.resourceIndex);

    assert(
        resources_[slot.resourceIndex].is<ExternalResourceRuntime>() ||
        (!slot.cpuDescriptor && !slot.gpuDescriptor));

    match_variant(
        slot.view,
        [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        if(slot.cpuDescriptor)
            slot.freeCPUDescriptorQueue.push(slot.cpuDescriptor);
        if(slot.gpuDescriptor)
            slot.freeGPUDescriptorQueue.push(slot.gpuDescriptor);

        if(slot.cpu)
        {
            assert(!slot.freeCPUDescriptorQueue.empty());
            slot.cpuDescriptor = slot.freeCPUDescriptorQueue.front();
            slot.freeCPUDescriptorQueue.pop();

            device_->CreateShaderResourceView(
                resource, &desc, slot.cpuDescriptor);
        }

        if(slot.gpu)
        {
            assert(!slot.freeGPUDescriptorQueue.empty());
            slot.gpuDescriptor = slot.freeGPUDescriptorQueue.front();
            slot.freeGPUDescriptorQueue.pop();

            if(slot.cpu)
            {
                device_->CopyDescriptorsSimple(
                    1, slot.gpuDescriptor, slot.cpuDescriptor,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
            else
            {
                device_->CreateShaderResourceView(
                    resource, &desc, slot.gpuDescriptor);
            }
        }
    },
        [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
    {
        if(slot.cpuDescriptor)
            slot.freeCPUDescriptorQueue.push(slot.cpuDescriptor);
        if(slot.gpuDescriptor)
            slot.freeGPUDescriptorQueue.push(slot.gpuDescriptor);

        if(slot.cpu)
        {
            assert(!slot.freeCPUDescriptorQueue.empty());
            slot.cpuDescriptor = slot.freeCPUDescriptorQueue.front();
            slot.freeCPUDescriptorQueue.pop();

            device_->CreateUnorderedAccessView(
                resource, nullptr, &desc, slot.cpuDescriptor);
        }

        if(slot.gpu)
        {
            assert(!slot.freeGPUDescriptorQueue.empty());
            slot.gpuDescriptor = slot.freeGPUDescriptorQueue.front();
            slot.freeGPUDescriptorQueue.pop();

            if(slot.cpu)
            {
                device_->CopyDescriptorsSimple(
                    1, slot.gpuDescriptor, slot.cpuDescriptor,
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
            else
            {
                device_->CreateUnorderedAccessView(
                    resource, nullptr, &desc, slot.gpuDescriptor);
            }
        }
    },
        [&](const D3D12_RENDER_TARGET_VIEW_DESC &desc)
    {
        assert(slot.cpu && !slot.gpu);

        if(slot.cpuDescriptor)
            slot.freeCPUDescriptorQueue.push(slot.cpuDescriptor);

        assert(!slot.freeCPUDescriptorQueue.empty());
        slot.cpuDescriptor = slot.freeCPUDescriptorQueue.front();
        slot.freeCPUDescriptorQueue.pop();

        device_->CreateRenderTargetView(resource, &desc, slot.cpuDescriptor);
    },
        [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        assert(slot.cpu && !slot.gpu);

        if(slot.cpuDescriptor)
            slot.freeCPUDescriptorQueue.push(slot.cpuDescriptor);

        assert(!slot.freeCPUDescriptorQueue.empty());
        slot.cpuDescriptor = slot.freeCPUDescriptorQueue.front();
        slot.freeCPUDescriptorQueue.pop();

        device_->CreateDepthStencilView(resource, &desc, slot.cpuDescriptor);
    },
        [&](const std::monostate &)
    {
        misc::unreachable();
    });

    slot.isDirty = false;
}

void Runtime::refreshDescriptorRange(DescriptorRangeSlot &slot)
{
    if(!slot.isDirty)
        return;

    if(slot.cpuDescriptorRange.getCount())
        slot.freeCPUDescriptorRangeQueue.push(slot.cpuDescriptorRange);
    if(slot.gpuDescriptorRange.getCount())
        slot.freeGPUDescriptorRangeQueue.push(slot.gpuDescriptorRange);

    auto createView = [&](
        ID3D12Resource   *resource,
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
            device_->CreateUnorderedAccessView(
                resource, nullptr, &desc, descriptor);
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
        slot.cpuDescriptorRange = slot.freeCPUDescriptorRangeQueue.front();
        slot.freeCPUDescriptorRangeQueue.pop();

        for(uint32_t i = 0; i < slot.views.size(); ++i)
        {
            ID3D12Resource *resource = getRawResource(slot.resourceIndices[i]);
            createView(resource, slot.cpuDescriptorRange[i], slot.views[i]);
        }
    }

    if(slot.gpu)
    {
        slot.gpuDescriptorRange = slot.freeGPUDescriptorRangeQueue.front();
        slot.freeGPUDescriptorRangeQueue.pop();

        if(slot.cpu)
        {
            device_->CopyDescriptorsSimple(
                slot.gpuDescriptorRange.getCount(),
                slot.gpuDescriptorRange[0],
                slot.cpuDescriptorRange[0],
                slot.heapType);
        }
        else
        {
            for(uint32_t i = 0; i < slot.views.size(); ++i)
            {
                ID3D12Resource *resource = getRawResource(slot.resourceIndices[i]);
                createView(resource, slot.gpuDescriptorRange[i], slot.views[i]);
            }
        }
    }

    slot.isDirty = false;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
