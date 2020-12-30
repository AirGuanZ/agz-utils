#pragma once

#include <queue>

#include <agz/utility/graphics_api/d3d12/graph/sectionRuntime.h>
#include <agz/utility/graphics_api/d3d12/uniqueResource.h>
#include <agz/utility/thread.h>

AGZ_D3D12_GRAPH_BEGIN

struct DescriptorSlot
{
    int resourceIndex = 0;

    bool cpu = false;
    bool gpu = false;
    ResourceView view;

    bool isDirty = false;
    Descriptor cpuDescriptor;
    Descriptor gpuDescriptor;

    std::queue<Descriptor> freeCPUDescriptorQueue;
    std::queue<Descriptor> freeGPUDescriptorQueue;
};

struct DescriptorRangeSlot
{
    std::vector<int> resourceIndices;

    D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    
    bool cpu = false;
    bool gpu = false;
    std::vector<ResourceView> views;
    
    bool isDirty = false;
    DescriptorRange cpuDescriptorRange;
    DescriptorRange gpuDescriptorRange;

    std::queue<DescriptorRange> freeCPUDescriptorRangeQueue;
    std::queue<DescriptorRange> freeGPUDescriptorRangeQueue;
};

class GraphRuntime : public misc::uncopyable_t
{
public:

    ~GraphRuntime();

    void runAsync(int frameIndex);

    void sync();

    void run(int frameIndex);

    ID3D12Resource *getRawResource(const Resource *resource);

    ID3D12Resource *getRawResource(int resourceIndex);

    void setSamplerHeap(ComPtr<ID3D12DescriptorHeap> heap);

    void setExternalResource(
        ExternalResource *node, ComPtr<ID3D12Resource> resource);

    void clearExternalResources();

private:

    friend class GraphCompiler;

    void reset();

    void refreshDescriptor(DescriptorSlot &slot);

    void refreshDescriptorRange(DescriptorRangeSlot &slot);

    struct ResourceRuntimeCommon
    {
        int              index = -1;
        std::vector<int> descriptorSlots;
        std::vector<int> descriptorRangeSlots;
    };

    struct InternalResourceRuntime : ResourceRuntimeCommon
    {
        InternalResourceRuntime() = default;
        InternalResourceRuntime(
            InternalResourceRuntime &&other) noexcept = default;
        InternalResourceRuntime &operator=(
            InternalResourceRuntime &&) noexcept = default;

        UniqueResource resource;
    };

    struct ExternalResourceRuntime : ResourceRuntimeCommon
    {
        ExternalResourceRuntime() = default;
        ExternalResourceRuntime(
            ExternalResourceRuntime &&other) noexcept = default;
        ExternalResourceRuntime &operator=(
            ExternalResourceRuntime &&) noexcept = default;

        ComPtr<ID3D12Resource> resource;
    };

    using ResourceRuntime = misc::variant_t<
        InternalResourceRuntime, ExternalResourceRuntime>;

    struct PerThreadData
    {
        struct CommandAllocator
        {
            ComPtr<ID3D12CommandAllocator> graphics;
            ComPtr<ID3D12CommandAllocator> compute;
        };

        std::vector<CommandAllocator> perFrameCmdAlloc;

        size_t                            sectionCount = 0;
        std::unique_ptr<SectionRuntime[]> sections;

        std::vector<int> descriptorSlots;
        std::vector<int> descriptorRangeSlots;
    };

    struct Sync
    {
        std::mutex                 mutex;
        std::condition_variable    cond;
        thread::count_down_latch_t latch;
    };

    void frameFunc(int threadIndex);

    void threadEntry(int threadIndex);

    ID3D12Device *device_ = nullptr;

    std::vector<ComPtr<ID3D12CommandQueue>> queues_;

    std::vector<ResourceRuntime>     resources_;
    std::vector<DescriptorSlot>      descriptorSlots_;
    std::vector<DescriptorRangeSlot> descriptorRangeSlots_;

    std::unique_ptr<PerThreadData[]> perThreadData_;
    std::vector<std::thread>         threads_;

    Sync sync_;

    int frameIndex_    = 0;
    int threadTrigger_ = -1;
    UINT64 fenceValue_ = 0;

    DescriptorAllocator *GPUDescAlloc_ = nullptr;
    DescriptorRange      GPUDescRange_;

    ComPtr<ID3D12DescriptorHeap> samplerHeap_;

    DescriptorHeap CPUDescriptorHeap_;
    DescriptorHeap RTVDescriptorHeap_;
    DescriptorHeap DSVDescriptorHeap_;
};

AGZ_D3D12_GRAPH_END
