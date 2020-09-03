#pragma once

#include "buffer.h"
#include "inputLayout.h"
#include "pipelineState.h"
#include "shader.h"
#include "window.h"

AGZ_D3D11_BEGIN

class Immediate2D
{
    WindowPostResizeHandler winResizeHandler;

public:

    /**
     * screen coord: [0, 1]^2
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     *
     * pixel coord: [0, width] * [0, height]
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     */

    explicit Immediate2D(const Int2 &framebufferSize = { 1, 1 });

    explicit Immediate2D(Window &window);

    void setFramebufferSize(const Int2 &size);

    void drawLine(
        const Float2 &a, const Color4 &aColor,
        const Float2 &b, const Color4 &bColor) const;

    void drawLine(
        const Float2 &a, const Float2 &b, const Color4 &color) const;

    void drawLineP(
        const Int2 &a, const Color4 &aColor,
        const Int2 &b, const Color4 &bColor) const;

    void drawLineP(
        const Int2 &a, const Int2 &b, const Color4 &color) const;

    void drawRect(
        const Float2 &a, const Float2 &b, const Color4 &color) const;

    void drawRectP(
        const Int2 &a, const Int2 &b, const Color4 &color) const;

    void drawTexRect(
        const Float2             &a,
        const Float2             &b,
        ID3D11ShaderResourceView *srv,
        const Float2             &tA     = { 0, 1 },
        const Float2             &tB     = { 1, 0 },
        bool                      linear = true,
        float                     alpha  = 1) const;
    
    void drawTexRectP(
        const Int2               &a,
        const Int2               &b,
        ID3D11ShaderResourceView *srv,
        const Float2             &tA     = { 0, 1 },
        const Float2             &tB     = { 1, 0 },
        bool                      linear = true,
        float                     alpha  = 1) const;

private:

    Int2   framebufferSize_;
    Float2 invFramebufferSize_;

    // line

    struct LineVertex
    {
        Float2 pos;
        Color4 color;
    };

    Shader<VS, PS>            lineShader_;
    PipelineState             lineState_;
    ComPtr<ID3D11InputLayout> lineInputLayout_;

    // tex

    struct TexVertex
    {
        Float2 pos;
        Float2 uv;
        float  alpha = 1;
    };

    Shader<VS, PS>              texShader_;
    Shader<VS, PS>::RscMgr      texRsc_;
    ShaderResourceViewSlot<PS> *texSlot_;
    SamplerSlot<PS>            *samplerSlot_;
    ComPtr<ID3D11InputLayout>   texInputLayout_;

    ComPtr<ID3D11SamplerState> pointSampler_;
    ComPtr<ID3D11SamplerState> linearSampler_;
};

inline Immediate2D::Immediate2D(const Int2 &framebufferSize)
{
    setFramebufferSize(framebufferSize);

    // line

    const char *LINE_VS =
#include "immediate/lineVS.hlsl"
        ;

    const char *LINE_PS =
#include "immediate/linePS.hlsl"
        ;

    lineShader_.initializeStage<VS>(LINE_VS)
               .initializeStage<PS>(LINE_PS);

    lineState_.setBlendState(
        true,
        D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
        D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, nullptr);
    lineState_.setDepthStencilState(false);

    lineInputLayout_ = InputLayoutBuilder({
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
            0, offsetof(LineVertex, pos),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
            0, offsetof(LineVertex, color),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        }
    }).build(lineShader_);

    // tex

    const char *TEX_VS =
#include "immediate/texVS.hlsl"
        ;

    const char *TEX_PS =
#include "immediate/texPS.hlsl"
        ;

    texShader_.initializeStage<VS>(TEX_VS);
    texShader_.initializeStage<PS>(TEX_PS);

    texInputLayout_ = InputLayoutBuilder({
        {
            "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
            0, offsetof(TexVertex, pos),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
            0, offsetof(TexVertex, uv),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "ALPHA", 0, DXGI_FORMAT_R32_FLOAT,
            0, offsetof(TexVertex, alpha),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        }
    }).build(texShader_);

    texRsc_ = texShader_.createResourceManager();

    texSlot_     = texRsc_.getShaderResourceViewSlot<PS>("Tex");
    samplerSlot_ = texRsc_.getSamplerSlot<PS>("Sam");

    
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MinLOD         = -FLT_MAX;
    samplerDesc.MaxLOD         = +FLT_MAX;
    samplerDesc.MipLODBias     = 0;
    samplerDesc.MaxAnisotropy  = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    pointSampler_ = device.createSampler(samplerDesc);

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    linearSampler_ = device.createSampler(samplerDesc);
}

inline Immediate2D::Immediate2D(Window &window)
    : Immediate2D()
{
    winResizeHandler.set_function([this](const WindowPostResizeEvent &e)
    {
        if(e.width > 0 && e.height > 0)
            setFramebufferSize({ e.width, e.height });
    });
    window.attach(&winResizeHandler);

    setFramebufferSize(window.getClientSize());
}

inline void Immediate2D::setFramebufferSize(const Int2 &size)
{
    framebufferSize_ = size;
    invFramebufferSize_ = { 1.0f / size.x, 1.0f / size.y };
}

