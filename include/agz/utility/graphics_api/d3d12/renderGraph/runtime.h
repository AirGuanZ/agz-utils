#pragma once

#include <vector>

#include <agz/utility/graphics_api/d3d12/resourceManager.h>
#include <agz/utility/graphics_api/d3d12/renderGraph/section.h>
#include <agz/utility/thread.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

struct DescriptorSlot
{
    int          resourceIndex = 0;
    bool         isDirty       = false;
    ResourceView view;
    Descriptor   descriptor;
};

class Runtime : public misc::uncopyable_t
{
public:

    ~Runtime();

    void runAsync(int frameIndex);

    void sync();

    void run(int frameIndex);

    ID3D12Resource *getRawResource(Resource *resource) noexcept;

    ID3D12Resource *getRawResource(int index) noexcept;

    void setExternalResource(
        ExternalResource *node, ComPtr<ID3D12Resource> rsc);

    void clearExternalResources();

private:

    void reset();

    void refreshDescriptorSlot(int threadIndex, DescriptorSlot &slot);

    friend class Compiler;

    struct RuntimeInternalResource
    {
        int              index;
        UniqueResource   resource;
        std::vector<int> descriptorSlots;
    };

    struct RuntimeExternalResource
    {
        int                    index;
        ComPtr<ID3D12Resource> resource;
        std::vector<int>       descriptorSlots;
    };

    using RuntimeResource = misc::variant_t<
        RuntimeInternalResource, RuntimeExternalResource>;

    struct PerThreadData
    {
        struct CommandAllocator
        {
            ComPtr<ID3D12CommandAllocator> graphics;
            ComPtr<ID3D12CommandAllocator> compute;
        };

        std::vector<CommandAllocator> perFrameCmdAlloc;

        size_t                     sectionCount = 0;
        std::unique_ptr<Section[]> sections;
        
        std::vector<int> descriptorSlots;

        DescriptorQueue GPUDescQueue;
        DescriptorQueue RTVDescQueue;
        DescriptorQueue DSVDescQueue;
    };

    struct Sync
    {
        std::mutex                 mutex;
        std::condition_variable    cond;
        thread::count_down_latch_t latch;
    };

    void iterationFunc(int threadIndex);

    void threadEntry(int threadIndex);

    ID3D12Device *device_ = nullptr;

    std::vector<ComPtr<ID3D12CommandQueue>> queues_;

    std::vector<RuntimeResource> resources_;
    std::vector<DescriptorSlot>  descriptorSlots_;


    std::unique_ptr<PerThreadData[]> perThreadData_;
    std::vector<std::thread>         threads_;

    Sync sync_;

    int    frameIndex_    = 0;
    int    threadTrigger_ = -1;
    UINT64 fenceValue_    = 0;

    DescriptorAllocator *GPUDescAlloc_ = nullptr;
    DescriptorRange      GPUDescRange_;

    DescriptorHeap RTVDescriptorHeap_;
    DescriptorHeap DSVDescriptorHeap_;
};

AGZ_D3D12_RENDERGRAPH_END
