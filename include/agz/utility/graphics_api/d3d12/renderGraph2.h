#pragma once

#include <map>
#include <set>

#include <agz/utility/graphics_api/d3d12/descriptorAllocator.h>
#include <agz/utility/graphics_api/d3d12/resourceManager.h>
#include <agz/utility/thread.h>

AGZ_D3D12_BEGIN

class ExternalResource;
class InternalResource;
class PassContext;
class RenderGraph;
class Resource;

using PassCallback = std::function<void(PassContext &)>;

using ResourceView = misc::variant_t<
    std::monostate,
    D3D12_SHADER_RESOURCE_VIEW_DESC,
    D3D12_UNORDERED_ACCESS_VIEW_DESC,
    D3D12_RENDER_TARGET_VIEW_DESC,
    D3D12_DEPTH_STENCIL_VIEW_DESC>;

struct TransientDescriptorAllocatorSet
{
    TransientDescriptorAllocator CBV_SRV_UAV;
    TransientDescriptorAllocator Sampler;
    TransientDescriptorAllocator RTV;
    TransientDescriptorAllocator DSV;
};

struct DescriptorAllocatorSet
{
    DescriptorAllocator *CBV_SRV_UAV = nullptr;
    DescriptorAllocator *Sampler     = nullptr;
    DescriptorAllocator *RTV         = nullptr;
    DescriptorAllocator *DSV         = nullptr;
};

class PassContext
{
public:

    struct ResourceRecord
    {
        ID3D12Resource *resource   = nullptr;
        Descriptor      descriptor = {};
    };

    PassContext(
        int                          frameIndex,
        ID3D12GraphicsCommandList   *cmdList,
        const ResourceRecord        *resources);

    int getFrameIndex() const noexcept;

    const ID3D12Resource *getResource(int resourceIndex) const noexcept;

    const ID3D12Resource *getResource(Resource *resource) const noexcept;

    Descriptor getDescriptor(int resourceIndex) const noexcept;

    Descriptor getDescriptor(Resource *resource) const noexcept;

    ID3D12GraphicsCommandList *getCommandList() noexcept;

    ID3D12GraphicsCommandList *operator->() noexcept;

private:

    int                          frameIndex_;
    ID3D12GraphicsCommandList   *cmdList_;
    const ResourceRecord        *resources_;
};

class Resource
{
public:

    Resource(RenderGraph *graph, std::string name, int index);

    virtual ~Resource() = default;

    int getIndex() const noexcept;

    const std::string &getName() const noexcept;

    RenderGraph *getGraph() const noexcept;

    ID3D12Resource *getRawResource() const noexcept;

    virtual const InternalResource *asInternal() const noexcept;

    virtual const ExternalResource *asExternal() const noexcept;

    virtual InternalResource *asInternal() noexcept;

    virtual ExternalResource *asExternal() noexcept;

private:

    RenderGraph *graph_;
    std::string  name_;
    int          index_;
};

class InternalResource : public Resource
{
public:

    InternalResource(RenderGraph *graph, std::string name, int index);

    void setResourceDescription(const D3D12_RESOURCE_DESC &desc);

    void setClearValue(const D3D12_CLEAR_VALUE &clearValue);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setHeapType(D3D12_HEAP_TYPE heapType);

    const InternalResource *asInternal() const noexcept override;

    InternalResource *asInternal() noexcept override;

private:

    friend class RenderGraph;

    D3D12_HEAP_TYPE heapType_;

    D3D12_RESOURCE_DESC desc_;
    bool                clear_;
    D3D12_CLEAR_VALUE   clearValue_;

    D3D12_RESOURCE_STATES initialState_;
};

class ExternalResource : public Resource
{
public:

    ExternalResource(RenderGraph *graph, std::string name, int index);

    void setResource(ComPtr<ID3D12Resource> rsc);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setFinalState(D3D12_RESOURCE_STATES state);

    const ExternalResource *asExternal() const noexcept override;

    ExternalResource *asExternal() noexcept override;

private:

    friend class RenderGraph;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

class Vertex : public misc::uncopyable_t
{
public:

    Vertex(std::string name, int index, int thread, int queue);

    const std::string &getName() const noexcept;

    int getIndex() const noexcept;

    void setQueue(int queueIndex);

    void setThread(int threadIndex);

    void setCallback(PassCallback callback);

    void useResource(Resource *rsc, D3D12_RESOURCE_STATES state);

