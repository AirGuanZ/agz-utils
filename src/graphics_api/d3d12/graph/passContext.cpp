#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/graphCompiler.h>
#include <agz-utils/graphics_api/d3d12/graph/graphRuntime.h>
#include <agz-utils/graphics_api/d3d12/graph/passContext.h>

AGZ_D3D12_GRAPH_BEGIN

PassContext::PassContext(
    GraphRuntime                *runtime,
    int                          frameIndex,
    RawGraphicsCommandList      *cmdList,
    const DescriptorMap         &descriptors,
    const DescriptorResourceMap &descriptorResources,
    const DescriptorRangeMap    &descriptorRanges)
    : runtime_(runtime),
      frameIndex_(frameIndex),
      cmdList_(cmdList),
      descriptors_(descriptors),
      descriptorResourceMap_(descriptorResources),
      descriptorRanges_(descriptorRanges)
{
    
}

int PassContext::getFrameIndex() const
{
    return frameIndex_;
}

RawGraphicsCommandList *PassContext::getCommandList()
{
    return cmdList_;
}

RawGraphicsCommandList *PassContext::operator->()
{
    return cmdList_;
}

ID3D12Resource *PassContext::getRawResource(const Resource *resource)
{
    return runtime_->getRawResource(resource);
}

Descriptor PassContext::getDescriptor(const Resource *resource)
{
    const auto it = descriptorResourceMap_.find(resource);
    if(it == descriptorResourceMap_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->cpu ^ it->second->gpu);
    return it->second->cpu ?
        it->second->cpuDescriptor : it->second->gpuDescriptor;
}

Descriptor PassContext::getCPUDescriptor(const Resource *resource)
{
    const auto it = descriptorResourceMap_.find(resource);
    if(it == descriptorResourceMap_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->cpu);
    return it->second->cpuDescriptor;
}

Descriptor PassContext::getGPUDescriptor(const Resource *resource)
{
    const auto it = descriptorResourceMap_.find(resource);
    if(it == descriptorResourceMap_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->gpu);
    return it->second->gpuDescriptor;
}

Descriptor PassContext::getDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->cpu ^ it->second->gpu);
    return it->second->cpu ?
        it->second->cpuDescriptor : it->second->gpuDescriptor;
}

Descriptor PassContext::getCPUDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->cpu);
    return it->second->cpuDescriptor;
}

Descriptor PassContext::getGPUDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    assert(it->second->gpu);
    return it->second->gpuDescriptor;
}

DescriptorRange PassContext::getDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    assert(it->second->cpu ^ it->second->gpu);
    return it->second->cpu ?
        it->second->cpuDescriptorRange : it->second->gpuDescriptorRange;
}

DescriptorRange PassContext::getCPUDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    assert(it->second->cpu);
    return it->second->cpuDescriptorRange;
}

DescriptorRange PassContext::getGPUDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    assert(it->second->gpu);
    return it->second->gpuDescriptorRange;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
