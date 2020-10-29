#pragma once

#include <map>

#include <agz/utility/graphics_api/d3d12/renderPass.h>
#include <agz/utility/graphics_api/d3d12/resourceManager.h>
#include <agz/utility/thread.h>

AGZ_D3D12_BEGIN

namespace rg
{

class RenderGraph : public misc::uncopyable_t
{
public:

    RenderGraph() = default;

    RenderGraph(RenderGraph &&other) noexcept;

    RenderGraph &operator=(RenderGraph &&other) noexcept;

    ~RenderGraph();

    void swap(RenderGraph &other) noexcept;

    void runAsync(int frameIndex);

    void sync();

    void setExternalResource(int index, ComPtr<ID3D12Resource> rsc);

    void setExternalResource(std::string_view name, ComPtr<ID3D12Resource> rsc);

    void clearExternalResources();

private:

    friend class RenderGraphBuilder;

    struct InternalResource
    {
        int idx;
        UniqueResource rsc;
    };

    struct ExternalResource
    {
        int idx;
        ComPtr<ID3D12Resource> rsc;
    };

    using Resource = misc::variant_t<
        InternalResource,
        ExternalResource>;

    struct SectionRecord
    {
        Section section;

        // should be submitted to which command queue
        int queueIdx = 0;

        // number of dependencies for submission (#OtherSections + MyCmdList)
        int targetDependencyCount = 0;

        // unfinished number of dependencies
        std::atomic<int> restDependencyCount = 0;

        // sections depending on this
        std::vector<SectionRecord*> outputs;

        // command lists used in each frame
        std::vector<ComPtr<ID3D12GraphicsCommandList>> cmdListPerFrame;

        // fence to wait for
        std::vector<ComPtr<ID3D12Fence>> waitFences;

        // fence to signal
        ComPtr<ID3D12Fence> signalFence;
    };

    struct PerThreadData
    {
        size_t sectionCount;
        std::unique_ptr<SectionRecord[]> sections;

        struct CommandListAllocator
        {
            ComPtr<ID3D12CommandAllocator> graphics;
            ComPtr<ID3D12CommandAllocator> compute;
        };

        // frame index -> command list allocator
        std::vector<CommandListAllocator> cmdAllocPerFrame;
    };

    struct Sync
    {
        std::mutex                 mutex;
        std::condition_variable    cond;
        thread::count_down_latch_t latch;
    };

    void increaseNumOfFinishedDependency(SectionRecord *section);

    void workingFunc(int threadIndex);

    void workingThread(int threadIndex);

    std::unique_ptr<Sync> sync_;
    int                   frameIndex_    = 0;
    int                   threadTrigger_ = -1;
    UINT64                newFenceValue_ = 0;

    std::vector<std::thread> threads_;

    std::vector<PerThreadData> perThreadData_;

    std::vector<Resource>                   rscs_;
    std::vector<ID3D12Resource*>            rawRscs_;
    std::map<std::string, int, std::less<>> name2Rsc_;

    std::vector<ComPtr<ID3D12CommandQueue>> cmdQueues_;
};

} // namespace rg

AGZ_D3D12_END
