#include "RSM.hlsli"

Texture2D giDirectionalTex : register(t9);
Texture2D giSpotTex : register(t10);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;

    const float4 indirectLightDirectional = giDirectionalTex.Sample(linearSampler, uv).rgba;
    const float4 indirectLightSpot = giSpotTex.Sample(linearSampler, uv).rgba;

    float4 indirectLight = indirectLightDirectional + indirectLightSpot;

    return indirectLight;
}
