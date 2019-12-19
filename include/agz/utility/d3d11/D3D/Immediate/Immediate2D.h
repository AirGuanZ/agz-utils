#pragma once

#include <agz/utility/d3d11/D3D/Buffer/ConstantBuffer.h>
#include <agz/utility/d3d11/D3D/Buffer/VertexBuffer.h>
#include <agz/utility/d3d11/D3D/State/DepthStencilState.h>
#include <agz/utility/d3d11/D3D/Texture/Sampler.h>
#include <agz/utility/d3d11/D3D/InputLayout.h>
#include <agz/utility/d3d11/D3D/RenderState.h>
#include <agz/utility/d3d11/D3D/Shader.h>

AGZ_D3D11_BEGIN

class Immediate2D : public agz::misc::uncopyable_t
{

    Vec2i framebufferSize_;

    struct DrawTexture
    {
        struct PS_AlphaTest
        {
            float alphaTestThreshold = 0;
            float pad[3] = { 0, 0, 0 };
        };

        DrawTexture();

        Shader<SS_VS, SS_PS> shader;
        UniformManager<SS_VS, SS_PS> uniforms;

        mutable SamplerSlot<SS_PS> *samplerSlot;
        mutable ShaderResourceSlot<SS_PS> *textureSlot;

        mutable ConstantBuffer<PS_AlphaTest> psAlphaTest;

        struct Vertex
        {
            Vec2 position;
            Vec2 texCoord;
        };

        InputLayout inputLayout;
        mutable VertexBuffer<Vertex> vertexBuffer;

    } drawTexture_;

    struct DrawRectangle
    {
        DrawRectangle();

        Shader<SS_VS, SS_PS> shader;

        struct Vertex
        {
            Vec2 position;
            Vec4 color;
        };

        InputLayout inputLayout;
        mutable VertexBuffer<Vertex> vertexBuffer;
    } drawRectangle_;

    Sampler pointSampler_;
    Sampler linearSampler_;

    DepthState disableDepth_;

public:

    enum SamplerType
    {
        Point,
        Linear
    };

    explicit Immediate2D(const Vec2i &framebufferSize = Vec2i(640, 480));

    void SetFramebufferSize(const Vec2i &newFramebufferSize) noexcept;

    const Vec2i &GetFramebufferSize() const noexcept;

    /**
     * @brief 在屏幕上绘制矩形纹理
     *
     * framebuffer左下角为原点，+x向右+y向上，单位为像素
     */
    void DrawTextureP(
        const Vec2i &leftBottom, const Vec2i &rightTop, ID3D11ShaderResourceView *srv,
        float alphaTestThreshold = 0, SamplerType samplerType = Point) const;

    /**
     * @brief 在屏幕上绘制矩形纹理
     *
     * framebuffer中心为原点，+x向右+y向上，坐标取值范围为[-1, 1]^2
     */
    void DrawTexture(
        const Vec2 &leftBottom, const Vec2 &rightTop, ID3D11ShaderResourceView *srv,
        float alphaTestThreshold = 0, SamplerType samplerType = Point) const;

    /**
     * @brief 在屏幕上绘制纯色矩形
     *
     * framebuffer左下角为原点，+x向右+y向上，单位为像素
     */
    void DrawRectangleP(const Vec2i &leftBottom, const Vec2i &rightTop, const Vec4 &color) const;

    /**
     * @brief 在屏幕上绘制纯色矩形
     *
     * framebuffer中心为原点，+x向右+y向上，坐标取值范围为[-1, 1]^2
     */
    void DrawRectangle(const Vec2 &leftBottom, const Vec2 &rightTop, const Vec4 &color) const;

    /**
     * @brief 在屏幕上绘制纯色线段
     *
     * framebuffer左下角为原点，+x向右+y向上，单位为像素
     */
    void DrawLineP(const Vec2i &A, const Vec2i &B, const Vec4 &color) const;

