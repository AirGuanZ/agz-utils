#include <agz/utility/graphics_api/d3d11/vertexArrayObject.h>

AGZ_D3D11_BEGIN

VertexArrayObject::VertexArrayObject()
{
    primTopology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    idxFmt_ = DXGI_FORMAT_UNKNOWN;
}

void VertexArrayObject::setPrimitiveTopology(
    D3D11_PRIMITIVE_TOPOLOGY topology)
{
    primTopology_ = topology;
}

void VertexArrayObject::setInputLayout(ComPtr<ID3D11InputLayout> inputLayout)
{
    inputLayout_.Swap(inputLayout);
}

void VertexArrayObject::setVertexBuffer(
    UINT inputSlot, ComPtr<ID3D11Buffer> buf, UINT stride)
{
    slot2VtxBuf_[inputSlot] = { std::move(buf), stride };
}

void VertexArrayObject::setIndexBuffer(
    ComPtr<ID3D11Buffer> buf, DXGI_FORMAT format)
{
    idxBuf_ = std::move(buf);
    idxFmt_ = format;
}

void VertexArrayObject::bind() const
{
    deviceContext.setPrimitiveTopology(primTopology_);
    if(inputLayout_)
        deviceContext.setInputLayout(inputLayout_);

    const UINT offset = 0;
    for(auto &p : slot2VtxBuf_)
    {
        deviceContext.d3dDeviceContext->IASetVertexBuffers(
            p.first, 1, p.second.buf.GetAddressOf(),
            &p.second.stride, &offset);
    }

    if(idxBuf_)
    {
        deviceContext.d3dDeviceContext->IASetIndexBuffer(
            idxBuf_.Get(), idxFmt_, 0);
    }
}

void VertexArrayObject::unbind() const
{
    if(inputLayout_)
        deviceContext.setInputLayout(nullptr);

    const UINT strideAndOffset = 0;
    for(auto &p : slot2VtxBuf_)
    {
        ID3D11Buffer *NULL_VTX_BUF = nullptr;
        deviceContext.d3dDeviceContext->IASetVertexBuffers(
            p.first, 1, &NULL_VTX_BUF, &strideAndOffset, &strideAndOffset);
    }

    if(idxBuf_)
    {
        deviceContext.d3dDeviceContext->IASetIndexBuffer(
            nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }
}

AGZ_D3D11_END
