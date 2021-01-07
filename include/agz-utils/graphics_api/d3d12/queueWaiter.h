#pragma once

#include <agz-utils/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class CommandQueueWaiter : public misc::uncopyable_t
{
    ComPtr<ID3D12Fence> fence_;

    UINT64 fenceValue_;

public:

    explicit CommandQueueWaiter(ID3D12Device *device);

    CommandQueueWaiter(CommandQueueWaiter &&other) noexcept;

    CommandQueueWaiter &operator=(CommandQueueWaiter &&other) noexcept;

    void waitIdle(ID3D12CommandQueue *cmdQueue);
};

AGZ_D3D12_END
