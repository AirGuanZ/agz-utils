#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/queueWaiter.h>

AGZ_D3D12_BEGIN

CommandQueueWaiter::CommandQueueWaiter(ID3D12Device *device)
{
    AGZ_D3D12_CHECK_HR(
        device->CreateFence(
            0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));

    fenceValue_ = 0;
}

CommandQueueWaiter::CommandQueueWaiter(CommandQueueWaiter &&other) noexcept
{
    fence_.Swap(other.fence_);
    fenceValue_ = other.fenceValue_; other.fenceValue_ = 0;
}

CommandQueueWaiter &CommandQueueWaiter::operator=(
    CommandQueueWaiter &&other) noexcept
{
    fence_.Reset();

    fence_.Swap(other.fence_);
    fenceValue_ = other.fenceValue_; other.fenceValue_ = 0;

    return *this;
}

void CommandQueueWaiter::waitIdle(ID3D12CommandQueue *cmdQueue)
{
    ++fenceValue_;
    cmdQueue->Signal(fence_.Get(), fenceValue_);
    if(fence_->GetCompletedValue() < fenceValue_)
        fence_->SetEventOnCompletion(fenceValue_, nullptr);
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