    /**
     * @brief 在屏幕上绘制纯色线段
     *
     * framebuffer中心为原点，+x向右+y向上，坐标取值范围为[-1, 1]^2
     */
    void DrawLine(const Vec2 &A, const Vec2 &B, const Vec4 &color) const;
};

inline Immediate2D::DrawTexture::DrawTexture()
{
    const char *vertexShaderSource = 
#include "./DrawTextureVertex.inl"
        ;
    const char *pixelShaderSource = 
#include "./DrawTexturePixel.inl"
        ;
    shader.InitializeStage<SS_VS>(vertexShaderSource);
    shader.InitializeStage<SS_PS>(pixelShaderSource);
    if(!shader.IsAllStagesAvailable())
    {
        throw VRPGBaseException("failed to initialize stages for Immediate2D.drawTexture.shader");
    }

    uniforms = shader.CreateUniformManager();

    samplerSlot = uniforms.GetSamplerSlot<SS_PS>("ImmediateSampler");
    textureSlot = uniforms.GetShaderResourceSlot<SS_PS>("ImmediateTexture");

    psAlphaTest.Initialize(true, nullptr);
    uniforms.GetConstantBufferSlot<SS_PS>("AlphaTest")->SetBuffer(psAlphaTest);

    inputLayout = InputLayoutBuilder
        ("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, offsetof(Vertex, position))
        ("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, offsetof(Vertex, texCoord))
        .Build(shader.GetVertexShaderByteCode());

    vertexBuffer.Initialize(6, true, nullptr);
}

inline Immediate2D::DrawRectangle::DrawRectangle()
{
    const char *vertexShaderSource =
#include "./DrawRectangleVertex.inl"
        ;
    const char *pixelShaderSource =
#include "./DrawRectanglePixel.inl"
        ;
    shader.InitializeStage<SS_VS>(vertexShaderSource);
    shader.InitializeStage<SS_PS>(pixelShaderSource);
    if(!shader.IsAllStagesAvailable())
    {
        throw VRPGBaseException("failed to initialize stages for Immediate2D.drawRectangle.shader");
    }

    inputLayout = InputLayoutBuilder
        ("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, offsetof(Vertex, position))
        ("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, offsetof(Vertex, color))
        .Build(shader.GetVertexShaderByteCode());

    vertexBuffer.Initialize(6, true, nullptr);
}

inline Immediate2D::Immediate2D(const Vec2i &framebufferSize)
{
    framebufferSize_ = framebufferSize;

    pointSampler_.Initialize(D3D11_FILTER_MIN_MAG_MIP_POINT);
    linearSampler_.Initialize(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

    disableDepth_.Initialize(false, D3D11_DEPTH_WRITE_MASK_ZERO);
}

inline void Immediate2D::SetFramebufferSize(const Vec2i &newFramebufferSize) noexcept
{
    framebufferSize_ = newFramebufferSize;
}

inline const Vec2i &Immediate2D::GetFramebufferSize() const noexcept
{
    return framebufferSize_;
}

inline void Immediate2D::DrawTextureP(
    const Vec2i &leftBottom, const Vec2i &rightTop, ID3D11ShaderResourceView *srv,
    float alphaTestThreshold, SamplerType samplerType) const
{
    Vec2 LB = {
        2.0f * leftBottom.x / framebufferSize_.x - 1,
        2.0f * leftBottom.y / framebufferSize_.y - 1
    };
    Vec2 RT = {
        2.0f * (rightTop.x + 1) / framebufferSize_.x - 1,
        2.0f * (rightTop.y + 1) / framebufferSize_.y - 1
    };
    DrawTexture(LB, RT, srv, alphaTestThreshold, samplerType);
}

inline void Immediate2D::DrawTexture(
    const Vec2 &leftBottom, const Vec2 &rightTop, ID3D11ShaderResourceView *srv,
    float alphaTestThreshold, SamplerType samplerType) const
{
    DrawTexture::Vertex vertexData[] = {
        { leftBottom,                   { 0, 1 } },
        { { leftBottom.x, rightTop.y }, { 0, 0 } },
        { rightTop,                     { 1, 0 } },
        { leftBottom,                   { 0, 1 } },
        { rightTop,                     { 1, 0 } },
        { { rightTop.x, leftBottom.y }, { 1, 1 } }
    };

    drawTexture_.vertexBuffer.SetValue(vertexData);
    switch(samplerType)
    {
    case Point:  drawTexture_.samplerSlot->SetSampler(pointSampler_); break;
    case Linear: drawTexture_.samplerSlot->SetSampler(linearSampler_); break;
    }
    drawTexture_.textureSlot->SetShaderResourceView(srv);
    drawTexture_.psAlphaTest.SetValue({ alphaTestThreshold });

    drawTexture_.shader.Bind();
    drawTexture_.uniforms.Bind();
    drawTexture_.inputLayout.Bind();
    drawTexture_.vertexBuffer.Bind(0);
    disableDepth_.Bind();

    RenderState::Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6);

    disableDepth_.Unbind();
    drawTexture_.vertexBuffer.Unbind(0);
    drawTexture_.inputLayout.Unbind();
    drawTexture_.uniforms.Unbind();
    drawTexture_.shader.Unbind();
}

inline void Immediate2D::DrawRectangleP(const Vec2i &leftBottom, const Vec2i &rightTop, const Vec4 &color) const
{
    Vec2 LB = {
        2.0f * leftBottom.x / framebufferSize_.x - 1,
        2.0f * leftBottom.y / framebufferSize_.y - 1
    };
    Vec2 RT = {
        2.0f * (rightTop.x + 1) / framebufferSize_.x - 1,
        2.0f * (rightTop.y + 1) / framebufferSize_.y - 1
    };
    DrawRectangle(LB, RT, color);
}

inline void Immediate2D::DrawRectangle(const Vec2 &leftBottom, const Vec2 &rightTop, const Vec4 &color) const
{
    DrawRectangle::Vertex vertexData[] = {
        { leftBottom,                   color },
        { { leftBottom.x, rightTop.y }, color},
        { rightTop,                     color },
        { leftBottom,                   color },
        { rightTop,                     color },
        { { rightTop.x, leftBottom.y }, color }
    };
    drawRectangle_.vertexBuffer.SetValue(vertexData);

    drawRectangle_.shader.Bind();
    drawRectangle_.inputLayout.Bind();
    drawRectangle_.vertexBuffer.Bind(0);
    disableDepth_.Bind();

    RenderState::Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6);

    disableDepth_.Unbind();
    drawRectangle_.vertexBuffer.Unbind(0);
    drawRectangle_.inputLayout.Unbind();
    drawRectangle_.shader.Unbind();
}

inline void Immediate2D::DrawLineP(const Vec2i &A, const Vec2i &B, const Vec4 &color) const
{
    Vec2 scrA = {
        2 * (A.x + 0.5f) / framebufferSize_.x - 1,
        2 * (A.y + 0.5f) / framebufferSize_.y - 1
    };
    Vec2 scrB = {
        2 * (B.x + 0.5f) / framebufferSize_.x - 1,
        2 * (B.y + 0.5f) / framebufferSize_.y - 1
    };
    DrawLine(scrA, scrB, color);
}

inline void Immediate2D::DrawLine(const Vec2 &A, const Vec2 &B, const Vec4 &color) const
{
    DrawRectangle::Vertex vertexData[] =
    {
        { A, color },
        { B, color },
        { }, { }, { }, { }
    };
    drawRectangle_.vertexBuffer.SetValue(vertexData);

    drawRectangle_.shader.Bind();
    drawRectangle_.inputLayout.Bind();
    drawRectangle_.vertexBuffer.Bind(0);
    disableDepth_.Bind();

    RenderState::Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST , 2);

    disableDepth_.Unbind();
    drawRectangle_.vertexBuffer.Unbind(0);
    drawRectangle_.inputLayout.Unbind();
    drawRectangle_.shader.Unbind();
}

AGZ_D3D11_END
