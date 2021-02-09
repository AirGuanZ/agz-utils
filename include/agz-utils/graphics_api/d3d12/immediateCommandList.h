#pragma once

#include <agz-utils/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class ImmediateCommandList : public misc::uncopyable_t
{
public:

    ImmediateCommandList();

    ImmediateCommandList(ImmediateCommandList &&) noexcept = default;

    ImmediateCommandList &operator=(ImmediateCommandList &&) noexcept = default;

    explicit ImmediateCommandList(ComPtr<ID3D12CommandQueue> queue);

    template<typename Func>
    void execute(Func &&func);

    void executeBarriers(const std::vector<D3D12_RESOURCE_BARRIER> &barriers);

private:

    ComPtr<ID3D12CommandQueue> queue_;

    ComPtr<ID3D12Fence> fence_;
    UINT64              nextFenceValue_ = 0;

    ComPtr<ID3D12CommandAllocator>    alloc_;
    ComPtr<ID3D12GraphicsCommandList> cmdList_;
};

inline ImmediateCommandList::ImmediateCommandList(
    ComPtr<ID3D12CommandQueue> queue)
        : queue_(std::move(queue))
{
    assert(queue_);

    ComPtr<ID3D12Device> device;
    AGZ_D3D12_CHECK_HR(
        queue_->GetDevice(
            IID_PPV_ARGS(device.GetAddressOf())));

    const auto type = queue_->GetDesc().Type;

    AGZ_D3D12_CHECK_HR(
        device->CreateCommandAllocator(
            type, IID_PPV_ARGS(alloc_.GetAddressOf())));

    AGZ_D3D12_CHECK_HR(
        device->CreateCommandList(
            0, type, alloc_.Get(), nullptr,
            IID_PPV_ARGS(cmdList_.GetAddressOf())));

    cmdList_->Close();

    AGZ_D3D12_CHECK_HR(
        device->CreateFence(
            0, D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(fence_.GetAddressOf())));
}

template<typename Func>
void ImmediateCommandList::execute(Func &&func)
{
    alloc_->Reset();
    cmdList_->Reset(alloc_.Get(), nullptr);

    func(cmdList_.Get());

    cmdList_->Close();

    ID3D12CommandList *rawCmdList = cmdList_.Get();
    queue_->ExecuteCommandLists(1, &rawCmdList);

    queue_->Signal(fence_.Get(), ++nextFenceValue_);
    fence_->SetEventOnCompletion(nextFenceValue_, nullptr);
}

inline void ImmediateCommandList::executeBarriers(
    const std::vector<D3D12_RESOURCE_BARRIER> &barriers)
{
    this->execute([&](ID3D12GraphicsCommandList *cmdList)
    {
        cmdList->ResourceBarrier(
            static_cast<UINT>(barriers.size()), barriers.data());
    });
}

AGZ_D3D12_END
