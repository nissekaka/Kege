
#include "common.hlsli"
#include "RSM.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;

    const float3 indirectLightDirectional = giDirectionalTex.Sample(defaultSampler, uv);;
    const float3 indirectLightSpot = giSpotTex.Sample(defaultSampler, uv);

    float3 indirectLight = indirectLightDirectional + indirectLightSpot;

    return float4(indirectLight, 1.0f);
}
