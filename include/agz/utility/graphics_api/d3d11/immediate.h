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

    explicit Immediate2D(const Int2 &framebufferSize = { 1, 1 });

    explicit Immediate2D(Window &window);

    void setFramebufferSize(const Int2 &size);

    /**
     * screen coord: [0, 1]^2
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     */
    void drawLine(
        const Float2 &a, const Color4 &aColor,
        const Float2 &b, const Color4 &bColor) const;

    void drawLine(
        const Float2 &a, const Float2 &b, const Color4 &color) const;

    /**
     * pixel coord: [0, width] * [0, height]
     * +x:     right
     * +y:     up
     * origin: left bottom corner
     */
    void drawLineP(
        const Int2 &a, const Color4 &aColor,
        const Int2 &b, const Color4 &bColor) const;

    void drawLineP(
        const Int2 &a, const Int2 &b, const Color4 &color) const;

private:

    Int2   framebufferSize_;
    Float2 invFramebufferSize_;

    struct LineVertex
    {
        Float2 pos;
        Color4 color;
    };

    Shader<VS, PS>            lineShader_;
    PipelineState             lineState_;
    ComPtr<ID3D11InputLayout> lineInputLayout_;
};

inline Immediate2D::Immediate2D(const Int2 &framebufferSize)
{
    setFramebufferSize(framebufferSize);

    const char *LINE_VS = R"___(
struct VSInput
{
    float2 position : POSITION;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.position = float4(input.position, 0.5, 1);
    output.color    = input.color;
    return output;
})___";

    const char *LINE_PS = R"___(
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    return input.color;
})___";

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
    gDeviceContext->IASetInputLayout(lineInputLayout_.Get());
    gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    gDeviceContext->Draw(2, 0);

    gDeviceContext->IASetInputLayout(nullptr);
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

AGZ_D3D11_END
