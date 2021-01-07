#pragma once

#include <set>

#include <agz-utils/graphics_api/d3d12/renderGraph/runtime.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

class Resource : public misc::uncopyable_t
{
public:

    Resource(std::string name, int index) noexcept;

    virtual ~Resource() = default;

    const std::string &getName() const noexcept;

    int getIndex() const noexcept;

    bool isInternal() const noexcept;

    bool isExternal() const noexcept;

    virtual ExternalResource *asExternal() noexcept;

    virtual InternalResource *asInternal() noexcept;

    virtual const ExternalResource *asExternal() const noexcept;

    virtual const InternalResource *asInternal() const noexcept;

    void setDescription(const D3D12_RESOURCE_DESC &desc) noexcept;

    const D3D12_RESOURCE_DESC *getDescription() const noexcept;

    D3D12_RESOURCE_DESC *getDescription() noexcept;

private:

    std::string name_;
    int         index_;

    D3D12_RESOURCE_DESC desc_;
};

class InternalResource : public Resource
{
public:

    InternalResource(std::string name, int index) noexcept;

    void setClearValue(const D3D12_CLEAR_VALUE &clearValue);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setHeapType(D3D12_HEAP_TYPE heapType);

    InternalResource *asInternal() noexcept override;

    const InternalResource *asInternal() const noexcept override;

private:

    friend class Compiler;

    D3D12_HEAP_TYPE heapType_;

    bool                clear_;
    D3D12_CLEAR_VALUE   clearValue_;

    D3D12_RESOURCE_STATES initialState_;
};

class ExternalResource : public Resource
{
public:

    ExternalResource(std::string name, int index) noexcept;

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setFinalState(D3D12_RESOURCE_STATES state);

    ExternalResource *asExternal() noexcept override;

    const ExternalResource *asExternal() const noexcept override;

private:

    friend class Compiler;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

class Vertex : public misc::uncopyable_t
{
public:

    Vertex(std::string name, int index, int thread, int queue);

    const std::string &getName() const noexcept;

    int getIndex() const noexcept;

    void setQueue(int queue);

    void setThread(int thread);

    void setCallback(PassCallback callback);

    void useResource(
        const Resource       *resource,
        D3D12_RESOURCE_STATES state,
        ResourceView          view = {});

private:

    friend class Compiler;

    struct ResourceUsage
    {
        const Resource       *resource = nullptr;
        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
        ResourceView          view = {};
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

class Compiler : public misc::uncopyable_t
{
public:

    Compiler();

    void setThreadCount(int count);

    void setQueueCount(int count);

    void setFrameCount(int count);

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Vertex *addVertex(std::string name, int thread = 0, int queue = 0);

    void addArc(Vertex *head, Vertex *tail);

    void compile(
        ID3D12Device                           *device,
        ResourceManager                        &rscMgr,
        DescriptorAllocator                    &GPUDescAlloc,
        std::vector<ComPtr<ID3D12CommandQueue>> queues,
        Runtime                                &runtime);

private:

    struct PassTemp
    {
        bool shouldSubmit = false;
        int section       = 0;

        std::vector<Pass::ResourceTransition> resourceTransitions;

        struct ContextUsage
        {
            int resource       = 0;
            int descriptorSlot = 0;
        };

        std::vector<ContextUsage> contextUsages;
    };

    struct SectionTemp
    {
        std::vector<int> passes;
        int              thread;
        int              indexInThread;

        int                              externalDependencies = 0;
        std::vector<int>                 outputs;
        std::vector<ComPtr<ID3D12Fence>> waitFences;
        ComPtr<ID3D12Fence>              signalFence;
    };

    struct ThreadTemp
    {
        std::vector<int> passes;
        std::vector<int> sections;

        struct DescriptorRecord
        {
            Resource    *resource       = nullptr;
            ResourceView view           = {};
            int          desceiptorSlot = 0;
        };

        std::vector<DescriptorRecord> GPUDescRecords;
        std::vector<DescriptorRecord> RTVRecords;
        std::vector<DescriptorRecord> DSVRecords;
    };

    struct Temps
    {
        std::vector<PassTemp>    passes;
        std::vector<SectionTemp> sections;
        std::vector<ThreadTemp>  threads;
    };

    std::vector<int> sortPasses() const;

    Temps assignSectionsToThreads() const;

    void generateSectionDependencies(ID3D12Device *device, Temps &temps) const;

    void generateResourceTransitions(Temps &temps);

    void generateDescriptorRecords(Temps &temps);

    void fillRuntimeResources(
        Runtime         &runtime,
        ResourceManager &rscMgr,
        Temps           &temps) const;

    void fillRuntimeDescriptors(
        ID3D12Device        *device,
        Runtime             &runtime,
        DescriptorAllocator &GPUDescAlloc,
        Temps               &temps) const;

    void fillRuntimeSections(
        ID3D12Device *device,
        Runtime      &runtime,
        Temps        &temps) const;

    void fillRuntimeCommandAllocators(
        ID3D12Device *device,
        Runtime      &runtime) const;

    int threadCount_;
    int queueCount_;
    int frameCount_;

    std::vector<std::unique_ptr<Resource>> resources_;
    std::vector<std::unique_ptr<Vertex>>   vertices_;
};

AGZ_D3D12_RENDERGRAPH_END
