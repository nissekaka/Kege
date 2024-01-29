#include "common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D albedoTex : register(t2);

float4 main(PixelInput aInput) : SV_TARGET
{
    float3 colour = albedoTex.Sample(splr, aInput.texCoord).rgb;

	return float4(colour, 1.0f);
}