#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph/pass.h>
#include <agz/utility/graphics_api/d3d12/renderGraph/runtime.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

Pass::Pass(
    Runtime                                  *runtime,
    std::shared_ptr<PassCallback>             callback,
    std::vector<ResourceTransition>           transitions,
    std::map<int, PassContext::ResourceUsage> contextResources)
    : runtime_(runtime),
      callback_(std::move(callback)),
      transitions_(std::move(transitions)),
      contextResources_(std::move(contextResources))
{

}

void Pass::execute(
    int                     frameIndex,
    RawGraphicsCommandList *cmdList) const
{
    beforeBarriers_.clear();
    afterBarriers_.clear();

    for(auto &rt : transitions_)
    {
        if(rt.beg != rt.mid)
        {
            beforeBarriers_.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                runtime_->getRawResource(rt.resource), rt.beg, rt.mid));
        }

        if(rt.mid != rt.end)
        {
            afterBarriers_.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                runtime_->getRawResource(rt.resource), rt.mid, rt.end));
        }
    }

    if(!beforeBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(beforeBarriers_.size()),
            beforeBarriers_.data());
    }

    PassContext passContext(
        runtime_, frameIndex, cmdList, contextResources_);

    (*callback_)(passContext);

    if(!afterBarriers_.empty())
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(afterBarriers_.size()),
            afterBarriers_.data());
    }
}

AGZ_D3D12_RENDERGRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
