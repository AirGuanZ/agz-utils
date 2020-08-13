#pragma once

#include "common.h"

AGZ_D3D11_BEGIN

ComPtr<ID3D11Buffer> createBuffer(
    const D3D11_BUFFER_DESC &desc, const void *initData);

template<typename Vertex>
class VertexBuffer
{
    UINT vertexCount_ = 0;
    ComPtr<ID3D11Buffer> buf_;

public:

    void initialize(size_t vertexCount, const Vertex *initData);

    void initialize(size_t vertexCount, ComPtr<ID3D11Buffer> buf);

    bool is_available() const noexcept;

    void destroy();

    UINT getVertexCount() const noexcept;

    void bind(int inputSlot) const;

    void unbind(int inputSlot) const;
};

template<typename Index>
class IndexBuffer
{
    UINT indexCount_ = 0;
    ComPtr<ID3D11Buffer> buf_;

public:

    void initialize(size_t indexCount, const Index *initData);

    void initialize(size_t indexCount, ComPtr<ID3D11Buffer> buf);

    bool is_available() const noexcept;

    void destroy();

    UINT getIndexCount() const noexcept;

    void bind() const;

    void unbind() const;
};

template<typename Struct>
class ConstantBuffer
{
    ComPtr<ID3D11Buffer> buf_;

public:

    void initialize();

    void initialize(bool dynamic, const Struct &initData);

    void initialize(ComPtr<ID3D11Buffer> buf);

    bool isAvailable() const noexcept;

    void destroy();

    void update(const Struct &data);

    ID3D11Buffer *get() const noexcept;

    ID3D11Buffer *const *getAddr() const noexcept;

    operator ID3D11Buffer *() const noexcept;
};

inline ComPtr<ID3D11Buffer> createBuffer(
    const D3D11_BUFFER_DESC &desc, const void *initData)
{
    D3D11_SUBRESOURCE_DATA subrsc;
    subrsc.pSysMem          = initData;
    subrsc.SysMemPitch      = 0;
    subrsc.SysMemSlicePitch = 0;

    ComPtr<ID3D11Buffer> buf;
    if(FAILED(gDevice->CreateBuffer(
        &desc, initData ? &subrsc : nullptr, buf.GetAddressOf())))
        throw D3D11Exception("failed to create d3d11 buffer");

    return buf;
}

template<typename Vertex>
void VertexBuffer<Vertex>::initialize(
    size_t vertexCount, const Vertex *initData)
{
    this->initialize(
        vertexCount,
        d3d11::createBuffer(
            CD3D11_BUFFER_DESC(
                static_cast<UINT>(sizeof(Vertex) * vertexCount),
                D3D11_BIND_VERTEX_BUFFER,
                D3D11_USAGE_IMMUTABLE), initData));
}

template<typename Vertex>
void VertexBuffer<Vertex>::initialize(
    size_t vertexCount, ComPtr<ID3D11Buffer> buf)
{
    buf_ = std::move(buf);
    vertexCount_ = static_cast<UINT>(vertexCount);
}

template<typename Vertex>
bool VertexBuffer<Vertex>::is_available() const noexcept
{
    return buf_ != nullptr;
}

template<typename Vertex>
void VertexBuffer<Vertex>::destroy()
{
    buf_.Reset();
    vertexCount_ = 0;
}

template<typename Vertex>
UINT VertexBuffer<Vertex>::getVertexCount() const noexcept
{
    return vertexCount_;
}

template<typename Vertex>
void VertexBuffer<Vertex>::bind(int inputSlot) const
{
    const UINT stride = sizeof(Vertex), offset = 0;
    gDeviceContext->IASetVertexBuffers(
        inputSlot, 1, buf_.GetAddressOf(), &stride, &offset);
}

template<typename Vertex>
void VertexBuffer<Vertex>::unbind(int inputSlot) const
{
    const UINT strideAndOffset = 0;
    ID3D11Buffer *EMPTY_BUFFER = nullptr;
    gDeviceContext->IASetVertexBuffers(
        inputSlot, 1, &EMPTY_BUFFER, &strideAndOffset, &strideAndOffset);
}

template<typename Index>
void IndexBuffer<Index>::initialize(
    size_t indexCount, const Index *initData)
{
    this->initialize(
        indexCount,
        d3d11::createBuffer(
            CD3D11_BUFFER_DESC(
                static_cast<UINT>(sizeof(Index) * indexCount),
                D3D11_BIND_INDEX_BUFFER,
                D3D11_USAGE_IMMUTABLE), initData));
}

template<typename Index>
void IndexBuffer<Index>::initialize(
    size_t indexCount, ComPtr<ID3D11Buffer> buf)
{
    indexCount_ = static_cast<UINT>(indexCount);
    buf_ = std::move(buf);
}

template<typename Index>
bool IndexBuffer<Index>::is_available() const noexcept
{
    return buf_ != nullptr;
}

template<typename Index>
void IndexBuffer<Index>::destroy()
{
    indexCount_ = 0;
    buf_.Reset();
}

template<typename Index>
UINT IndexBuffer<Index>::getIndexCount() const noexcept
{
    return indexCount_;
}

template<typename Index>
void IndexBuffer<Index>::bind() const
{
    static_assert(
        std::is_same_v<Index, uint16_t> ||
        std::is_same_v<Index, uint32_t>);

    constexpr DXGI_FORMAT FMT =
        std::is_same_v<Index, uint16_t> ?
        DXGI_FORMAT_R16_UINT :
        DXGI_FORMAT_R32_UINT;

    gDeviceContext->IASetIndexBuffer(buf_.Get(), FMT, 0);
}

template<typename Index>
void IndexBuffer<Index>::unbind() const
{
    gDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}

template<typename Struct>
void ConstantBuffer<Struct>::initialize()
{
    buf_ = createBuffer(
        CD3D11_BUFFER_DESC(
            sizeof(Struct), D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE,
            0, 0), nullptr);
}

template<typename Struct>
void ConstantBuffer<Struct>::initialize(
    bool dynamic, const Struct &initData)
{
    buf_ = createBuffer(
        CD3D11_BUFFER_DESC(
            sizeof(Struct), D3D11_BIND_CONSTANT_BUFFER,
            dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE,
            dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
            0, 0), &initData);
}

template<typename Struct>
void ConstantBuffer<Struct>::initialize(ComPtr<ID3D11Buffer> buf)
{
    buf_ = std::move(buf);
}

template<typename Struct>
bool ConstantBuffer<Struct>::isAvailable() const noexcept
{
    return buf_ != nullptr;
}

template<typename Struct>
void ConstantBuffer<Struct>::destroy()
{
    buf_.Reset();
}

template<typename Struct>
void ConstantBuffer<Struct>::update(const Struct &data)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubrsc;
    if(FAILED(gDeviceContext->Map(
        buf_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubrsc)))
        throw D3D11Exception("failed to map constant buffer to memory");
    std::memcpy(mappedSubrsc.pData, &data, sizeof(Struct));
    gDeviceContext->Unmap(buf_.Get(), 0);
}

template<typename Struct>
ID3D11Buffer *ConstantBuffer<Struct>::get() const noexcept
{
    return buf_.Get();
}

template<typename Struct>
ID3D11Buffer *const *ConstantBuffer<Struct>::getAddr() const noexcept
{
    return buf_.GetAddressOf();
}

template<typename Struct>
ConstantBuffer<Struct>::operator ID3D11Buffer*() const noexcept
{
    return buf_.Get();
}

AGZ_D3D11_END
