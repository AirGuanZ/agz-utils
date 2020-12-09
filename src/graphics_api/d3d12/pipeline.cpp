#ifdef AGZ_ENABLE_D3D12

#include <d3dx12.h>

#include <agz/utility/graphics_api/d3d12/pipeline.h>

AGZ_D3D12_BEGIN

namespace
{
    D3D12_DEPTH_STENCIL_DESC defaultDepthStencilDesc() noexcept
    {
        return D3D12_DEPTH_STENCIL_DESC{
            .DepthEnable      = false,
            .DepthWriteMask   = D3D12_DEPTH_WRITE_MASK_ZERO,
            .DepthFunc        = D3D12_COMPARISON_FUNC_LESS,
            .StencilEnable    = false,
            .StencilReadMask  = 0xff,
            .StencilWriteMask = 0xff,
            .FrontFace = {
                .StencilFailOp      = D3D12_STENCIL_OP_KEEP,
                .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                .StencilPassOp      = D3D12_STENCIL_OP_KEEP,
                .StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS
            },
            .BackFace = {
                .StencilFailOp      = D3D12_STENCIL_OP_KEEP,
                .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                .StencilPassOp      = D3D12_STENCIL_OP_KEEP,
                .StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS
            }
        };
    }

} // namespace anonymous

PipelineBuilder::PipelineBuilder(ID3D12Device *device)
    : device_(device), desc_{}
{
    desc_.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    //desc_.BlendState.IndependentBlendEnable = true;
    desc_.SampleMask                        = 0xffffffff;
    desc_.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc_.DepthStencilState                 = defaultDepthStencilDesc();
    desc_.PrimitiveTopologyType             = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc_.NumRenderTargets                  = 0;
    desc_.DSVFormat                         = DXGI_FORMAT_UNKNOWN;
    desc_.SampleDesc                        = { 1, 0 };
    desc_.Flags                             = D3D12_PIPELINE_STATE_FLAG_NONE;

    //for(auto &b : desc_.BlendState.RenderTarget)
    //    b.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void PipelineBuilder::addInputElement(const D3D12_INPUT_ELEMENT_DESC &elem)
{
    inputElems_.push_back(elem);
}

void PipelineBuilder::addInputElements(
    const D3D12_INPUT_ELEMENT_DESC *elems,
    size_t                          cnt)
{
    std::copy(elems, elems + cnt, std::back_inserter(inputElems_));
}

void PipelineBuilder::setRootSignature(ComPtr<ID3D12RootSignature> rootSignature)
{
    rootSignature_ = std::move(rootSignature);
}

void PipelineBuilder::setVertexShader(ComPtr<ID3DBlob> byteCode)
{
    sharedVertexShaderByteCode_ = std::move(byteCode);
    desc_.VS.pShaderBytecode    = sharedVertexShaderByteCode_->GetBufferPointer();
    desc_.VS.BytecodeLength     = sharedVertexShaderByteCode_->GetBufferSize();
}

void PipelineBuilder::setPixelShader(ComPtr<ID3DBlob> byteCode)
{
    sharedPixelShaderByteCode_ = std::move(byteCode);
    desc_.PS.pShaderBytecode    = sharedPixelShaderByteCode_->GetBufferPointer();
    desc_.PS.BytecodeLength     = sharedPixelShaderByteCode_->GetBufferSize();
}

void PipelineBuilder::setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology)
{
    desc_.PrimitiveTopologyType = topology;
}

void PipelineBuilder::setRenderTargetCount(int count)
{
    desc_.NumRenderTargets = static_cast<UINT>(count);
}

void PipelineBuilder::setRenderTargetFormat(int renderTarget, DXGI_FORMAT format)
{
    assert(static_cast<UINT>(renderTarget) < desc_.NumRenderTargets);
    desc_.RTVFormats[renderTarget] = format;
}

void PipelineBuilder::setDepthStencilFormat(DXGI_FORMAT format)
{
    desc_.DSVFormat = format;
}

void PipelineBuilder::setMultisample(int sampleCount, int sampleQuaility)
{
    desc_.SampleDesc = DXGI_SAMPLE_DESC{
        .Count   = static_cast<UINT>(sampleCount),
        .Quality = static_cast<UINT>(sampleQuaility),
    };
}

