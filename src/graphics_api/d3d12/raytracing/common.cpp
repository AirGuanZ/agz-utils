#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/raytracing/common.h>

AGZ_D3D12_RT_BEGIN

ComPtr<ID3D12Device4> getDXRDevice(ID3D12Device *device)
{
    ComPtr<ID3D12Device4> result;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to get DXR device from D3D12 device",
        device->QueryInterface(result.GetAddressOf()));
    return result;
}

ComPtr<ID3D12Device4> getDXRDevice(const ComPtr<ID3D12Device> &device)
{
    return getDXRDevice(device.Get());
}

ComPtr<ID3D12GraphicsCommandList4> getDXRCommandList(
    ID3D12CommandList *cmdList)
{
    ComPtr<ID3D12GraphicsCommandList4> result;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to get DXR command list from graphics command list",
        cmdList->QueryInterface(result.GetAddressOf()));
    return result;
}

ComPtr<ID3D12GraphicsCommandList4> getDXRCommandList(
    const ComPtr<ID3D12CommandList> &cmdList)
{
    return getDXRCommandList(cmdList.Get());
}

AGZ_D3D12_RT_END

#endif // #ifdef AGZ_ENABLE_D3D12