    void useShaderResource(
        Resource                              *rsc,
        D3D12_RESOURCE_STATES                  state,
        const D3D12_SHADER_RESOURCE_VIEW_DESC &SRVDesc);

    void useUnorderedAccess(
        Resource                               *rsc,
        D3D12_RESOURCE_STATES                   state,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC &UAVDesc);

    void useRenderTarget(
        Resource                            *rsc,
        D3D12_RESOURCE_STATES                state,
        const D3D12_RENDER_TARGET_VIEW_DESC &RTVDesc);

    void useDepthStencil(
        Resource                            *rsc,
        D3D12_RESOURCE_STATES                state,
        const D3D12_DEPTH_STENCIL_VIEW_DESC &DSVDesc);

private:

    friend class RenderGraph;

    struct ResourceUsage
    {
        Resource             *resource = nullptr;
        D3D12_RESOURCE_STATES state    = D3D12_RESOURCE_STATE_COMMON;
        ResourceView          view     = {};
    };

    std::string name_;
    int         index_;

    int thread_;
    int queue_;

    std::shared_ptr<PassCallback> callback_;

    std::set<Vertex *> in_;
    std::set<Vertex *> out_;

    std::vector<ResourceUsage> resourceUsages_;
};

class Pass : public misc::uncopyable_t
{
public:

    struct ResourceUsage
    {
        Resource *resource = nullptr;

        D3D12_RESOURCE_STATES beg = D3D12_RESOURCE_STATE_COMMON;
        D3D12_RESOURCE_STATES mid = D3D12_RESOURCE_STATE_COMMON;
        D3D12_RESOURCE_STATES end = D3D12_RESOURCE_STATE_COMMON;

        ResourceView view;
    };

    Pass(
        RenderGraph                  *graph,
        std::shared_ptr<PassCallback> callback,
        std::vector<ResourceUsage>    usages);

    Pass(Pass &&other) noexcept;

    Pass &operator=(Pass &&other) noexcept;

    void execute(
        ID3D12Device                    *device,
        TransientDescriptorAllocatorSet &descAlloc,
        int                              frameIndex,
        ID3D12GraphicsCommandList       *cmdList);

private:

    void swap(Pass &other) noexcept;

    RenderGraph *graph_;

    std::shared_ptr<PassCallback> callback_;

    std::vector<ResourceUsage>               resourceUsages_;
    std::vector<PassContext::ResourceRecord> resourceRecords_;

    std::vector<D3D12_RESOURCE_BARRIER> beforeBarriers_;
    std::vector<D3D12_RESOURCE_BARRIER> afterBarriers_;
};

class Section : public misc::uncopyable_t
{
public:

    void setQueue(int queue);

    void initializeCommandList(
        ID3D12Device           *device,
        int                     frameCount,
        ID3D12CommandAllocator *cmdAlloc,
        D3D12_COMMAND_LIST_TYPE cmdListType);

    void addPass(Pass pass);

    void addOut(Section *out);

    void addWaitFence(ComPtr<ID3D12Fence> fence);

    void addSignalFence(ComPtr<ID3D12Fence> fence);

    void execute(
        ID3D12Device                    *device,
        UINT64                           fenceValue,
        TransientDescriptorAllocatorSet &descAlloc,
        int                              frameIndex,
        ID3D12CommandAllocator          *cmdAlloc,
        ComPtr<ID3D12CommandQueue>      *queues);

    void increaseNumebrOfAllDependencies();

    void resetNumberOfUnfinishedDependencies();

    void increaseNumberOfFinishedDependencies(
        int                         frameIndex,
        ComPtr<ID3D12CommandQueue> *queues);

    D3D12_COMMAND_LIST_TYPE getCommandListType() const noexcept;

private:

    int queue_ = 0;

    D3D12_COMMAND_LIST_TYPE cmdListType_ = {};
    std::vector<ComPtr<ID3D12GraphicsCommandList>> frame2CmdList_;

    std::vector<ComPtr<ID3D12Fence>> waitFences_;
    std::vector<ComPtr<ID3D12Fence>> signalFences_;

    std::vector<Pass>      passes_;
    std::vector<Section *> out_;

    int              allDependencies_        = 0;
    std::atomic<int> unfinishedDependencies_ = 0;
};

class RenderGraph : public misc::uncopyable_t
{
public:

    RenderGraph(
        ID3D12Device        *device,
        ResourceManager     &rscMgr,
        DescriptorAllocator &viewDescAlloc,
        DescriptorAllocator &samplerAlloc);

