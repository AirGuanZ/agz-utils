#pragma once

#include <queue>

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorSlotAssigner.h>
#include <agz-utils/graphics_api/d3d12/descriptorAllocator.h>

AGZ_D3D12_GRAPH_BEGIN

struct DescriptorSlot
{
    int resourceIndex           = 0;
    int uavCounterResourceIndex = 0;

    bool cpu = false;
    bool gpu = false;
    ResourceView view;

    struct PerFrame
    {
        bool       isDirty = false;
        Descriptor cpu;
        Descriptor gpu;

        std::queue<Descriptor> freeCPUQueue;
        std::queue<Descriptor> freeGPUQueue;
    };

    bool                  isPerFrame = false;
    std::vector<PerFrame> frames;
};

struct DescriptorRangeSlot
{
    std::vector<int> resourceIndices;
    std::vector<int> uavCounterResourceIndices;

    D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    
    bool cpu = false;
    bool gpu = false;
    std::vector<ResourceView> views;

    struct PerFrame
    {
        bool            isDirty = false;
        DescriptorRange cpu;
        DescriptorRange gpu;

        std::queue<DescriptorRange> freeCPUQueue;
        std::queue<DescriptorRange> freeGPUQueue;
    };

    bool                  isPerFrame = false;
    std::vector<PerFrame> frames;
};

class DescriptorSlotManager : public misc::uncopyable_t
{
public:

    ~DescriptorSlotManager();

    void reset();

    void addDescriptor(const DescriptorItem *item);

    void addDescriptorTable(const DescriptorTable *table);

    void initializeDescriptorSlots(
        ID3D12Device            *device,
        int                      frameCount,
        DescriptorAllocator     *GPUAllocator,
        DescriptorSlotAssigner *slotAllocator);

    DescriptorSlot *getSlot(int slotIndex);

    DescriptorRangeSlot *getRangeSlot(int slotIndex);

    ID3D12DescriptorHeap *getGPUHeap() const;

private:

    int CPUDescCount_ = 0;
    int GPUDescCount_ = 0;
    int RTVCount_     = 0;
    int DSVCount_     = 0;

    int descSlotCount_      = 0;
    int descTableSlotCount_ = 0;

    DescriptorAllocator *GPUDescAlloc_ = nullptr;
    DescriptorRange      GPUDescRange_;

    DescriptorHeap CPUDescriptorHeap_;
    DescriptorHeap RTVDescriptorHeap_;
    DescriptorHeap DSVDescriptorHeap_;

    std::vector<DescriptorSlot>      slots_;
    std::vector<DescriptorRangeSlot> rangeSlots_;
};

AGZ_D3D12_GRAPH_END
