#pragma once

#include <agz/utility/d3d11/D3D/Buffer/ConstantBuffer.h>
#include <agz/utility/d3d11/D3D/Buffer/VertexBuffer.h>
#include <agz/utility/d3d11/D3D/State/RasterizerState.h>
#include <agz/utility/d3d11/D3D/InputLayout.h>
#include <agz/utility/d3d11/D3D/RenderState.h>
#include <agz/utility/d3d11/D3D/Shader.h>

AGZ_D3D11_BEGIN

class Immediate3D : public agz::misc::uncopyable_t
{
public:

    void DrawLine(const Vec3 &a, const Vec3 &b, const Vec4 &color, const Mat4 &WVP) const;

    void DrawLine(const Vec3 &a, const Vec3 &b, const Vec4 &color) const;

    void DrawTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec4 &color, const Mat4 &WVP) const;

    void DrawTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec4 &color) const;

private:

    struct PureColor
    {
        struct Vertex
        {
            Vec3 position;
            Vec4 color;
        };

        struct VSTransform
        {
            Mat4 WVP;
        };

        PureColor();

        Shader<SS_VS, SS_PS>         shader;
        UniformManager<SS_VS, SS_PS> uniforms;
        InputLayout                  inputLayout;

        mutable ConstantBuffer<VSTransform> vsTransform;

        void Start(const Mat4 &WVP) const;

        void End() const;

    } pureColor_;

    struct DrawLine
    {
        DrawLine();

        RasterizerState rasterizerState;

        mutable VertexBuffer<PureColor::Vertex> vertexBuffer;

    } drawLine_;

    struct DrawTriangle
    {
        DrawTriangle();

        RasterizerState rasterizerState;

        mutable VertexBuffer<PureColor::Vertex> vertexBuffer;

    } drawTriangle_;
};

inline void Immediate3D::DrawLine(const Vec3 &a, const Vec3 &b, const Vec4 &color) const
{
    DrawLine(a, b, color, Mat4::identity());
}

inline void Immediate3D::DrawLine(const Vec3 &a, const Vec3 &b, const Vec4 &color, const Mat4 &WVP) const
{
    PureColor::Vertex vertexData[] =
    {
        { a, color }, { b, color }
    };
    drawLine_.vertexBuffer.SetValue(vertexData);

    pureColor_.Start(WVP);
    drawLine_.rasterizerState.Bind();
    drawLine_.vertexBuffer   .Bind(0);

    RenderState::Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 2);

    pureColor_.End();
    drawLine_.rasterizerState.Unbind();
    drawLine_.vertexBuffer   .Unbind(0);
}

inline void Immediate3D::DrawTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec4 &color) const
{
    DrawTriangle(a, b, c, color, Mat4::identity());
}

inline void Immediate3D::DrawTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec4 &color, const Mat4 &WVP) const
{
    PureColor::Vertex vertexData[] =
    {
        { a, color }, { b, color }, { c, color }
    };
    drawTriangle_.vertexBuffer.SetValue(vertexData);

    pureColor_.Start(WVP);
    drawTriangle_.vertexBuffer.Bind(0);
    drawTriangle_.rasterizerState.Bind();

    RenderState::Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 3);
    
    pureColor_.End();
    drawTriangle_.vertexBuffer.Unbind(0);
    drawTriangle_.rasterizerState.Unbind();
}

inline Immediate3D::PureColor::PureColor()
{
    const char *VERTEX_SHADER_SOURCE = R"___(
cbuffer Transform
{
    float4x4 WVP;
};

struct VSInput
{
    float3 position : POSITION;
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
    output.position = mul(float4(input.position, 1), WVP);
    output.color = input.color;
    return output;
}
)___";

    const char *PIXEL_SHADER_SOURCE = R"___(
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    return input.color;
}
)___";

    shader.InitializeStage<SS_VS>(VERTEX_SHADER_SOURCE);
    shader.InitializeStage<SS_PS>(PIXEL_SHADER_SOURCE);
    if(!shader.IsAllStagesAvailable())
    {
        throw VRPGBaseException("failed to initialize Immediate3D::DrawLine::shader");
    }

    uniforms = shader.CreateUniformManager();

    inputLayout = InputLayoutBuilder
    ("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(Vertex, position))
        ("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, offsetof(Vertex, color))
        .Build(shader.GetVertexShaderByteCode());

    vsTransform.Initialize(true, nullptr);
    uniforms.GetConstantBufferSlot<SS_VS>("Transform")->SetBuffer(vsTransform);
}

inline void Immediate3D::PureColor::Start(const Mat4 &WVP) const
{
    vsTransform.SetValue({ WVP });
    shader     .Bind();
    uniforms   .Bind();
    inputLayout.Bind();
}

inline void Immediate3D::PureColor::End() const
{
    shader     .Unbind();
    uniforms   .Unbind();
    inputLayout.Unbind();
}

inline Immediate3D::DrawLine::DrawLine()
{
    rasterizerState.Initialize(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, false);
    vertexBuffer.Initialize(2, true, nullptr);
}

inline Immediate3D::DrawTriangle::DrawTriangle()
{
    rasterizerState.Initialize(D3D11_FILL_SOLID, D3D11_CULL_NONE, false);
    vertexBuffer.Initialize(3, true, nullptr);
}

AGZ_D3D11_END
