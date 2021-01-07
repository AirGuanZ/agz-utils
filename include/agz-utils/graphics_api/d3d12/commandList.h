#pragma once

#include <agz-utils/graphics_api/d3d12/common.h>

AGZ_D3D12_BEGIN

class CommandList : public misc::uncopyable_t
{
public:

    CommandList() = default;

    CommandList(ID3D12Device *device, D3D12_COMMAND_LIST_TYPE type);

    CommandList(CommandList &&other) noexcept;

    CommandList &operator=(CommandList &&other) noexcept;

    void swap(CommandList &other) noexcept;

    void initialize(ID3D12Device *device, D3D12_COMMAND_LIST_TYPE type);

    bool isAvailable() const noexcept;

    void destroy();

    ID3D12GraphicsCommandList4 *operator->() noexcept;

    ID3D12GraphicsCommandList4 *getRawCommandList() noexcept;

    void executeOnQueue(ID3D12CommandQueue *queue);

    void executeOnQueue(const ComPtr<ID3D12CommandQueue> &queue);

private:

    ComPtr<ID3D12CommandAllocator>     allocator_;
    ComPtr<ID3D12GraphicsCommandList4> cmdList_;
};

AGZ_D3D12_END
