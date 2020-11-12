#pragma once

AGZ_D3D12_BEGIN

template<typename Vertex>
VertexBuffer<Vertex>::VertexBuffer(VertexBuffer &&other) noexcept
    : VertexBuffer()
{
    this->swap(other);
}

template<typename Vertex>
VertexBuffer<Vertex> &VertexBuffer<Vertex>::operator=(VertexBuffer &&other) noexcept
{
    this->swap(other);
    return *this;
}

template<typename Vertex>
void VertexBuffer<Vertex>::swap(VertexBuffer &other) noexcept
{
    buffer_.swap(other.buffer_);
    std::swap(vertexCount_, other.vertexCount_);
}

template<typename Vertex>
bool VertexBuffer<Vertex>::isAvailable() const noexcept
{
    return buffer_.isAvailable();
}

template<typename Vertex>
void VertexBuffer<Vertex>::destroy()
{
    buffer_.destroy();
    vertexCount_ = 0;
}

template<typename Vertex>
void VertexBuffer<Vertex>::initializeDefault(
    ResourceManager      &rscMgr,
    size_t                vertexCount,
    D3D12_RESOURCE_STATES initState)
{
    vertexCount_ = 0;
    buffer_.initializeDefault(rscMgr, sizeof(Vertex) * vertexCount, initState);
    vertexCount_ = vertexCount;
}

template<typename Vertex>
void VertexBuffer<Vertex>::initializeUpload(
    ResourceManager &rscMgr,
    size_t           vertexCount)
{
    vertexCount_ = 0;
    buffer_.initializeUpload(rscMgr, sizeof(Vertex) * vertexCount);
    vertexCount_ = vertexCount;
}

template<typename Vertex>
ID3D12Resource *VertexBuffer<Vertex>::getResource() const noexcept
{
    return buffer_.getResource();
}

template<typename Vertex>
size_t VertexBuffer<Vertex>::getByteSize() const noexcept
{
    return buffer_.getByteSize();
}

template<typename Vertex>
UINT VertexBuffer<Vertex>::getVertexCount() const noexcept
{
    return static_cast<UINT>(vertexCount_);
}

template<typename Vertex>
D3D12_VERTEX_BUFFER_VIEW VertexBuffer<Vertex>::getView() const noexcept
{
    return D3D12_VERTEX_BUFFER_VIEW{
        .BufferLocation = getResource()->GetGPUVirtualAddress(),
        .SizeInBytes    = static_cast<UINT>(getByteSize()),
        .StrideInBytes  = sizeof(Vertex)
    };
}

template<typename Vertex>
Buffer &VertexBuffer<Vertex>::getBuffer() noexcept
{
    return buffer_;
}

template<typename Vertex>
const Buffer &VertexBuffer<Vertex>::getBuffer() const noexcept
{
    return buffer_;
}

template<typename Struct>
ConstantBuffer<Struct>::ConstantBuffer(ConstantBuffer &&other) noexcept
    : ConstantBuffer()
{
    this->swap(other);
}

template<typename Struct>
ConstantBuffer<Struct> &ConstantBuffer<Struct>::operator=(ConstantBuffer &&other) noexcept
{
    this->swap(other);
    return *this;
}

template<typename Struct>
void ConstantBuffer<Struct>::swap(ConstantBuffer &other) noexcept
{
    buffer_.swap(other.buffer_);
    std::swap(count_, other.count_);
}

template<typename Struct>
bool ConstantBuffer<Struct>::isAvailable() const noexcept
{
    return buffer_.isAvailable();
}

template<typename Struct>
void ConstantBuffer<Struct>::destroy()
{
    buffer_.destroy();
    count_ = 0;
}

template<typename Struct>
void ConstantBuffer<Struct>::initializeUpload(
    ResourceManager &rscMgr,
    size_t           count)
{
    constexpr size_t PADDED_SIZE = padTo256(sizeof(Struct));
    count_ = 0;
    buffer_.initializeUpload(rscMgr, count * PADDED_SIZE);
    count_ = count;
}

template<typename Struct>
ID3D12Resource *ConstantBuffer<Struct>::getResource() const noexcept
{
    return buffer_.getResource();
}

template<typename Struct>
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer<Struct>::getGPUVirtualAddress(
    size_t index) const noexcept
{
    constexpr size_t PADDED_SIZE = padTo256(sizeof(Struct));
    return getResource()->GetGPUVirtualAddress() + PADDED_SIZE * index;
}

template<typename Struct>
size_t ConstantBuffer<Struct>::getCount() const noexcept
{
    return count_;
}

template<typename Struct>
D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer<Struct>::getView(size_t index) const noexcept
{
    constexpr size_t PADDED_SIZE = padTo256(sizeof(Struct));
    const auto start = getResource()->GetGPUVirtualAddress();
    return D3D12_CONSTANT_BUFFER_VIEW_DESC{
        .BufferLocation = start + PADDED_SIZE * index,
        .SizeInBytes    = PADDED_SIZE
    };
}

template<typename Struct>
void ConstantBuffer<Struct>::updateData(size_t index, const Struct &data)
{
    const size_t PADDED_SIZE = padTo256(sizeof(Struct));
    const size_t offset = PADDED_SIZE * index;
    buffer_.updateData(offset, sizeof(Struct), &data);
}

template<typename Struct>
constexpr size_t ConstantBuffer<Struct>::padTo256(size_t unpadded) noexcept
{
    return (unpadded + 255) & ~size_t(255);
}

AGZ_D3D12_END
