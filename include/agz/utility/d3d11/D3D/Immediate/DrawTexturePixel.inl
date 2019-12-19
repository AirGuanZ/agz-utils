R"___(
cbuffer AlphaTest
{
    float AlphaTestThreshold;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D<float4> ImmediateTexture;
SamplerState ImmediateSampler;

float4 main(PSInput input) : SV_TARGET
{
    float4 texel = ImmediateTexture.Sample(ImmediateSampler, input.texCoord);
    clip(texel.a - AlphaTestThreshold);
    return texel;
}
)___"