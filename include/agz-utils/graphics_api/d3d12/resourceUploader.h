#pragma once

#include <agz-utils/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_BEGIN

class ResourceUploader : public misc::uncopyable_t
{
public:

    struct Texture2DInitData
    {
        const void *data = nullptr;
        size_t rowBytes  = 0;
    };

    ResourceUploader(
        ComPtr<ID3D12Device>       device,
        ComPtr<ID3D12CommandQueue> copyCmdQueue,
        size_t                     ringCmdListCount,
        ResourceManager           &rscMgr);

    ~ResourceUploader();

    ID3D12Device *getDevice() const noexcept;

    // dataBytes is 0 by default, which means buffer.GetBytes()
    void upload(
        Buffer     &buffer,
        const void *data,
        size_t      dataBytes = 0);

    template<typename Vertex>
    void upload(
        VertexBuffer<Vertex> &buffer,
        const Vertex         *data,
        size_t                vertexCount = 0)
    {
        this->upload(buffer.getBuffer(), data, sizeof(Vertex) * vertexCount);
    }

    void uploadTexture2D(
        ID3D12Resource          *rsc,
        const Texture2DInitData *data);

    // submit all accumulated tasks to gpu
    void submit();

    // wait for all tasks to be finished
    void sync();

    void submitAndSync();

private:

    void sync(size_t index);

    ComPtr<ID3D12Device>       device_;
    ComPtr<ID3D12CommandQueue> cmdQueue_;

    ComPtr<ID3D12Fence> fence_;
    UINT64              newFenceValue_;

    struct RingCommandList
    {
        UINT64                            expectedFenceValue_ = 0;
        ComPtr<ID3D12GraphicsCommandList> cmdList;
        ComPtr<ID3D12CommandAllocator>    cmdAlloc;
    };

    std::vector<RingCommandList> cmdLists_;
    size_t cmdListIdx_;

    struct UploadResource
    {
        UniqueResource rsc;
    };

    std::vector<std::vector<UploadResource>> uploadRscs_;

    ResourceManager &rscMgr_;
};

AGZ_D3D12_END
