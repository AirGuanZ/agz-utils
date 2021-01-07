#pragma once

#include <queue>

#include <agz-utils/graphics_api/d3d12/descriptorAllocator.h>
#include <agz-utils/graphics_api/d3d12/renderGraph/common.h>

AGZ_D3D12_RENDERGRAPH_BEGIN

class DescriptorQueue : public misc::uncopyable_t
{
public:

    DescriptorQueue() = default;

    explicit DescriptorQueue(const DescriptorRange &range);

    DescriptorQueue(DescriptorQueue &&) noexcept = default;

    DescriptorQueue &operator=(DescriptorQueue &&) noexcept = default;

    Descriptor alloc();

    void free(Descriptor descriptor);

private:

    std::queue<Descriptor> descs_;
};

inline DescriptorQueue::DescriptorQueue(const DescriptorRange &range)
{
    for(uint32_t i = 0; i < range.getCount(); ++i)
        descs_.push(range[i]);
}

inline Descriptor DescriptorQueue::alloc()
{
    assert(!descs_.empty());
    auto ret = descs_.front();
    descs_.pop();
    return ret;
}

inline void DescriptorQueue::free(Descriptor descriptor)
{
    descs_.push(descriptor);
}

AGZ_D3D12_RENDERGRAPH_END
