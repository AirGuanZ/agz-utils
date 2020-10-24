#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraphBuilder.h>

AGZ_D3D12_BEGIN

namespace rg
{

InternalResource::InternalResource(std::string name, int index)
    : name_(std::move(name)), index_(index),
      desc_{},
      clear_(false), clearValue_{},
      initialState_(D3D12_RESOURCE_STATE_COMMON)
{
    
}

void InternalResource::setDescription(const D3D12_RESOURCE_DESC &desc)
{
    desc_ = desc;
}

void InternalResource::setClearValue(const D3D12_CLEAR_VALUE &clearValue)
{
    clear_      = true;
    clearValue_ = clearValue;
}

void InternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

ExternalResource::ExternalResource(std::string name, int index)
    : name_(std::move(name)), index_(index),
      initialState_(D3D12_RESOURCE_STATE_COMMON),
      finalState_  (D3D12_RESOURCE_STATE_COMMON)
{
    
}

void ExternalResource::setInitialState(D3D12_RESOURCE_STATES state)
{
    initialState_ = state;
}

void ExternalResource::setFinalState(D3D12_RESOURCE_STATES state)
{
    finalState_ = state;
}

Vertex::Vertex(std::string name, int index)
    : name_(std::move(name)), index_(index),
      cmdListType_(D3D12_COMMAND_LIST_TYPE_DIRECT),
      threadIndex_(0),
      queueIndex_(0)
{
    
}

void Vertex::useResource(ExternalResource *rsc, D3D12_RESOURCE_STATES state)
{
    rscUsages_.push_back({ rsc, state });
}

void Vertex::useResource(InternalResource *rsc, D3D12_RESOURCE_STATES state)
{
    rscUsages_.push_back({ rsc, state });
}

void Vertex::setQueue(int index)
{
    queueIndex_ = index;
}

void Vertex::setThread(int index)
{
    threadIndex_ = index;
}

void Vertex::setCallback(std::shared_ptr<Callback> callback)
{
    callback_ = std::move(callback);
}

RenderGraphBuilder::RenderGraphBuilder()
    : threadCount_(0), queueCount_(0)
{
    
}

void RenderGraphBuilder::setThreadCount(int count)
{
    threadCount_ = count;
}

void RenderGraphBuilder::setQueueCount(int count)
{
    queueCount_ = count;
}

InternalResource *RenderGraphBuilder::addInternalResource(std::string name)
{
    const int index = static_cast<int>(rscs_.size());
    rscs_.push_back(
        std::make_unique<Resource>(InternalResource(std::move(name), index)));
    return &rscs_.back()->as<InternalResource>();
}

ExternalResource *RenderGraphBuilder::addExternalResource(std::string name)
{
    const int index = static_cast<int>(rscs_.size());
    rscs_.push_back(
        std::make_unique<Resource>(ExternalResource(std::move(name), index)));
    return &rscs_.back()->as<ExternalResource>();
}

Vertex *RenderGraphBuilder::addVertex(std::string name)
{
    const int index = static_cast<int>(vtxs_.size());
    vtxs_.push_back(std::make_unique<Vertex>(std::move(name), index));
    return vtxs_.back().get();
}

void RenderGraphBuilder::addArc(Vertex *head, Vertex *tail)
{
    head->out_.insert(tail);
    tail->in_ .insert(head);
}

RenderGraph RenderGraphBuilder::build(
    ID3D12Device                                     *device,
    std::initializer_list<ComPtr<ID3D12CommandQueue>> cmdQueues) const
{
    std::vector<PassRecord> passRecords(vtxs_.size());

    const auto linearPasses = topologySortPasses();

    // assign passes to threads

    std::vector<ThreadRecord> thread2Passes(threadCount_);
    for(int i : linearPasses)
    {
        const int threadIndex = vtxs_[i]->threadIndex_;
        thread2Passes[threadIndex].passes.push_back(i);
    }

    // collect all users of each resource

    std::vector<ResourceRecord> resource2Usages(rscs_.size());
    for(int passIdx : linearPasses)
    {
        const Vertex *vtx = vtxs_[passIdx].get();
        for(auto &r : vtx->rscUsages_)
        {
            const int rscIdx = match_variant(
                r.rsc,
                [&](const InternalResource *rsc) { return rsc->index_; },
                [&](const ExternalResource *rsc) { return rsc->index_; });

            resource2Usages[rscIdx].usages.push_back(
                ResourceRecord::Usage{
                    .pass        = passIdx,
                    .state       = r.state,
                    .cmdListType = vtx->cmdListType_
                });
        }
    }

    // generate resource state barriers

    for(size_t rscIdx = 0; rscIdx < rscs_.size(); ++rscIdx)
    {
        const auto &usages = resource2Usages[rscIdx].usages;
        const auto &rsc    = *rscs_[rscIdx];

        if(usages.empty())
            continue;

        const D3D12_RESOURCE_STATES initialState = match_variant(
            rsc,
            [&](const InternalResource &r) { return r.initialState_; },
            [&](const ExternalResource &r) { return r.initialState_; });

        const D3D12_RESOURCE_STATES finalState = match_variant(
            rsc,
            [&](const InternalResource &r) { return r.initialState_; },
            [&](const ExternalResource &r) { return r.finalState_; });

        D3D12_RESOURCE_STATES lastState = initialState;
        for(size_t i = 0; i < usages.size(); ++i)
        {
            auto &usage = usages[i];

            const D3D12_RESOURCE_STATES beg = lastState;
            const D3D12_RESOURCE_STATES mid = usage.state;
            const D3D12_RESOURCE_STATES end =
                (i == usages.size() - 1) ? finalState : mid;

            if(beg != mid || mid != end)
            {
                passRecords[usage.pass].transitions.push_back({
                       static_cast<int>(rscIdx), beg, mid, end });
            }

            // TODO: UAV barrier

            lastState = end;
        }
    }

    // generate fences & submission flags

    std::vector<ComPtr<ID3D12Fence>> allFences;
    for(auto &vtx : vtxs_)
    {
        ComPtr<ID3D12Fence> fence;

        for(auto out : vtx->out_)
        {
            if(vtx->queueIndex_ != out->queueIndex_)
            {
                if(!fence)
                {
                    AGZ_D3D12_CHECK_HR_MSG(
                        "failed to create fence object for render graph",
                        device->CreateFence(
                            0, D3D12_FENCE_FLAG_NONE,
                            IID_PPV_ARGS(fence.GetAddressOf())));

                    passRecords[vtx->index_].signalFence = fence;
                    allFences.push_back(fence);
                }

                passRecords[out->index_].waitFences.push_back(fence);
            }

            if(vtx->queueIndex_  != out->queueIndex_ ||
               vtx->threadIndex_ != out->threadIndex_)
                passRecords[vtx->index_].mustSubmit = true;
        }
    }

    for(auto &tr : thread2Passes)
    {
        if(tr.passes.empty())
            continue;
        passRecords[tr.passes.back()].mustSubmit = true;
    }

    // generate render graph

    RenderGraph renderGraph;

    // TODO
    return renderGraph;
}

std::vector<int> RenderGraphBuilder::topologySortPasses() const
{
    // TODO
    return {};
}

} // namespace rg

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
