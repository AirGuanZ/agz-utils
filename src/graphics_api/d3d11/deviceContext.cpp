#include <agz/utility/graphics_api/d3d11/deviceContext.h>

AGZ_D3D11_BEGIN

void DeviceContext::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    d3dDeviceContext->IASetPrimitiveTopology(topology);
}

void DeviceContext::setInputLayout(ID3D11InputLayout *inputLayout)
{
    d3dDeviceContext->IASetInputLayout(inputLayout);
}

void DeviceContext::setInputLayout(const ComPtr<ID3D11InputLayout> &inputLayout)
{
    setInputLayout(inputLayout.Get());
}

void DeviceContext::draw(UINT vtxCnt, UINT vtxOffset)
{
    d3dDeviceContext->Draw(vtxCnt, vtxOffset);
}

void DeviceContext::drawIndexed(UINT idxCnt, UINT idxOffset, UINT vtxOffset)
{
    d3dDeviceContext->DrawIndexed(idxCnt, idxOffset, vtxOffset);
}

void DeviceContext::dispatch(UINT x, UINT y, UINT z)
{
    d3dDeviceContext->Dispatch(x, y, z);
}

AGZ_D3D11_END
