#pragma once

#include <agz-utils/graphics_api/d3d12/graph/common.h>
#include <agz-utils/graphics_api/d3d12/descriptorAllocator.h>

AGZ_D3D12_GRAPH_BEGIN

class PassContext : public misc::uncopyable_t
{
public:

    using DescriptorMap      = std::map<const Resource *, std::vector<DescriptorSlot *>>;
    using DescriptorRangeMap = std::map<const DescriptorTable *, DescriptorRangeSlot *>;

    struct ResourceUsage
    {
        Resource       *resource       = nullptr;
        DescriptorSlot *descriptorSlot = nullptr;
    };

    PassContext(
        GraphRuntime             *runtime,
        int                       frameIndex,
        RawGraphicsCommandList   *cmdList,
        const DescriptorMap      &descriptors,
        const DescriptorRangeMap &descriptorRanges);

    int getFrameIndex() const;

    RawGraphicsCommandList *getCommandList();

    RawGraphicsCommandList *operator->();

    ID3D12Resource *getRawResource(const Resource *resource);

    Descriptor getDescriptor(const Resource *resource, int index = 0);

    Descriptor getCPUDescriptor(const Resource *resource, int index = 0);

    Descriptor getGPUDescriptor(const Resource *resource, int index = 0);

    DescriptorRange getDescriptorRange(const DescriptorTable *table);

    DescriptorRange getCPUDescriptorRange(const DescriptorTable *table);

    DescriptorRange getGPUDescriptorRange(const DescriptorTable *table);

private:

    GraphRuntime           *runtime_;

    int                     frameIndex_;
    RawGraphicsCommandList *cmdList_;

    const DescriptorMap      &descriptors_;
    const DescriptorRangeMap &descriptorRanges_;
};

AGZ_D3D12_GRAPH_END
