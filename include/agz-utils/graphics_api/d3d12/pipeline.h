#pragma once

#include <agz-utils/misc.h>

#include <agz-utils/graphics_api/d3d12/rootSignature.h>

AGZ_D3D12_BEGIN

class PipelineBuilder : public misc::uncopyable_t
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_;

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElems_;

    ComPtr<ID3D12RootSignature> rootSignature_;

    ComPtr<ID3DBlob> sharedVertexShaderByteCode_;
    ComPtr<ID3DBlob> sharedPixelShaderByteCode_;

    template<int I>
    void setRenderTargetFormatsAux() { }

    template<int I, typename...Formats>
    void setRenderTargetFormatsAux(DXGI_FORMAT fmt, Formats...formats)
    {
        this->setRenderTargetFormat(I, fmt);
        this->setRenderTargetFormatsAux<I + 1>(formats...);
    }

public:

    PipelineBuilder();

    void addInputElement(const D3D12_INPUT_ELEMENT_DESC &elem);

    void addInputElements(const D3D12_INPUT_ELEMENT_DESC *elems, size_t cnt);

    template<int N>
    void addInputElements(const D3D12_INPUT_ELEMENT_DESC (&elems)[N])
    {
        this->addInputElements(elems, N);
    }

    void setRootSignature(ComPtr<ID3D12RootSignature> rootSignature);

    void setVertexShader(ComPtr<ID3DBlob> byteCode);

    void setPixelShader(ComPtr<ID3DBlob> byteCode);

    void setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);

    void setRenderTargetCount(int count);

    template<typename...Formats>
    void setRenderTargetFormats(Formats...formats)
    {
        this->setRenderTargetFormatsAux<0>(formats...);
    }

    void setRenderTargetFormat(int renderTarget, DXGI_FORMAT format);

    void setDepthStencilFormat(DXGI_FORMAT format);

    void setMultisample(int sampleCount, int sampleQuaility);

    void enableBlend(
        int            renderTarget,
        D3D12_BLEND    src      = D3D12_BLEND_SRC_ALPHA,
        D3D12_BLEND    dst      = D3D12_BLEND_INV_SRC_ALPHA,
        D3D12_BLEND_OP op       = D3D12_BLEND_OP_ADD,
        D3D12_BLEND    srcAlpha = D3D12_BLEND_ONE,
        D3D12_BLEND    dstAlpha = D3D12_BLEND_ZERO,
        D3D12_BLEND_OP opAlpha  = D3D12_BLEND_OP_ADD);

    void setRenderTargetWriteMask(
        int   renderTarget,
        UINT8 mask = D3D12_COLOR_WRITE_ENABLE_ALL);

    void setFillMode(D3D12_FILL_MODE fillMode);

    void setCullMode(
        D3D12_CULL_MODE cullMode,
        bool            frontCounterClockwise = false);

    void setDepthBias(
        int   bias,
        float biasClamp,
        float slopeScaledBias);

    void setDepthTest(bool test, bool write);

    void setDepthFunc(D3D12_COMPARISON_FUNC func);

    void setStencilTest(bool enabled, UINT8 readMask, UINT8 writeMask);

    void setFrontStencil(
        D3D12_COMPARISON_FUNC func,
        D3D12_STENCIL_OP      failOp,
        D3D12_STENCIL_OP      depthFailOp,
        D3D12_STENCIL_OP      passOp);

    void setBackStencil(
        D3D12_COMPARISON_FUNC func,
        D3D12_STENCIL_OP      failOp,
        D3D12_STENCIL_OP      depthFailOp,
        D3D12_STENCIL_OP      passOp);

    ComPtr<ID3D12PipelineState> build(ID3D12Device *device);
};

AGZ_D3D12_END
