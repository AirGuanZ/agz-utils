R"___(
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
}
)___"