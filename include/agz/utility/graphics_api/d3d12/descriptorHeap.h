#pragma once

#include <agz/utility/container.h>
#include <agz/utility/graphics_api/d3d12/descriptorHeapRaw.h>

AGZ_D3D12_BEGIN

class Descriptor
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_;

public:

    Descriptor() noexcept : cpu_{}, gpu_{} { }

    Descriptor(
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) noexcept
        : cpu_(cpuHandle), gpu_(gpuHandle)
    {
        
    }

    D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle() const noexcept { return cpu_; }

    D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const noexcept { return gpu_; }

    operator D3D12_CPU_DESCRIPTOR_HANDLE() const noexcept { return getCPUHandle(); }

    operator D3D12_GPU_DESCRIPTOR_HANDLE() const noexcept { return getGPUHandle(); }

    operator bool() const noexcept { return cpu_.ptr != 0; }
};

class DescriptorRange
{
    RawDescriptorHeap *rawHeap_;

    uint32_t beg_;
    uint32_t cnt_;

public:

    DescriptorRange() noexcept;

    DescriptorRange(
        RawDescriptorHeap *rawHeap,
        uint32_t           beg,
        uint32_t           cnt) noexcept;

    DescriptorRange getSubRange(
        uint32_t offset,
        uint32_t cnt) const noexcept;

    Descriptor getDescriptor(uint32_t idx) const noexcept;

    Descriptor operator[](uint32_t idx) const noexcept;

    uint32_t getStartIndexInRawHeap() const noexcept;

    uint32_t getCount() const noexcept;
};

class DescriptorSubHeap : public misc::uncopyable_t
{
    friend class DescriptorHeap;

    RawDescriptorHeap *rawHeap_;

    container::interval_mgr_t<uint32_t> freeRanges_;

    uint32_t beg_;
    uint32_t cnt_;

public:

    DescriptorSubHeap();

    DescriptorSubHeap(DescriptorSubHeap &&other) noexcept;

    DescriptorSubHeap &operator=(DescriptorSubHeap &&other) noexcept;

    ~DescriptorSubHeap();

    void initialize(
        RawDescriptorHeap *rawHeap,
        uint32_t           beg,
        uint32_t           cnt);

    bool isAvailable() const noexcept;

    void destroy();

    void swap(DescriptorSubHeap &other) noexcept;

    ID3D12DescriptorHeap *getRawHeap() const noexcept;

    DescriptorSubHeap allocSubHeap(uint32_t subHeapSize);

    DescriptorRange allocRange(uint32_t rangeSize);

    Descriptor alloc();

    std::optional<DescriptorSubHeap> tryAllocSubHeap(uint32_t subHeapSize);

    std::optional<DescriptorRange> tryAllocRange(uint32_t rangeSize);

    std::optional<Descriptor> tryAlloc();

    void freeAll();

    void freeSubHeap(DescriptorSubHeap &&subheap);

    void freeRange(const DescriptorRange &range);

    void free(Descriptor descriptor);
};

class DescriptorHeap : DescriptorSubHeap
{
    RawDescriptorHeap rawHeap_;

public:

    void initialize(
        ID3D12Device              *device,
        uint32_t                   size,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        bool                       isShaderVisible);

    bool isAvailable() const noexcept;

    void destroy();

    using DescriptorSubHeap::getRawHeap;

    using DescriptorSubHeap::allocSubHeap;
    using DescriptorSubHeap::allocRange;
    using DescriptorSubHeap::alloc;

    using DescriptorSubHeap::tryAllocSubHeap;
    using DescriptorSubHeap::tryAllocRange;
    using DescriptorSubHeap::tryAlloc;

    using DescriptorSubHeap::freeAll;
    using DescriptorSubHeap::freeSubHeap;
    using DescriptorSubHeap::freeRange;
    using DescriptorSubHeap::free;

    DescriptorSubHeap       &asSubHeap()       noexcept;
    const DescriptorSubHeap &asSubHeap() const noexcept;
};

AGZ_D3D12_END
