#include "common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

Texture2D colourTex : register(t2);

float4 main(PixelInput aInput) : SV_TARGET
{
    float3 colour = colourTex.Sample(splr, aInput.texCoord).rgb;

	return float4(colour, 1.0f);
}