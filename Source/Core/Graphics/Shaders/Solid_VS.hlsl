cbuffer Transform : register(b0)
{
    matrix modelView;
    matrix modelViewProj;
};

float4 main(float3 aPos : POSITION) : SV_POSITION
{
    return mul(float4(aPos, 1.0f), modelViewProj);
}