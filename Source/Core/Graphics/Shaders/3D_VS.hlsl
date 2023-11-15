struct VSOut
{
    float4 colour : COLOUR;
    float4 pos : SV_POSITION;
};

cbuffer CBuf
{
    matrix transform;
}

VSOut main(float3 pos : POSITION, float4 colour : COLOUR)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.colour = colour;
    return vso;
}