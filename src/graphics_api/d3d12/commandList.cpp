#ifdef AGZ_ENABLE_D3D12

#include <agz/utility/graphics_api/d3d12/commandList.h>

AGZ_D3D12_BEGIN

CommandList::CommandList(ID3D12Device *device, D3D12_COMMAND_LIST_TYPE type)
    : CommandList()
{
    initialize(device, type);
}

CommandList::CommandList(CommandList &&other) noexcept
    : CommandList()
{
    swap(other);
}

CommandList &CommandList::operator=(CommandList &&other) noexcept
{
    swap(other);
    return *this;
}

void CommandList::swap(CommandList &other) noexcept
{
    allocator_.Swap(other.allocator_);
    cmdList_.Swap(other.cmdList_);
}

void CommandList::initialize(ID3D12Device *device, D3D12_COMMAND_LIST_TYPE type)
{
    destroy();

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create command allocator",
        device->CreateCommandAllocator(
            type, IID_PPV_ARGS(allocator_.GetAddressOf())));

    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create command list",
        device->CreateCommandList(
            0, type, allocator_.Get(), nullptr,
            IID_PPV_ARGS(cmdList_.GetAddressOf())));
}

bool CommandList::isAvailable() const noexcept
{
    return cmdList_.Get() != nullptr;
}

void CommandList::destroy()
{
    allocator_.Reset();
    cmdList_.Reset();
}

ID3D12GraphicsCommandList4 *CommandList::operator->() noexcept
{
    return cmdList_.Get();
}

ID3D12GraphicsCommandList4 *CommandList::getRawCommandList() noexcept
{
    return cmdList_.Get();
}

void CommandList::executeOnQueue(ID3D12CommandQueue *queue)
{
    assert(isAvailable());
    cmdList_->Close();
    ID3D12CommandList *raw = cmdList_.Get();
    queue->ExecuteCommandLists(1, &raw);
}

void CommandList::executeOnQueue(const ComPtr<ID3D12CommandQueue> &queue)
{
    executeOnQueue(queue.Get());
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
