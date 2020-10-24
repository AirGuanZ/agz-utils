#pragma once

#include <d3dx12.h>

#include <agz/utility/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class RawDescriptorHeap : public misc::uncopyable_t
{
public:

    RawDescriptorHeap();

    RawDescriptorHeap(
        ID3D12Device              *device,
        int                        size,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        bool                       isShaderVisible);

    void swap(RawDescriptorHeap &other) noexcept;

    bool isAvailable() const noexcept;

    ID3D12DescriptorHeap *getHeap() noexcept;

    UINT getDescSize() const noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle(int index) const noexcept;

    D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle(int index) const noexcept;

private:
    
    void initialize(
        ID3D12Device              *device,
        int                        size,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        bool                       isShaderVisible);

    UINT descIncSize_;

    ComPtr<ID3D12DescriptorHeap> heap_;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuStart_;
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuStart_;
};

AGZ_D3D12_END
