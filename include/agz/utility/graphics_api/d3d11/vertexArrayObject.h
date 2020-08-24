#pragma once

#include <map>

#include "buffer.h"

AGZ_D3D11_BEGIN

class VertexArrayObject : public misc::uncopyable_t
{
public:

    VertexArrayObject();

    void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

    void setInputLayout(ComPtr<ID3D11InputLayout> inputLayout);

    template<typename Vertex>
    void setVertexBuffer(UINT inputSlot, VertexBuffer<Vertex> &buf);

    void setVertexBuffer(UINT inputSlot, ComPtr<ID3D11Buffer> buf, UINT stride);

    template<typename Index>
    void setIndexBuffer(IndexBuffer<Index> &buf);

    void setIndexBuffer(ComPtr<ID3D11Buffer> buf, DXGI_FORMAT format);

    void bind() const;

    void unbind() const;

private:

    D3D11_PRIMITIVE_TOPOLOGY primTopology_;
    ComPtr<ID3D11InputLayout> inputLayout_;

    struct VertexBufferRecord
    {
        ComPtr<ID3D11Buffer> buf;
        UINT stride = 0;
    };

    std::map<UINT, VertexBufferRecord> slot2VtxBuf_;

    ComPtr<ID3D11Buffer> idxBuf_;
    DXGI_FORMAT idxFmt_;
};

template<typename Vertex>
void VertexArrayObject::setVertexBuffer(
    UINT inputSlot, VertexBuffer<Vertex> &buf)
{
    this->setVertexBuffer(inputSlot, buf.getBuffer(), sizeof(Vertex));
}

template<typename Index>
void VertexArrayObject::setIndexBuffer(IndexBuffer<Index> &buf)
{
    this->setIndexBuffer(buf.getBuffer(), IndexBuffer<Index>::getFormat());
}

AGZ_D3D11_END
