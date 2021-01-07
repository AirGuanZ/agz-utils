#pragma once

#include <agz-utils/graphics_api/d3d12/window.h>

AGZ_D3D12_BEGIN

class FrameFence : public misc::uncopyable_t
{
public:

    FrameFence(
        ID3D12Device       *device,
        ID3D12CommandQueue *cmdQueue,
        int                frameCount)
        : cmdQueue_(cmdQueue), frameIndex_(0)
    {
        frameEndResources_.resize(frameCount);
        for(auto &f : frameEndResources_)
        {
            AGZ_D3D12_CHECK_HR(
                device->CreateFence(
                    0, D3D12_FENCE_FLAG_NONE,
                    IID_PPV_ARGS(f.fence.GetAddressOf())));
            f.lastSignaledFenceValue = 0;
        }
    }

    void startFrame(int frameIndex)
    {
        auto &f = frameEndResources_[frameIndex];
        f.fence->SetEventOnCompletion(f.lastSignaledFenceValue, nullptr);
        frameIndex_ = frameIndex;
    }

    void endFrame()
    {
        auto &f = frameEndResources_[frameIndex_];
        cmdQueue_->Signal(f.fence.Get(), ++f.lastSignaledFenceValue);
    }

private:

    struct FrameEndResource
    {
        ComPtr<ID3D12Fence> fence;
        UINT64 lastSignaledFenceValue = 0;
    };

    ID3D12CommandQueue *cmdQueue_;

    int frameIndex_;
    std::vector<FrameEndResource> frameEndResources_;
};

AGZ_D3D12_END
