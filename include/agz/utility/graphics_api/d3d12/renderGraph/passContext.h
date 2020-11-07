#pragma once

#include <agz/utility/graphics_api/d3d12/renderGraph/descriptorQueue.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

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
        ID3D12GraphicsCommandList          *cmdList,
        const std::map<int, ResourceUsage> &resourceUsages) noexcept;

    int getFrameIndex() const noexcept;

    ID3D12GraphicsCommandList *getCommandList() noexcept;

    ID3D12GraphicsCommandList *operator->() noexcept;

    ID3D12Resource *getRawResource(Resource *resource) noexcept;

    Descriptor getDescriptor(Resource *resource);

private:

    Runtime                            *runtime_;
    int                                 frameIndex_;
    ID3D12GraphicsCommandList          *cmdList_;
    const std::map<int, ResourceUsage> &resources_;
};

AGZ_D3D12_RENDERGRAPH_END
