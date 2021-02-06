#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/runtime/descriptorSlotManager.h>

AGZ_D3D12_GRAPH_BEGIN

DescriptorSlotManager::~DescriptorSlotManager()
{
    reset();
}

void DescriptorSlotManager::reset()
{
    CPUDescCount_ = 0;
    GPUDescCount_ = 0;
    RTVCount_ = 0;
    DSVCount_ = 0;

    descSlotCount_      = 0;
    descTableSlotCount_ = 0;

    if(GPUDescAlloc_ && GPUDescRange_.getCount())
        GPUDescAlloc_->freeRangeStatic(GPUDescRange_);

    GPUDescAlloc_ = nullptr;
    GPUDescRange_ = {};

    CPUDescriptorHeap_.destroy();
    RTVDescriptorHeap_.destroy();
    DSVDescriptorHeap_.destroy();

    slots_.clear();
    rangeSlots_.clear();
}

void DescriptorSlotManager::addDescriptor(const DescriptorItem *item)
{
    ++descSlotCount_;

    match_variant(
        item->getInfo().view,
        [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
    {
        CPUDescCount_ += item->isOnCPU();
        GPUDescCount_ += item->isOnGPU();
    },
        [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
    {
        CPUDescCount_ += item->isOnCPU();
        GPUDescCount_ += item->isOnGPU();
    },
        [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
    {
        ++RTVCount_;
    },
        [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
    {
        ++DSVCount_;
    },
        [](const std::monostate &)
    {
        misc::unreachable();
    });
}

void DescriptorSlotManager::addDescriptorTable(const DescriptorTable *table)
{
    ++descTableSlotCount_;

    for(auto &record : table->getRecords())
    {
        match_variant(
            record.view,
            [&](const D3D12_SHADER_RESOURCE_VIEW_DESC &)
        {
            CPUDescCount_ += table->isOnCPU();
            GPUDescCount_ += table->isOnGPU();
        },
            [&](const D3D12_UNORDERED_ACCESS_VIEW_DESC &)
        {
            CPUDescCount_ += table->isOnCPU();
            GPUDescCount_ += table->isOnGPU();
        },
            [&](const D3D12_RENDER_TARGET_VIEW_DESC &)
        {
            ++RTVCount_;
        },
            [&](const D3D12_DEPTH_STENCIL_VIEW_DESC &)
        {
            ++DSVCount_;
        },
            [](const std::monostate &)
        {
            misc::unreachable();
        });
    }
}

void DescriptorSlotManager::initializeDescriptorSlots(
    ID3D12Device            *device,
    int                      frameCount,
    DescriptorAllocator     *GPUAllocator,
    DescriptorSlotAssigner *slotAllocator)
{
    // allocate descriptor ranges

    DescriptorRange allCPUDescs, allGPUDescs, allRTVs, allDSVs;

    if(CPUDescCount_)
    {
        CPUDescriptorHeap_.initialize(
            device,
            CPUDescCount_ * frameCount,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            false);
        allCPUDescs = CPUDescriptorHeap_.allocRange(
            CPUDescCount_ * frameCount);
    }

    GPUDescAlloc_ = GPUAllocator;
    if(GPUDescCount_)
    {
        GPUDescRange_ = GPUDescAlloc_->allocStaticRange(
            GPUDescCount_ * frameCount);
        allGPUDescs = GPUDescRange_;
    }

    if(RTVCount_)
    {
        RTVDescriptorHeap_.initialize(
            device,
            RTVCount_ * frameCount,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            false);
        allRTVs = RTVDescriptorHeap_.allocRange(
            RTVCount_ * frameCount);
    }

    if(DSVCount_)
    {
        DSVDescriptorHeap_.initialize(
            device,
            DSVCount_ * frameCount,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            false);
        allDSVs = DSVDescriptorHeap_.allocRange(
            DSVCount_ * frameCount);
    }

    // fill slots

    slots_.resize(descSlotCount_);
    rangeSlots_.resize(descTableSlotCount_);

    uint32_t cpuDescOffset = 0, gpuDescOffset = 0, rtvOffset = 0, dsvOffset = 0;

    for(int ti = 0; ti < slotAllocator->getThreadCount(); ++ti)
    {
        auto &descs = slotAllocator->getDescriptorSlotRecords(ti);
        auto &descTables = slotAllocator->getDescriptorRangeSlotRecords(ti);

        for(auto &d : descs)
        {
            auto &slot = slots_[d.slot];

            bool isPerFrame = d.item->getInfo().resource->isPerFrame();
            if(d.item->getInfo().uavCounterResource)
                isPerFrame |= d.item->getInfo().uavCounterResource->isPerFrame();

            slot.resourceIndex = d.item->getInfo().resource->getIndex();
            slot.cpu           = d.item->isOnCPU();
            slot.gpu           = d.item->isOnGPU();
            slot.view          = d.item->getInfo().view;
            slot.isPerFrame    = isPerFrame;
            
            slot.uavCounterResourceIndex =
                d.item->getInfo().uavCounterResource ?
                d.item->getInfo().uavCounterResource->getIndex() : -1;

            if(isPerFrame)
            {
                slot.frames.resize(frameCount);
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
                    for(int i = 0; i < frameCount; ++i)
                        frame.freeCPUQueue.push(allRTVs[rtvOffset++]);
                }
                else if(slot.view.is<D3D12_DEPTH_STENCIL_VIEW_DESC>())
                {
                    for(int i = 0; i < frameCount; ++i)
                        frame.freeCPUQueue.push(allDSVs[dsvOffset++]);
                }
                else
                {
                    for(int i = 0; i < frameCount; ++i)
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

        for(auto &d : descTables)
        {
            auto &slot = rangeSlots_[d.slot];

            const uint32_t recordCount =
                static_cast<uint32_t>(d.table->getRecords().size());
            slot.resourceIndices.resize(recordCount);
            slot.uavCounterResourceIndices.resize(recordCount);
            slot.views.resize(recordCount);

            bool isRangePerFrame = false;
            for(uint32_t i = 0; i < recordCount; ++i)
            {
                slot.resourceIndices[i] = d.table->getRecords()[i].resource->getIndex();
                slot.views[i]           = d.table->getRecords()[i].view;

                slot.uavCounterResourceIndices[i] =
                    d.table->getRecords()[i].uavCounterResource ?
                    d.table->getRecords()[i].uavCounterResource->getIndex() : -1;

                if(d.table->getRecords()[i].resource->isPerFrame())
                    isRangePerFrame = true;

                if(d.table->getRecords()[i].uavCounterResource &&
                   d.table->getRecords()[i].uavCounterResource->isPerFrame())
                    isRangePerFrame = true;
            }

            slot.heapType   = viewToHeapType(d.table->getRecords().front().view);
            slot.cpu        = d.table->isOnCPU();
            slot.gpu        = d.table->isOnGPU();
            slot.isPerFrame = isRangePerFrame;
            
            if(isRangePerFrame)
            {
                slot.frames.resize(frameCount);
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
                    for(int i = 0; i < frameCount; ++i)
                    {
                        frame.freeCPUQueue.push(
                            allRTVs.getSubRange(rtvOffset, recordCount));
                        rtvOffset += recordCount;
                    }
                }
                else if(slot.heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
                {
                    for(int i = 0; i < frameCount; ++i)
                    {
                        frame.freeCPUQueue.push(
                            allDSVs.getSubRange(dsvOffset, recordCount));
                        dsvOffset += recordCount;
                    }
                }
                else
                {
                    for(int i = 0; i < frameCount; ++i)
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
                    if(slot.heapType != viewToHeapType(d.table->getRecords()[i].view))
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

DescriptorSlot *DescriptorSlotManager::getSlot(int slotIndex)
{
    return &slots_[slotIndex];
}

DescriptorRangeSlot *DescriptorSlotManager::getRangeSlot(int slotIndex)
{
    return &rangeSlots_[slotIndex];
}

ID3D12DescriptorHeap *DescriptorSlotManager::getGPUHeap() const
{
    return GPUDescAlloc_->getHeap();
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