    ~RenderGraph();

    // compile

    void setRTVAndDSVHeapSize(size_t RTVSize, size_t DSVSize);

    void setThreadCount(int count);

    void setQueueCount(int count);

    void setFrameCount(int count);

    void setQueue(int index, ComPtr<ID3D12CommandQueue> queue);

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Vertex *addVertex(std::string name, int thread = 0, int queue = 0);

    void addArc(Vertex *head, Vertex *tail);

    int getResourceCount() const noexcept;

    int getVertexCount() const noexcept;

    void compile();

    // run

    void setExternalResource(
        ExternalResource *rscNode, ComPtr<ID3D12Resource> rsc);

    void clearExternalResources();

    ID3D12Resource *getRawResource(int index);

    void runAsync(int frameIndex);

    void sync();

    void run(int frameIndex);

private:

    ID3D12Device           *device_;
    ResourceManager        &rscMgr_;

    DescriptorAllocator &viewDescAlloc_;
    DescriptorAllocator &samplerAlloc_;

    size_t RTVAllocSize_;
    size_t DSVAllocSize_;

    std::unique_ptr<DescriptorAllocator> RTVAlloc_;
    std::unique_ptr<DescriptorAllocator> DSVAlloc_;

    // compile
    
    struct ResourceTemp
    {
        struct Usage
        {
            int                     pass;
            D3D12_RESOURCE_STATES   state;
            ResourceView            view;
        };

        std::vector<Usage> usages;
    };

    struct PassTemp
    {
        std::vector<Pass::ResourceUsage> usages;
        std::vector<ComPtr<ID3D12Fence>> waitFences;
        ComPtr<ID3D12Fence>              signalFence;

        int sectionIndex = 0;
        bool mustSubmit  = false;
    };

    struct SectionTemp
    {
        std::vector<int> passes;
        int              thread;
        int              queue;
        int              indexInThread;
    };

    struct ThreadTemp
    {
        std::vector<int> passes;
        std::vector<int> sections;
    };

    std::function<D3D12_COMMAND_LIST_TYPE(Vertex *)> getQueueTypeMapping() const;

    std::vector<int> sortPasses() const;

    void checkThreadAndQueueIndices() const;

    std::vector<ResourceTemp> collectResourceUsages(
        const std::vector<int> &linearPasses) const;

    // fill passTemps[i].usages
    void generateResourceUsagesForPasses(
        const std::vector<ResourceTemp> &resourceTemps,
        std::vector<PassTemp>           &passTemps) const;

    // fill passTemps[i].waitFences/signalFence/mustSubmit
    void generateFencesAndSubmissionFlags(
        const std::vector<ThreadTemp> &threadTemps,
        std::vector<PassTemp>         &passTemps) const;

    // fill passTemps[i].sectionIndex and threadTemps[i].sections
    std::vector<SectionTemp> generateSectionTemps(
        std::vector<PassTemp>   &passTemps,
        std::vector<ThreadTemp> &threadTemps) const;

    int threadCount_;
    int queueCount_;
    int frameCount_;

    std::vector<std::unique_ptr<Resource>>  resources_;
    std::vector<std::unique_ptr<Vertex>>    vertices_;
    std::vector<ComPtr<ID3D12CommandQueue>> queues_;

    // run

    void workingFunc(int threadIndex);

    void workingThread(int threadIndex);

    struct RuntimeInternalResource
    {
        int index = 0;
        UniqueResource resource;
    };

    struct RuntimeExternalResource
    {
        int index = 0;
        ComPtr<ID3D12Resource> resource;
    };

    using RuntimeResource = misc::variant_t<
        RuntimeInternalResource,
        RuntimeExternalResource>;

    struct PerThread
    {
        struct CommandAllocator
        {
            ComPtr<ID3D12CommandAllocator> graphics;
            ComPtr<ID3D12CommandAllocator> compute;
        };

        size_t                     sectionCount = 0;
        std::unique_ptr<Section[]> sections;

        std::vector<CommandAllocator> frame2CmdAlloc;
    };

    std::mutex                 mutex_;
    std::condition_variable    cond_;
    thread::count_down_latch_t latch_;

    std::vector<std::thread> threads_;

    int    frameIndex_;
    int    threadTrigger_;
    UINT64 fenceValue_;

    std::vector<PerThread>       perThread_;
    std::vector<RuntimeResource> runtimeResources_;
};

AGZ_D3D12_END