void PipelineBuilder::enableBlend(
    int            renderTarget,
    D3D12_BLEND    src,
    D3D12_BLEND    dst,
    D3D12_BLEND_OP op,
    D3D12_BLEND    srcAlpha,
    D3D12_BLEND    dstAlpha,
    D3D12_BLEND_OP opAlpha)
{
    assert(static_cast<UINT>(renderTarget) < desc_.NumRenderTargets);

    desc_.BlendState.RenderTarget[renderTarget] =
        D3D12_RENDER_TARGET_BLEND_DESC{
            .BlendEnable           = true,
            .LogicOpEnable         = false,
            .SrcBlend              = src,
            .DestBlend             = dst,
            .BlendOp               = op,
            .SrcBlendAlpha         = srcAlpha,
            .DestBlendAlpha        = dstAlpha,
            .BlendOpAlpha          = opAlpha,
            .LogicOp               = D3D12_LOGIC_OP_NOOP,
            .RenderTargetWriteMask =
                desc_.BlendState.RenderTarget[renderTarget].RenderTargetWriteMask
        };
}

void PipelineBuilder::setRenderTargetWriteMask(int renderTarget, UINT8 mask)
{
    assert(static_cast<UINT>(renderTarget) < desc_.NumRenderTargets);
    desc_.BlendState.RenderTarget[renderTarget].RenderTargetWriteMask = mask;
}

void PipelineBuilder::setFillMode(D3D12_FILL_MODE fillMode)
{
    desc_.RasterizerState.FillMode = fillMode;
}

void PipelineBuilder::setCullMode(
    D3D12_CULL_MODE cullMode,
    bool            frontCounterClockwise)
{
    desc_.RasterizerState.CullMode              = cullMode;
    desc_.RasterizerState.FrontCounterClockwise = frontCounterClockwise;
}

void PipelineBuilder::setDepthBias(
    int   bias,
    float biasClamp,
    float slopeScaledBias)
{
    desc_.RasterizerState.DepthBias            = bias;
    desc_.RasterizerState.DepthBiasClamp       = biasClamp;
    desc_.RasterizerState.SlopeScaledDepthBias = slopeScaledBias;
}

void PipelineBuilder::setDepthTest(bool test, bool write)
{
    desc_.DepthStencilState.DepthEnable    = test;
    desc_.DepthStencilState.DepthWriteMask =
        write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
}

void PipelineBuilder::setDepthFunc(D3D12_COMPARISON_FUNC func)
{
    desc_.DepthStencilState.DepthFunc = func;
}

void PipelineBuilder::setStencilTest(bool enabled, UINT8 readMask, UINT8 writeMask)
{
    desc_.DepthStencilState.StencilEnable    = enabled;
    desc_.DepthStencilState.StencilReadMask  = readMask;
    desc_.DepthStencilState.StencilWriteMask = writeMask;
}

void PipelineBuilder::setFrontStencil(
    D3D12_COMPARISON_FUNC func,
    D3D12_STENCIL_OP      failOp,
    D3D12_STENCIL_OP      depthFailOp,
    D3D12_STENCIL_OP      passOp)
{
    desc_.DepthStencilState.FrontFace = D3D12_DEPTH_STENCILOP_DESC{
        .StencilFailOp      = failOp,
        .StencilDepthFailOp = depthFailOp,
        .StencilPassOp      = passOp,
        .StencilFunc        = func
    };
}

void PipelineBuilder::setBackStencil(
    D3D12_COMPARISON_FUNC func,
    D3D12_STENCIL_OP      failOp,
    D3D12_STENCIL_OP      depthFailOp,
    D3D12_STENCIL_OP      passOp)
{
    desc_.DepthStencilState.BackFace = D3D12_DEPTH_STENCILOP_DESC{
        .StencilFailOp      = failOp,
        .StencilDepthFailOp = depthFailOp,
        .StencilPassOp      = passOp,
        .StencilFunc        = func
    };
}

ComPtr<ID3D12PipelineState> PipelineBuilder::build()
{
    assert(device_);

    desc_.pRootSignature                 = rootSignature_.Get();
    desc_.InputLayout.NumElements        = static_cast<UINT>(inputElems_.size());
    desc_.InputLayout.pInputElementDescs = inputElems_.data();

    ComPtr<ID3D12PipelineState> ret;
    AGZ_D3D12_CHECK_HR_MSG(
        "failed to create d3d12 graphics pipeline state",
        device_->CreateGraphicsPipelineState(
            &desc_, IID_PPV_ARGS(ret.GetAddressOf())));

    return ret;
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