inline void Immediate2D::drawLine(
    const Float2 &a, const Color4 &aColor,
    const Float2 &b, const Color4 &bColor) const
{
    const LineVertex vertexData[2] = {
        { a + a - Float2(1), aColor }, { b + b - Float2(1), bColor }
    };

    VertexBuffer<LineVertex> vertexBuffer;
    vertexBuffer.initialize(2, vertexData);

    lineShader_.bind();
    lineState_.bind();
    vertexBuffer.bind(0);
    deviceContext.d3dDeviceContext->IASetInputLayout(lineInputLayout_.Get());
    deviceContext.d3dDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    deviceContext.d3dDeviceContext->Draw(2, 0);

    deviceContext.d3dDeviceContext->IASetInputLayout(nullptr);
    vertexBuffer.unbind(0);
    lineState_.unbind();
    lineShader_.unbind();
}

inline void Immediate2D::drawLine(
    const Float2 &a, const Float2 &b, const Color4 &color) const
{
    drawLine(a, color, b, color);
}

inline void Immediate2D::drawLineP(
    const Int2 &a, const Color4 &aColor,
    const Int2 &b, const Color4 &bColor) const
{
    const Float2 af = Float2(a.x + 0.5f, a.y + 0.5f) * invFramebufferSize_;
    const Float2 bf = Float2(b.x + 0.5f, b.y + 0.5f) * invFramebufferSize_;
    drawLine(af, aColor, bf, bColor);
}

inline void Immediate2D::drawLineP(
    const Int2 &a, const Int2 &b, const Color4 &color) const
{
    return drawLineP(a, color, b, color);
}

inline void Immediate2D::drawRect(
    const Float2 &_a, const Float2 &_b, const Color4 &color) const
{
    Float2 a = Float2((std::min)(_a.x, _b.x), (std::min)(_a.y, _b.y));
    Float2 b = Float2((std::max)(_a.x, _b.x), (std::max)(_a.y, _b.y));

    a = a + a - 1.0f;
    b = b + b - 1.0f;

    const LineVertex vertexData[] = {
        { a, color }, { { a.x, b.y }, color }, { b, color },
        { a, color }, { b, color }, { { b.x, a.y }, color }
    };

    VertexBuffer<LineVertex> vertexBuffer;
    vertexBuffer.initialize(6, vertexData);

    lineShader_.bind();
    lineState_.bind();
    vertexBuffer.bind(0);
    deviceContext.d3dDeviceContext->IASetInputLayout(lineInputLayout_.Get());
    deviceContext.d3dDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext.d3dDeviceContext->Draw(6, 0);

    deviceContext.d3dDeviceContext->IASetInputLayout(nullptr);
    vertexBuffer.unbind(0);
    lineState_.unbind();
    lineShader_.unbind();
}

inline void Immediate2D::drawRectP(
    const Int2 &a, const Int2 &b, const Color4 &color) const
{
    const Float2 af = Float2(a.x + 0.5f, a.y + 0.5f) * invFramebufferSize_;
    const Float2 bf = Float2(b.x + 0.5f, b.y + 0.5f) * invFramebufferSize_;
    drawRect(af, bf, color);
}

inline void Immediate2D::drawTexRect(
    const Float2             &_a,
    const Float2             &_b,
    ID3D11ShaderResourceView *srv,
    const Float2             &_tA,
    const Float2             &_tB,
    bool                      linear,
    float                     alpha) const
{
    Float2 a, b, tA, tB;

    if(_a.x < _b.x)
    {
        a.x  = _a.x;
        tA.x = _tA.x;
        b.x  = _b.x;
        tB.x = _tB.x;
    }
    else
    {
        a.x  = _b.x;
        tA.x = _tB.x;
        b.x  = _a.x;
        tB.x = _tA.x;
    }

    if(_a.y < _b.y)
    {
        a.y  = _a.y;
        tA.y = _tA.y;
        b.y  = _b.y;
        tB.y = _tB.y;
    }
    else
    {
        a.y  = _b.y;
        tA.y = _tB.y;
        b.y  = _a.y;
        tB.y = _tA.y;
    }

    a = a + a - 1.0f;
    b = b + b - 1.0f;

    const TexVertex vertexData[] = {
        { a,            tA,             alpha },
        { { a.x, b.y }, { tA.x, tB.y }, alpha },
        { b,            tB,             alpha },
        { a,            tA,             alpha },
        { b,            tB,             alpha },
        { { b.x, a.y }, { tB.x, tA.y }, alpha }
    };

    VertexBuffer<TexVertex> vertexBuffer;
    vertexBuffer.initialize(6, vertexData);

    samplerSlot_->setSampler(linear ? linearSampler_ : pointSampler_);
    texSlot_->setShaderResourceView(srv);

    texShader_.bind();
    lineState_.bind();
    texRsc_.bind();

    vertexBuffer.bind(0);
    deviceContext.d3dDeviceContext->IASetInputLayout(texInputLayout_.Get());
    deviceContext.d3dDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext.d3dDeviceContext->Draw(6, 0);

    deviceContext.d3dDeviceContext->IASetInputLayout(nullptr);
    vertexBuffer.unbind(0);

    texRsc_.unbind();
    lineState_.unbind();
    texShader_.unbind();

    samplerSlot_->setSampler(nullptr);
    texSlot_->setShaderResourceView(nullptr);
}

inline void Immediate2D::drawTexRectP(
    const Int2               &a,
    const Int2               &b,
    ID3D11ShaderResourceView *srv,
    const Float2             &tA,
    const Float2             &tB,
    bool                      linear,
    float                     alpha) const
{
    const Float2 af = Float2(a.x + 0.5f, a.y + 0.5f) * invFramebufferSize_;
    const Float2 bf = Float2(b.x + 0.5f, b.y + 0.5f) * invFramebufferSize_;
    drawTexRect(af, bf, srv, tA, tB, linear, alpha);
}

AGZ_D3D11_END
