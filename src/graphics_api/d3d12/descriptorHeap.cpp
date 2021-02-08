#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/descriptorHeap.h>

AGZ_D3D12_BEGIN

DescriptorRange::DescriptorRange() noexcept
    : DescriptorRange(nullptr, 0, 0)
{
    
}

DescriptorRange::DescriptorRange(
    RawDescriptorHeap *rawHeap,
    uint32_t           beg,
    uint32_t           cnt) noexcept
    : rawHeap_(rawHeap), beg_(beg), cnt_(cnt)
{
    
}

DescriptorRange DescriptorRange::getSubRange(
    uint32_t offset,
    uint32_t cnt) const noexcept
{
    assert(offset + cnt <= cnt_);
    return DescriptorRange(rawHeap_, beg_ + offset, cnt);
}

Descriptor DescriptorRange::getDescriptor(size_t idx) const noexcept
{
    return Descriptor(
        rawHeap_->getCPUHandle(static_cast<int>(beg_ + idx)),
        rawHeap_->getGPUHandle(static_cast<int>(beg_ + idx)));
}

Descriptor DescriptorRange::operator[](size_t idx) const noexcept
{
    return getDescriptor(idx);
}

uint32_t DescriptorRange::getStartIndexInRawHeap() const noexcept
{
    return beg_;
}

uint32_t DescriptorRange::getCount() const noexcept
{
    return cnt_;
}

DescriptorSubHeap::DescriptorSubHeap()
{
    rawHeap_ = nullptr;
    beg_     = 0;
    cnt_     = 0;
}

DescriptorSubHeap::DescriptorSubHeap(DescriptorSubHeap &&other) noexcept
    : DescriptorSubHeap()
{
    swap(other);
}

DescriptorSubHeap &DescriptorSubHeap::operator=(DescriptorSubHeap &&other) noexcept
{
    swap(other);
    return *this;
}

DescriptorSubHeap::~DescriptorSubHeap()
{

}

void DescriptorSubHeap::initialize(
    RawDescriptorHeap *rawHeap,
    uint32_t           beg,
    uint32_t           cnt)
{
    destroy();

    misc::fixed_scope_guard_t guard([&] { destroy(); });

    freeRanges_.free(beg, beg + cnt);
    rawHeap_ = rawHeap;
    beg_     = beg;
    cnt_     = cnt;

    guard.dismiss();
}

bool DescriptorSubHeap::isAvailable() const noexcept
{
    return rawHeap_ != nullptr;
}

void DescriptorSubHeap::destroy()
{
    rawHeap_    = nullptr;
    freeRanges_ = {};
    beg_        = 0;
    cnt_        = 0;
}

void DescriptorSubHeap::swap(DescriptorSubHeap &other) noexcept
{
    std::swap(rawHeap_, other.rawHeap_);
    std::swap(beg_, other.beg_);
    std::swap(cnt_, other.cnt_);
    freeRanges_.swap(other.freeRanges_);
}

ID3D12DescriptorHeap *DescriptorSubHeap::getRawHeap() const noexcept
{
    return rawHeap_->getHeap();
}

DescriptorSubHeap DescriptorSubHeap::allocSubHeap(uint32_t subHeapSize)
{
    return *tryAllocSubHeap(subHeapSize);
}

DescriptorRange DescriptorSubHeap::allocRange(uint32_t rangeSize)
{
    return *tryAllocRange(rangeSize);
}

Descriptor DescriptorSubHeap::alloc()
{
    return *tryAlloc();
}

std::optional<DescriptorSubHeap> DescriptorSubHeap::tryAllocSubHeap(
    uint32_t subHeapSize)
{
    const auto obeg = freeRanges_.alloc(subHeapSize);
    if(!obeg)
        return std::nullopt;

    DescriptorSubHeap subheap;
    subheap.initialize(rawHeap_, *obeg, subHeapSize);

    return std::make_optional(std::move(subheap));
}

std::optional<DescriptorRange> DescriptorSubHeap::tryAllocRange(
    uint32_t rangeSize)
{
    const auto obeg = freeRanges_.alloc(rangeSize);
    if(!obeg)
        return std::nullopt;
    return std::make_optional<DescriptorRange>(rawHeap_, *obeg, rangeSize);
}

std::optional<Descriptor> DescriptorSubHeap::tryAlloc()
{
    auto orange = tryAllocRange(1);
    return orange ? std::make_optional(orange->getDescriptor(0)) : std::nullopt;
}

void DescriptorSubHeap::freeAll()
{
    initialize(rawHeap_, beg_, cnt_);
}

void DescriptorSubHeap::freeSubHeap(DescriptorSubHeap &&subheap)
{
    assert(subheap.rawHeap_ == rawHeap_);
    freeRanges_.free(subheap.beg_, subheap.beg_ + subheap.cnt_);
    subheap.destroy();
}

void DescriptorSubHeap::freeRange(const DescriptorRange &range)
{
    const uint32_t beg = range.getStartIndexInRawHeap();
    const uint32_t end = beg + range.getCount();
    freeRanges_.free(beg, end);
}

void DescriptorSubHeap::free(Descriptor descriptor)
{
    const auto rawDiff = descriptor.getCPUHandle().ptr -
                         rawHeap_->getCPUHandle(0).ptr;

    assert(rawDiff % rawHeap_->getDescSize() == 0);
    const uint32_t idx = static_cast<uint32_t>(
        rawDiff / rawHeap_->getDescSize());

    freeRanges_.free(idx, idx + 1);
}

void DescriptorHeap::initialize(
    ID3D12Device              *device,
    uint32_t                   size,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    bool                       isShaderVisible)
{
    rawHeap_ = RawDescriptorHeap(device, size, type, isShaderVisible);
    misc::fixed_scope_guard_t guard([&] { destroy(); });

    DescriptorSubHeap::initialize(&rawHeap_, 0, size);
    guard.dismiss();
}

bool DescriptorHeap::isAvailable() const noexcept
{
    return rawHeap_.isAvailable();
}

void DescriptorHeap::destroy()
{
    DescriptorSubHeap::destroy();
    rawHeap_ = {};
}

DescriptorSubHeap &DescriptorHeap::asSubHeap() noexcept
{
    return *this;
}

const DescriptorSubHeap &DescriptorHeap::asSubHeap() const noexcept
{
    return *this;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12