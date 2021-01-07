R"___(

struct VSInput
{
    float2 position : POSITION;
    float2 uv       : TEXCOORD;
    float  alpha    : ALPHA;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float  alpha    : ALPHA;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 0.5, 1);
    output.uv       = input.uv;
    output.alpha    = input.alpha;
    return output;
}

)___"