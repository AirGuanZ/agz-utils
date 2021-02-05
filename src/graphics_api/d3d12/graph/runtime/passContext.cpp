#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/compiler.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/passContext.h>
#include <agz-utils/graphics_api/d3d12/graph/runtime/runtime.h>

AGZ_D3D12_GRAPH_BEGIN

PassContext::PassContext(
    Runtime                     *runtime,
    int                          frameIndex,
    RawGraphicsCommandList      *cmdList,
    const DescriptorMap         &descriptors,
    const DescriptorResourceMap &resourceToDescriptors,
    const DescriptorRangeMap    &descriptorRanges)
    : runtime_(runtime),
      frameIndex_(frameIndex),
      cmdList_(cmdList),
      descriptors_(descriptors),
      resourceToDescriptors_(resourceToDescriptors),
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
    const auto it = resourceToDescriptors_.find(resource);
    if(it == resourceToDescriptors_.end())
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "no descriptor is declared for " + resource->getName());
    }

    if(!it->second)
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "multiple descriptors are declared for " + resource->getName());
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->cpu ?
        slot->frames[index].cpu : slot->frames[index].gpu;
}

Descriptor PassContext::getCPUDescriptor(const Resource *resource)
{
    const auto it = resourceToDescriptors_.find(resource);
    if(it == resourceToDescriptors_.end())
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "no descriptor is declared for " + resource->getName());
    }

    if(!it->second)
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "multiple descriptors are declared for " + resource->getName());
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].cpu;
}

Descriptor PassContext::getGPUDescriptor(const Resource *resource)
{
    const auto it = resourceToDescriptors_.find(resource);
    if(it == resourceToDescriptors_.end())
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "no descriptor is declared for " + resource->getName());
    }

    if(!it->second)
    {
        throw D3D12Exception(
            "PassContext::getDescriptor: "
            "multiple descriptors are declared for " + resource->getName());
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].gpu;
}

Descriptor PassContext::getDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->cpu ?
        slot->frames[index].cpu : slot->frames[index].gpu;
}

Descriptor PassContext::getCPUDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].cpu;
}

Descriptor PassContext::getGPUDescriptor(const DescriptorItem *item)
{
    const auto it = descriptors_.find(item);
    if(it == descriptors_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor");
    }

    const DescriptorSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].gpu;
}

DescriptorRange PassContext::getDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    const DescriptorRangeSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->cpu ?
        slot->frames[index].cpu : slot->frames[index].gpu;
}

DescriptorRange PassContext::getCPUDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    const DescriptorRangeSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].cpu;
}

DescriptorRange PassContext::getGPUDescriptorRange(const DescriptorTable *table)
{
    const auto it = descriptorRanges_.find(table);
    if(it == descriptorRanges_.end())
    {
        throw D3D12Exception(
            "undeclared resource descriptor range");
    }

    const DescriptorRangeSlot *slot = it->second;
    const int index = slot->isPerFrame ? frameIndex_ : 0;

    return slot->frames[index].gpu;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
