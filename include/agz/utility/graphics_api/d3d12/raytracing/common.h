#pragma once

#include "../common.h"

#define AGZ_D3D12_RT_BEGIN namespace agz::d3d12::rt {
#define AGZ_D3D12_RT_END   }

AGZ_D3D12_RT_BEGIN

ComPtr<ID3D12Device4> getDXRDevice(
    ID3D12Device *device);

ComPtr<ID3D12Device4> getDXRDevice(
    const ComPtr<ID3D12Device> &device);

ComPtr<ID3D12GraphicsCommandList4> getDXRCommandList(
    ID3D12CommandList *cmdList);

ComPtr<ID3D12GraphicsCommandList4> getDXRCommandList(
    const ComPtr<ID3D12CommandList> &cmdList);

AGZ_D3D12_RT_END
