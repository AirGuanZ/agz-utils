R"___(

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float  alpha    : ALPHA;
};

Texture2D Tex;

SamplerState Sam;

float4 main(PSInput input) : SV_TARGET
{
    float4 t = Tex.SampleLevel(Sam, input.uv, 0);
    return float4(t.rgb, t.a * input.alpha);
}

)___"