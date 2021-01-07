#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/renderGraph/compiler.h>
#include <agz-utils/graphics_api/d3d12/renderGraph/runtime.h>
#include <agz-utils/system.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

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
        for(size_t si = 0; si < d.sectionCount; ++si)
            d.sections[si].resetFinishedDependencies();
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

void Runtime::iterationFunc(int threadIndex)
{
    auto &threadData = perThreadData_[threadIndex];

    // refresh descriptors

    for(int i : threadData.descriptorSlots)
        refreshDescriptorSlot(threadIndex, descriptorSlots_[i]);

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

    for(size_t si = 0; si < threadData.sectionCount; ++si)
    {
        auto &section = threadData.sections[si];
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
                lk, [=] { return trigger != threadTrigger_; });
            newTrigger = threadTrigger_;
        }
    
        assert(trigger != newTrigger);
        trigger = newTrigger;
        if(trigger < 0)
            break;
    
        iterationFunc(threadIndex);
    
        sync_.latch.count_down();
    }
}

ID3D12Resource *Runtime::getRawResource(Resource *resource) noexcept
{
    return getRawResource(resource->getIndex());
}

ID3D12Resource *Runtime::getRawResource(int index) noexcept
{
    return match_variant(
        resources_[index],
        [](const RuntimeInternalResource &r)
    {
        return r.resource->resource.Get();
    },
        [](const RuntimeExternalResource &r)
    {
        return r.resource.Get();
    });
}

void Runtime::setSamplerHeap(ComPtr<ID3D12DescriptorHeap> heap)
{
    samplerHeap_ = std::move(heap);
}

void Runtime::setExternalResource(
    ExternalResource *node, ComPtr<ID3D12Resource> rsc)
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

        auto desc = node->getDescription();
        assert(desc->Width <= 0 || isEqual(*desc, rsc->GetDesc()));
    });

    auto &external = resources_[node->getIndex()].as<RuntimeExternalResource>();
    external.resource = std::move(rsc);
    for(auto ds : external.descriptorSlots)
        descriptorSlots_[ds].isDirty = true;
}

void Runtime::clearExternalResources()
{
    for(auto &r : resources_)
    {
        if(auto external = r.as_if<RuntimeExternalResource>())
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

    perThreadData_.reset();
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

    RTVDescriptorHeap_.destroy();
    DSVDescriptorHeap_.destroy();
}

void Runtime::refreshDescriptorSlot(int threadIndex, DescriptorSlot &slot)
{
    if(!slot.isDirty)
        return;

    auto &threadData = perThreadData_[threadIndex];

    assert(!slot.view.is<std::monostate>());

    auto resource = getRawResource(slot.resourceIndex);

    match_variant(
        slot.view,
        [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &desc)
    {
        if(slot.descriptor)
            threadData.GPUDescQueue.free(slot.descriptor);
        slot.descriptor = threadData.GPUDescQueue.alloc();

        device_->CreateShaderResourceView(
            resource, &desc, slot.descriptor);
        slot.isDirty = false;
    },
        [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc)
    {
        if(slot.descriptor)
            threadData.GPUDescQueue.free(slot.descriptor);
        slot.descriptor = threadData.GPUDescQueue.alloc();

        device_->CreateUnorderedAccessView(
            resource, nullptr, &desc, slot.descriptor);
        slot.isDirty = false;
    },
        [&](const D3D12_RENDER_TARGET_VIEW_DESC &desc)
    {
        if(slot.descriptor)
            threadData.RTVDescQueue.free(slot.descriptor);
        slot.descriptor = threadData.RTVDescQueue.alloc();

        device_->CreateRenderTargetView(
            resource, &desc, slot.descriptor);
        slot.isDirty = false;
    },
        [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &desc)
    {
        if(slot.descriptor)
            threadData.DSVDescQueue.free(slot.descriptor);
        slot.descriptor = threadData.DSVDescQueue.alloc();

        device_->CreateDepthStencilView(
            resource, &desc, slot.descriptor);
        slot.isDirty = false;
    },
        [&](const std::monostate &) {});
}

AGZ_D3D12_RENDERGRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
