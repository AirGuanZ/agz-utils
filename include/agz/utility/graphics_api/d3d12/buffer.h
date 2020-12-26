#pragma once

#include <agz/utility/graphics_api/d3d12/uniqueResource.h>

AGZ_D3D12_BEGIN

class ResourceManager;

class Buffer : public misc::uncopyable_t
{
public:

    Buffer();

    Buffer(Buffer &&other) noexcept;

    Buffer &operator=(Buffer &&other) noexcept;

    void initializeDefault(
        ResourceManager      &rscMgr,
        size_t                byteSize,
        D3D12_RESOURCE_STATES initState);

    void initializeUpload(
        ResourceManager &rscMgr,
        size_t           byteSize);

    bool isAvailable() const noexcept;

    void destroy();

    ID3D12Resource *getResource() const noexcept;

    D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const noexcept;

    size_t getByteSize() const noexcept;

    void swap(Buffer &other) noexcept;

    void updateData(size_t offset, size_t bytes, const void *data);

protected:

    UniqueResource resource_;
    size_t         byteSize_;
    void          *mappedData_;
};

template<typename Vertex>
class VertexBuffer : public misc::uncopyable_t
{
public:

    VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&other) noexcept;

    VertexBuffer &operator=(VertexBuffer &&other) noexcept;

    void swap(VertexBuffer &other) noexcept;

    bool isAvailable() const noexcept;

    void destroy();

    void initializeDefault(
        ResourceManager      &rscMgr,
        size_t                vertexCount,
        D3D12_RESOURCE_STATES initState);

    void initializeUpload(
        ResourceManager &rscMgr,
        size_t           vertexCount);

    ID3D12Resource *getResource() const noexcept;

    size_t getByteSize() const noexcept;

    UINT getVertexCount() const noexcept;

    D3D12_VERTEX_BUFFER_VIEW getView() const noexcept;

    Buffer &getBuffer() noexcept;

    const Buffer &getBuffer() const noexcept;

private:

    Buffer buffer_;
    size_t vertexCount_ = 0;
};

template<typename Index>
class IndexBuffer : public misc::uncopyable_t
{
public:

    IndexBuffer() = default;

    IndexBuffer(IndexBuffer &&other) noexcept;

    IndexBuffer &operator=(IndexBuffer &&other) noexcept;

    void swap(IndexBuffer &other) noexcept;

    bool isAvailable() const noexcept;

    void destroy();

    void initializeDefault(
        ResourceManager      &rscMgr,
        size_t                indexCount,
        D3D12_RESOURCE_STATES initState);

    void initializeUpload(
        ResourceManager &rscMgr,
        size_t           indexCount);

    ID3D12Resource *getResource() const noexcept;

    size_t getByteSize() const noexcept;

    UINT getIndexCount() const noexcept;

    D3D12_INDEX_BUFFER_VIEW getView() const noexcept;

    Buffer &getBuffer() noexcept;

    const Buffer &getBuffer() const noexcept;

private:

    static_assert(
        std::is_same_v<Index, uint16_t> ||
        std::is_same_v<Index, uint32_t>);

    Buffer buffer_;
    size_t count_ = 0;
};

template<typename Struct>
class ConstantBuffer : public misc::uncopyable_t
{
public:

    ConstantBuffer() = default;

    ConstantBuffer(ConstantBuffer &&other) noexcept;

    ConstantBuffer &operator=(ConstantBuffer &&other) noexcept;

    void swap(ConstantBuffer &other) noexcept;

    bool isAvailable() const noexcept;

    void destroy();

    void initializeUpload(
        ResourceManager &rscMgr,
        size_t           count);

    ID3D12Resource *getResource() const noexcept;

    D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress(size_t index) const noexcept;

    size_t getCount() const noexcept;

    D3D12_CONSTANT_BUFFER_VIEW_DESC getView(size_t index) const noexcept;

    void updateData(size_t index, const Struct &data);

private:

    static constexpr size_t padTo256(size_t unpadded) noexcept;

    Buffer buffer_;
    size_t count_ = 0;
};

AGZ_D3D12_END

#include <agz/utility/graphics_api/d3d12/buffer.inl>
