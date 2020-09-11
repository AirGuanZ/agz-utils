#pragma once

#include "./common.h"

AGZ_D3D11_BEGIN

class DeviceContext
{
public:

    ID3D11DeviceContext *d3dDeviceContext;

    void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

    void setInputLayout(ID3D11InputLayout *inputLayout);

    void setInputLayout(const ComPtr<ID3D11InputLayout> &inputLayout);

    void draw(UINT vtxCnt, UINT vtxOffset);

    void drawIndexed(UINT idxCnt, UINT idxOffset, UINT vtxOffset);

    void dispatch(UINT x, UINT y = 1, UINT z = 1);

    ID3D11DeviceContext *operator->() noexcept { return d3dDeviceContext; }
};

inline DeviceContext deviceContext = { nullptr };

AGZ_D3D11_END
