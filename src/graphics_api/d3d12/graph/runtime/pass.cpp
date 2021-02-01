#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/compiler.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/runtime.h>

AGZ_D3D12_GRAPH_BEGIN

PassRuntime::PassRuntime(
    Runtime                        *runtime,
    std::shared_ptr<PassCallback>   callback,
    std::vector<StateTransition>    transitions,
    PassContext::DescriptorMap      descriptors,
    PassContext::DescriptorRangeMap descriptorRanges)
    : runtime_(runtime),
      callback_(std::move(callback)),
      transitions_(std::move(transitions)),
      descriptors_(std::move(descriptors)),
      descriptorRanges_(std::move(descriptorRanges))
{
    for(auto &p : descriptors_)
    {
        const auto it = resourceToDescriptors_.find(p.first->getResource());
        if(it == resourceToDescriptors_.end())
            resourceToDescriptors_.insert({ p.first->getResource(), p.second });
        else
            it->second = nullptr;
    }
}

void PassRuntime::execute(
    int                     frameIndex,
    RawGraphicsCommandList *cmdList) const
{
    entryBarriers_.clear();
    exitBarriers_.clear();

    for(auto &rt : transitions_)
    {
        if(rt.beg != rt.mid)
        {
            entryBarriers_.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                runtime_->getRawResource(rt.resource), rt.beg, rt.mid, rt.subrsc));
        }

        if(rt.mid != rt.end)
        {
            exitBarriers_.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                runtime_->getRawResource(rt.resource), rt.mid, rt.end, rt.subrsc));
        }
    }

    if(!entryBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(entryBarriers_.size()),
            entryBarriers_.data());
    }

    PassContext passContext(
        runtime_, frameIndex, cmdList,
        descriptors_, resourceToDescriptors_, descriptorRanges_);

    (*callback_)(passContext);

    if(!exitBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(exitBarriers_.size()),
            exitBarriers_.data());
    }
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
