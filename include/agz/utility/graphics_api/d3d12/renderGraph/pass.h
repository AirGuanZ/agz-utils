#pragma once

#include <agz/utility/graphics_api/d3d12/renderGraph/passContext.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

class Pass : public misc::uncopyable_t
{
public:

    struct ResourceTransition
    {
        Resource *resource;
        D3D12_RESOURCE_STATES beg;
        D3D12_RESOURCE_STATES mid;
        D3D12_RESOURCE_STATES end;
    };

    Pass(
        Runtime                                  *runtime,
        std::shared_ptr<PassCallback>             callback,
        std::vector<ResourceTransition>           transitions,
        std::map<int, PassContext::ResourceUsage> contextResources);

    void execute(
        int                     frameIndex,
        RawGraphicsCommandList *cmdList) const;

private:

    Runtime *runtime_;

    std::shared_ptr<PassCallback> callback_;

    std::vector<ResourceTransition>           transitions_;
    std::map<int, PassContext::ResourceUsage> contextResources_;

    // IMPROVE: most barriers can be cached
    mutable std::vector<D3D12_RESOURCE_BARRIER> beforeBarriers_;
    mutable std::vector<D3D12_RESOURCE_BARRIER> afterBarriers_;
};

AGZ_D3D12_RENDERGRAPH_END
