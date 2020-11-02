#pragma once

#include <mutex>

#include <agz/utility/graphics_api/d3d12/descriptorHeap.h>

AGZ_D3D12_BEGIN

class DescriptorAllocator;

class TransientDescriptorAllocator : public misc::uncopyable_t
{
public:

    TransientDescriptorAllocator();

    TransientDescriptorAllocator(
        DescriptorAllocator   *mainAllocator,
        const DescriptorRange &range,
        float                  sizeIncFactor);

    TransientDescriptorAllocator(
        TransientDescriptorAllocator &&other) noexcept;

    TransientDescriptorAllocator &operator=(
        TransientDescriptorAllocator &&other) noexcept;

    bool isAvailable() const noexcept;

    void swap(TransientDescriptorAllocator &other) noexcept;

    Descriptor alloc();

    DescriptorRange allocRange(uint32_t size);

private:

    float sizeIncFactor_;

    DescriptorRange range_;
    uint32_t next_;

    DescriptorAllocator *mainAllocator_;
};

class DescriptorAllocator : public misc::uncopyable_t
{
public:

    DescriptorAllocator(
        ID3D12Device              *device,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        bool                       isShaderVisible,
        int                        heapSize,
        int                        frameCount);

    Descriptor allocStatic();

    void freeStatic(Descriptor descriptor);

    DescriptorRange allocStaticRange(uint32_t count);

    void freeRangeStatic(const DescriptorRange &range);

    void startFrame(int frameIndex);

    DescriptorRange allocTransientRange(uint32_t size);

    TransientDescriptorAllocator createTransientAllocator(
        uint32_t initialSize = 64, float sizeIncFactor = 1.5f);

    ID3D12DescriptorHeap *getHeap() const noexcept;

private:

    struct FrameResource
    {
        std::vector<DescriptorRange> dynamicRanges;
    };

    std::vector<FrameResource> frameRscs_;
    int frameIndex_;

    std::mutex heapMutex_;
    DescriptorHeap heap_;
};

AGZ_D3D12_END
