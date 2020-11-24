#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/renderGraph/compiler.h>
#include <agz/utility/graphics_api/d3d12/renderGraph/passContext.h>
#include <agz/utility/graphics_api/d3d12/renderGraph/runtime.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

PassContext::PassContext(
    Runtime                            *runtime,
    int                                 frameIndex,
    ID3D12GraphicsCommandList          *cmdList,
    const std::map<int, ResourceUsage> &resourceUsages) noexcept
    : runtime_(runtime),
      frameIndex_(frameIndex),
      cmdList_(cmdList),
      resources_(resourceUsages)
{
    
}

int PassContext::getFrameIndex() const noexcept
{
    return frameIndex_;
}

ID3D12GraphicsCommandList *PassContext::getCommandList() noexcept
{
    return cmdList_;
}

ID3D12GraphicsCommandList *PassContext::operator->() noexcept
{
    return cmdList_;
}

ID3D12Resource *PassContext::getRawResource(const Resource *resource) noexcept
{
    return runtime_->getRawResource(resource->getIndex());
}

Descriptor PassContext::getDescriptor(const Resource *resource)
{
    const auto it = resources_.find(resource->getIndex());
    if(it == resources_.end())
    {
        throw D3D12Exception(
            "undecled resource usage of " + resource->getName());
    }
    return it->second.descriptorSlot->descriptor;
}

AGZ_D3D12_RENDERGRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
