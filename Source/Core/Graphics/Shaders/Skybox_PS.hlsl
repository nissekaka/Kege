#include "common.hlsli"

cbuffer DirectionalLightBuffer : register(b1)
{
    float3 dLightDirection;
    float padding;
    float3 dLightColour;
    float ambientLight;
};

struct PixelInput
{
    float3 worldPosition : POSITION;
    float4 position : SV_POSITION;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    const float3 day = dayTex.Sample(splr, aInput.worldPosition).rgb;
    const float3 night = nightTex.Sample(splr, aInput.worldPosition).rgb;
    const float3 lightDir = normalize(dLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f;
    const float4 colour = float4(lerp(day, night, blendFactor), 1.0f);
    
    return colour;
}