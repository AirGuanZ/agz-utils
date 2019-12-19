#pragma once

#include <agz/utility/d3d11/Common.h>

AGZ_D3D11_BEGIN

namespace RenderState
{

    inline void Draw(D3D11_PRIMITIVE_TOPOLOGY topology, UINT vertexCount, UINT startVertex = 0)
    {
        gDeviceContext->IASetPrimitiveTopology(topology);
        gDeviceContext->Draw(vertexCount, startVertex);
    }

    inline void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, UINT indexCount, UINT startIndex = 0, UINT baseVertex = 0)
    {
        gDeviceContext->IASetPrimitiveTopology(topology);
        gDeviceContext->DrawIndexed(indexCount, startIndex, baseVertex);
    }

} // namespace RenderState

AGZ_D3D11_END
