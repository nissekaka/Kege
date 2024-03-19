#include "RSM.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;

    const float4 indirectLightDirectional = giDirectionalTex.Sample(defaultSampler, uv).rgba;
    const float4 indirectLightSpot = giSpotTex.Sample(defaultSampler, uv).rgba;

    float4 indirectLight = indirectLightDirectional + indirectLightSpot;

    return indirectLight;
}
