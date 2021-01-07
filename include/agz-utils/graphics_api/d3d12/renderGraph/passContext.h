#pragma once

#include <agz-utils/graphics_api/d3d12/renderGraph/descriptorQueue.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

using RawGraphicsCommandList = ID3D12GraphicsCommandList4;

class PassContext : public misc::uncopyable_t
{
public:

    struct ResourceUsage
    {
        Resource       *node           = nullptr;
        DescriptorSlot *descriptorSlot = nullptr;
    };

    PassContext(
        Runtime                            *runtime,
        int                                 frameIndex,
        RawGraphicsCommandList             *cmdList,
        const std::map<int, ResourceUsage> &resourceUsages) noexcept;

    int getFrameIndex() const noexcept;

    RawGraphicsCommandList *getCommandList() noexcept;

    RawGraphicsCommandList *operator->() noexcept;

    ID3D12Resource *getRawResource(const Resource *resource) noexcept;

    Descriptor getDescriptor(const Resource *resource);

private:

    Runtime                            *runtime_;
    int                                 frameIndex_;
    RawGraphicsCommandList             *cmdList_;
    const std::map<int, ResourceUsage> &resources_;
};

AGZ_D3D12_RENDERGRAPH_END
