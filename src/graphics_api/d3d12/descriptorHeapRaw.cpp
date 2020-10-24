#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/descriptorHeapRaw.h>

AGZ_D3D12_BEGIN

RawDescriptorHeap::RawDescriptorHeap()
    : descIncSize_(0), cpuStart_{}, gpuStart_{}
{
    
}

RawDescriptorHeap::RawDescriptorHeap(
    ID3D12Device              *device,
    int                        size,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    bool                       isShaderVisible)
    : RawDescriptorHeap()
{
    initialize(device, size, type, isShaderVisible);
}

void RawDescriptorHeap::swap(RawDescriptorHeap &other) noexcept
{
    std::swap(descIncSize_, other.descIncSize_);
    std::swap(heap_,        other.heap_);
    std::swap(cpuStart_,    other.cpuStart_);
    std::swap(gpuStart_,    other.gpuStart_);
}

bool RawDescriptorHeap::isAvailable() const noexcept
{
    return heap_ != nullptr;
}

ID3D12DescriptorHeap *RawDescriptorHeap::getHeap() noexcept
{
    return heap_.Get();
}

UINT RawDescriptorHeap::getDescSize() const noexcept
{
    return descIncSize_;
}

D3D12_CPU_DESCRIPTOR_HANDLE RawDescriptorHeap::getCPUHandle(
    int index) const noexcept
{
    auto ret = cpuStart_;
    return ret.Offset(index, descIncSize_);
}

D3D12_GPU_DESCRIPTOR_HANDLE RawDescriptorHeap::getGPUHandle(int index) const noexcept
{
    assert(gpuStart_.ptr);
    auto ret = gpuStart_;
    return ret.Offset(index, descIncSize_);
}

void RawDescriptorHeap::initialize(
    ID3D12Device              *device,
    int                        size,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    bool                       isShaderVisible)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.NodeMask = 0;
    desc.Flags    = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE :
                                      D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NumDescriptors = static_cast<UINT>(size);
    desc.Type           = type;

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create d3d12 descriptor heap",
        device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.GetAddressOf())));

    descIncSize_ = device->GetDescriptorHandleIncrementSize(type);

    cpuStart_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        heap_->GetCPUDescriptorHandleForHeapStart());

    if(isShaderVisible)
    {
        gpuStart_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(
            heap_->GetGPUDescriptorHandleForHeapStart());
    }
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
