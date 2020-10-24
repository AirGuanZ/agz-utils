#pragma once

#include <set>

#include <agz/utility/graphics_api/d3d12/renderGraph.h>

AGZ_D3D12_BEGIN

namespace rg
{

class InternalResource : public misc::uncopyable_t
{
public:

    InternalResource(std::string name, int index);

    void setDescription(const D3D12_RESOURCE_DESC &desc);

    void setClearValue(const D3D12_CLEAR_VALUE &clearValue);

    void setInitialState(D3D12_RESOURCE_STATES state);

private:

    friend class RenderGraphBuilder;

    std::string name_;
    int         index_;

    D3D12_RESOURCE_DESC desc_;

    bool              clear_;
    D3D12_CLEAR_VALUE clearValue_;

    D3D12_RESOURCE_STATES initialState_;
};

class ExternalResource : public misc::uncopyable_t
{
public:

    explicit ExternalResource(std::string name, int index);

    void setInitialState(D3D12_RESOURCE_STATES state);

    void setFinalState(D3D12_RESOURCE_STATES state);

private:

    friend class RenderGraphBuilder;

    std::string name_;
    int         index_;

    D3D12_RESOURCE_STATES initialState_;
    D3D12_RESOURCE_STATES finalState_;
};

class Vertex : public misc::uncopyable_t
{
public:

    using Callback = std::function<void(ID3D12GraphicsCommandList *)>;

    Vertex(std::string name, int index);

    virtual ~Vertex() = default;

    void useResource(InternalResource *rsc, D3D12_RESOURCE_STATES state);

    void useResource(ExternalResource *rsc, D3D12_RESOURCE_STATES state);

    void setQueue(int index);

    void setThread(int index);

    void setCallback(std::shared_ptr<Callback> callback);

private:

    friend class RenderGraphBuilder;

    struct ResourceUsage
    {
        misc::variant_t<
            InternalResource*,
            ExternalResource*> rsc;

        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
    };

    std::string name_;
    int         index_;

    D3D12_COMMAND_LIST_TYPE cmdListType_;

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

    InternalResource *addInternalResource(std::string name);

    ExternalResource *addExternalResource(std::string name);

    Vertex *addVertex(std::string name);

    void addArc(Vertex *head, Vertex *tail);

    RenderGraph build(
        ID3D12Device                                     *device,
        std::initializer_list<ComPtr<ID3D12CommandQueue>> cmdQueues) const;

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

        bool mustSubmit = false;
    };

    struct ThreadRecord
    {
        std::vector<int> passes;
    };

    std::vector<int> topologySortPasses() const;

    int threadCount_;
    int queueCount_;

    std::vector<std::unique_ptr<Resource>> rscs_;
    std::vector<std::unique_ptr<Vertex>>   vtxs_;
};

} // namespace rg

AGZ_D3D12_END
