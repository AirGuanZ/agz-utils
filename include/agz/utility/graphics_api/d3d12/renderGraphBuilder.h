#pragma once

#include <set>

#include <agz/utility/graphics_api/d3d12/renderGraph.h>

AGZ_D3D12_BEGIN

namespace rg
{

class InternalResource;
class ExternalResource;

class Resource
{
protected:

    std::string name_;

    int index_;

public:

    Resource(std::string name, int index) noexcept;

    virtual ~Resource() = default;

    int getIndex() const noexcept;

    virtual const InternalResource *asInternal() const noexcept;

    virtual const ExternalResource *asExternal() const noexcept;

    virtual InternalResource *asInternal() noexcept;

    virtual ExternalResource *asExternal() noexcept;
};

class InternalResource : public Resource
{
public:

    InternalResource(std::string name, int index);

    void setDescription(const D3D12_RESOURCE_DESC &desc);

    void setClearValue(const D3D12_CLEAR_VALUE &clearValue);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setHeapType(D3D12_HEAP_TYPE heapType);

    const InternalResource *asInternal() const noexcept override;

    InternalResource *asInternal() noexcept override;

private:

    friend class RenderGraphBuilder;

    D3D12_HEAP_TYPE heapType_;

    D3D12_RESOURCE_DESC desc_;

    bool              clear_;
    D3D12_CLEAR_VALUE clearValue_;

    D3D12_RESOURCE_STATES initialState_;
};

class ExternalResource : public Resource
{
public:

    explicit ExternalResource(std::string name, int index);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setFinalState(D3D12_RESOURCE_STATES state);

    const ExternalResource *asExternal() const noexcept override;

    ExternalResource *asExternal() noexcept override;

private:

    friend class RenderGraphBuilder;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

class Vertex : public misc::uncopyable_t
{
public:

    using Callback = Pass::Callback;

    Vertex(std::string name, int index);

    virtual ~Vertex() = default;

    void useResource(Resource *rsc, D3D12_RESOURCE_STATES state);

    void setQueue(int index);

    void setThread(int index);

    void setCallback(Callback callback);

private:

    friend class RenderGraphBuilder;

    struct ResourceUsage
    {
        Resource             *rsc;
        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
    };

    std::string name_;
    int         index_;

    int threadIndex_;
    int queueIndex_;

    std::set<Vertex *> in_;
    std::set<Vertex *> out_;

    std::vector<ResourceUsage> rscUsages_;

    std::shared_ptr<Callback> callback_;
};

class RenderGraphBuilder
{
public:

    RenderGraphBuilder();

    void setThreadCount(int count);

    void setQueueCount (int count);

    void setFrameCount(int count);

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Vertex *addVertex(std::string name, int threadIndex = 0, int queueIndex = 0);

    void addArc(Vertex *head, Vertex *tail);

    std::unique_ptr<RenderGraph> build(
        ID3D12Device                           *device,
        std::vector<ComPtr<ID3D12CommandQueue>> cmdQueues,
        ResourceManager                        &rscMgr) const;
    
private:

    using Resource = misc::variant_t<InternalResource, ExternalResource>;

    struct ResourceRecord
    {
        struct Usage
        {
            int                     pass;
            D3D12_RESOURCE_STATES   state;
            D3D12_COMMAND_LIST_TYPE cmdListType;
        };

        std::vector<Usage> usages;
    };

    struct PassRecord
    {
        std::vector<Pass::ResourceStateTransition> transitions;

        std::vector<ComPtr<ID3D12Fence>> waitFences;
        ComPtr<ID3D12Fence>              signalFence;

        int sectionIndex = 0;
        bool mustSubmit  = false;
    };

    struct SectionRecord
    {
        std::vector<int> passes;
        int              threadIndex;
        int              queueIndex;
        int              indexInThread;
    };

    struct ThreadRecord
    {
        std::vector<int> passes;
        std::vector<int> sections;
    };

    std::vector<int> topologySortPasses() const;

    int threadCount_;
    int queueCount_;
    int frameCount_;

    std::vector<std::unique_ptr<Resource>> rscs_;
    std::vector<std::unique_ptr<Vertex>>   vtxs_;
};

} // namespace rg

AGZ_D3D12_END
