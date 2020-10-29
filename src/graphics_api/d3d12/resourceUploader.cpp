#ifdef AGZ_ENABLE_D3D12

#include <d3dx12.h>

#include <agz/utility/graphics_api/d3d12/resourceUploader.h>

AGZ_D3D12_BEGIN

ResourceUploader::ResourceUploader(
    ComPtr<ID3D12Device>       device,
    ComPtr<ID3D12CommandQueue> copyCmdQueue,
    size_t                     ringCmdListCount,
    ResourceManager           &rscMgr)
    : device_(std::move(device)),
      cmdQueue_(std::move(copyCmdQueue)),
      newFenceValue_(0),
      cmdListIdx_(0),
      rscMgr_(rscMgr)
{
    assert(cmdQueue_->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_COPY);

    AGZ_D3D12_CHECK_HR(
        device_->CreateFence(
            0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));
    
    cmdLists_.resize(ringCmdListCount);
    for(size_t i = 0; i < cmdLists_.size(); ++i)
    {
        auto &c = cmdLists_[i];
        c.expectedFenceValue_ = 0;

        AGZ_D3D12_CHECK_HR(
            device_->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_COPY,
                IID_PPV_ARGS(c.cmdAlloc.GetAddressOf())));

        AGZ_D3D12_CHECK_HR(
            device_->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_COPY,
                c.cmdAlloc.Get(), nullptr,
                IID_PPV_ARGS(c.cmdList.GetAddressOf())));

        if(i)
            c.cmdList->Close();
    }

    uploadRscs_.resize(ringCmdListCount);
}

ResourceUploader::~ResourceUploader()
{
    sync();
}

void ResourceUploader::upload(
    Buffer     &buffer,
    const void *data,
    size_t      dataBytes)
{
    if(dataBytes == 0)
        dataBytes = buffer.getByteSize();

    auto upload = rscMgr_.create(
        D3D12_HEAP_TYPE_UPLOAD,
        CD3DX12_RESOURCE_DESC::Buffer(dataBytes),
        D3D12_RESOURCE_STATE_GENERIC_READ);

    D3D12_SUBRESOURCE_DATA subrscData;
    subrscData.pData      = data;
    subrscData.RowPitch   = dataBytes;
    subrscData.SlicePitch = dataBytes;

    UpdateSubresources(
        cmdLists_[cmdListIdx_].cmdList.Get(),
        buffer.getResource(),
        upload->resource.Get(),
        0, 0, 1, &subrscData);

    uploadRscs_[cmdListIdx_].emplace_back();
    uploadRscs_[cmdListIdx_].back().rsc = std::move(upload);
}

void ResourceUploader::submit()
{
    // submit

    auto &c = cmdLists_[cmdListIdx_];
    c.cmdList->Close();

    ID3D12CommandList *rawCmdList = c.cmdList.Get();
    cmdQueue_->ExecuteCommandLists(1, &rawCmdList);

    cmdQueue_->Signal(fence_.Get(), ++newFenceValue_);
    c.expectedFenceValue_ = newFenceValue_;

    // new

    cmdListIdx_ = (cmdListIdx_ + 1) % cmdLists_.size();

    sync(cmdListIdx_);

    auto &nc = cmdLists_[cmdListIdx_];
    nc.cmdAlloc->Reset();
    nc.cmdList->Reset(nc.cmdAlloc.Get(), nullptr);
}

void ResourceUploader::sync()
{
    for(size_t i = 0; i < cmdLists_.size(); ++i)
        sync(i);
}

void ResourceUploader::sync(size_t index)
{
    auto &c = cmdLists_[index];
    fence_->SetEventOnCompletion(c.expectedFenceValue_, nullptr);
    uploadRscs_[index].clear();
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
