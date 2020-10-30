#pragma once

#include <map>

#include <agz/utility/graphics_api/d3d12/resourceManager.h>
#include <agz/utility/thread.h>

AGZ_D3D12_BEGIN

namespace rg
{

class PassContext
{
public:

    PassContext(
        ID3D12GraphicsCommandList *cmdList,
        ID3D12Resource *const     *rscs) noexcept
        : cmdList_(cmdList), rscs_(rscs) { }

    ID3D12GraphicsCommandList *getCommandList() noexcept { return cmdList_; }

    ID3D12Resource *getResource(int index) noexcept { return rscs_[index]; }

    ID3D12GraphicsCommandList *operator->() noexcept { return cmdList_; }

private:

    ID3D12GraphicsCommandList *cmdList_;

    ID3D12Resource *const *rscs_;
};

class Pass : public misc::uncopyable_t
{
public:

    using Callback = std::function<void(PassContext&)>;

    Pass() = default;

    Pass(Pass &&other) noexcept = default;

    Pass &operator=(Pass &&other) noexcept = default;

    void setCallback(std::shared_ptr<Callback> callback) noexcept;

    void addResourceStateTransition(
        int                   idx,
        D3D12_RESOURCE_STATES beg,
        D3D12_RESOURCE_STATES mid,
        D3D12_RESOURCE_STATES end);

    void execute(
        ID3D12GraphicsCommandList *cmdList,
        ID3D12Resource    * const *rscs) const;

private:

    friend class RenderGraphBuilder;

    struct ResourceStateTransition
    {
        int idx;

        D3D12_RESOURCE_STATES beg;
        D3D12_RESOURCE_STATES mid;
        D3D12_RESOURCE_STATES end;
    };

    std::shared_ptr<Callback> callback_;

    std::vector<ResourceStateTransition> rscStateTransitions_;
};

class Section : public misc::uncopyable_t
{
public:

    Section() = default;

    Section(Section &&other) noexcept = default;

    Section &operator=(Section &&other) noexcept = default;

    void addPass(Pass pass);

    void execute(
        ID3D12GraphicsCommandList *cmdList,
        ID3D12Resource     *const *rscs) const;

private:

    std::vector<Pass> passes_;
};

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
