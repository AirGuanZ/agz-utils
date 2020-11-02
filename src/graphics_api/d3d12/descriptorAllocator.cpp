#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/descriptorAllocator.h>

AGZ_D3D12_BEGIN

TransientDescriptorAllocator::TransientDescriptorAllocator()
    : sizeIncFactor_(0), next_(0), mainAllocator_(nullptr)
{
    
}

TransientDescriptorAllocator::TransientDescriptorAllocator(
    DescriptorAllocator   *mainAllocator,
    const DescriptorRange &range,
    float                  sizeIncFactor)
    : sizeIncFactor_(sizeIncFactor),
      range_(range),
      next_(0),
      mainAllocator_(mainAllocator)
{
    
}

TransientDescriptorAllocator::TransientDescriptorAllocator(
    TransientDescriptorAllocator &&other) noexcept
    : TransientDescriptorAllocator()
{
    swap(other);
}

TransientDescriptorAllocator &TransientDescriptorAllocator::operator=(
    TransientDescriptorAllocator &&other) noexcept
{
    swap(other);
    return *this;
}

bool TransientDescriptorAllocator::isAvailable() const noexcept
{
    return mainAllocator_ != nullptr;
}

void TransientDescriptorAllocator::swap(
    TransientDescriptorAllocator &other) noexcept
{
    std::swap(sizeIncFactor_, other.sizeIncFactor_);
    std::swap(range_,         other.range_);
    std::swap(next_,          other.next_);
    std::swap(mainAllocator_, other.mainAllocator_);
}

Descriptor TransientDescriptorAllocator::alloc()
{
    return allocRange(1)[0];
}

DescriptorRange TransientDescriptorAllocator::allocRange(uint32_t size)
{
    if(next_ + size <= range_.getCount())
    {
        const auto ret = range_.getSubRange(next_, size);
        next_ += size;
        return ret;
    }

    const uint32_t newPoolSize = static_cast<uint32_t>(
        std::ceil(sizeIncFactor_ * range_.getCount()));

    // allocate from main allocator directly when size is too large.
    if(newPoolSize <= size)
        return mainAllocator_->allocTransientRange(size);

    try
    {
        range_ = mainAllocator_->allocTransientRange(newPoolSize);
    }
    catch(...)
    {
        return mainAllocator_->allocTransientRange(size);
    }

    const auto ret = range_.getSubRange(0, size);
    next_ = size;

    return ret;
}

DescriptorAllocator::DescriptorAllocator(
    ID3D12Device              *device,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    bool                       isShaderVisible,
    int                        heapSize,
    int                        frameCount)
    : frameIndex_(0)
{
    frameRscs_.resize(frameCount);
    heap_.initialize(device, heapSize, type, isShaderVisible);
}

Descriptor DescriptorAllocator::allocStatic()
{
    return heap_.alloc();
}

void DescriptorAllocator::freeStatic(Descriptor descriptor)
{
    heap_.free(descriptor);
}

DescriptorRange DescriptorAllocator::allocStaticRange(uint32_t count)
{
    return heap_.allocRange(count);
}

void DescriptorAllocator::freeRangeStatic(const DescriptorRange &range)
{
    heap_.freeRange(range);
}

void DescriptorAllocator::startFrame(int frameIndex)
{
    frameIndex_ = frameIndex;

    for(auto &r : frameRscs_[frameIndex_].dynamicRanges)
        heap_.freeRange(r);
    frameRscs_[frameIndex_].dynamicRanges.clear();
}

DescriptorRange DescriptorAllocator::allocTransientRange(uint32_t size)
{
    std::lock_guard lk(heapMutex_);
    const auto ret = heap_.allocRange(size);
    frameRscs_[frameIndex_].dynamicRanges.push_back(ret);
    return ret;
}

TransientDescriptorAllocator DescriptorAllocator::createTransientAllocator(
    uint32_t initialSize,
    float    sizeIncFactor)
{
    return TransientDescriptorAllocator(
        this, allocTransientRange(initialSize), sizeIncFactor);
}

ID3D12DescriptorHeap *DescriptorAllocator::getHeap() const noexcept
{
    return heap_.getRawHeap();
}

AGZ_D3D12_END

#endif //#ifdef AGZ_ENABLE_D3D12
