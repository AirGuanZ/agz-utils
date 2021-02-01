#pragma once

#include <agz-utils/graphics_api/d3d12/graph/runtime/passContext.h>

AGZ_D3D12_GRAPH_BEGIN

class PassRuntime : public misc::uncopyable_t
{
public:

    struct StateTransition
    {
        Resource *resource;
        UINT      subrsc;
        D3D12_RESOURCE_STATES beg;
        D3D12_RESOURCE_STATES mid;
        D3D12_RESOURCE_STATES end;
    };

    PassRuntime(
        Runtime                        *runtime,
        std::shared_ptr<PassCallback>   callback,
        std::vector<StateTransition>    transitions,
        PassContext::DescriptorMap      descriptors,
        PassContext::DescriptorRangeMap descriptorRanges);

    void execute(
        int                     frameIndex,
        RawGraphicsCommandList *cmdList) const;

private:

    Runtime *runtime_;

    std::shared_ptr<PassCallback> callback_;

    std::vector<StateTransition>    transitions_;
    PassContext::DescriptorMap      descriptors_;
    PassContext::DescriptorRangeMap descriptorRanges_;

    PassContext::DescriptorResourceMap resourceToDescriptors_;

    mutable std::vector<D3D12_RESOURCE_BARRIER> entryBarriers_;
    mutable std::vector<D3D12_RESOURCE_BARRIER> exitBarriers_;
};

AGZ_D3D12_GRAPH_END