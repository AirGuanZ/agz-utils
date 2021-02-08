#pragma once

#include <queue>

#include <agz-utils/graphics_api/d3d12/graph/runtime/descriptorSlotManager.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/section.h>
#include <agz-utils/graphics_api/d3d12/uniqueResource.h>
#include <agz-utils/thread.h>

AGZ_D3D12_GRAPH_BEGIN

class Runtime : public misc::uncopyable_t
{
public:

    ~Runtime();

    void runAsync(int frameIndex);

    void sync();

    void run(int frameIndex);

    ID3D12Resource *getRawResource(const Resource *resource);

    ID3D12Resource *getRawResource(int resourceIndex);

    void setSamplerHeap(ComPtr<ID3D12DescriptorHeap> heap);

    void setExternalResource(
        ExternalResource *node, ID3D12Resource *resource);

    void setExternalResource(
        ExternalResource *node, int frameIndex, ID3D12Resource *resource);

    void clearExternalResources();

private:

    friend class Compiler;

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

        bool                        isPerFrame = false;
        std::vector<UniqueResource> resource;
    };

    struct ExternalResourceRuntime : ResourceRuntimeCommon
    {
        ExternalResourceRuntime() = default;
        ExternalResourceRuntime(
            ExternalResourceRuntime &&other) noexcept = default;
        ExternalResourceRuntime &operator=(
            ExternalResourceRuntime &&) noexcept = default;

        bool                         isPerFrame = false;
        std::vector<ID3D12Resource*> resource;
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

        std::vector<SectionRuntime> sections;

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

    std::vector<ResourceRuntime> resources_;

    std::vector<PerThreadData> perThreadData_;
    std::vector<std::thread>   threads_;

    Sync sync_;

    int frameIndex_    = 0;
    int threadTrigger_ = -1;
    UINT64 fenceValue_ = 0;

    ComPtr<ID3D12DescriptorHeap> samplerHeap_;

    DescriptorSlotManager descSlotMgr_;
};

AGZ_D3D12_GRAPH_END
