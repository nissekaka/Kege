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
Texture2D worldPosTex : register(t12);


float4 main(PixelInput aInput) : SV_TARGET
{
    float4 colour = colourTex.Sample(splr, aInput.texCoord).rgba;

    if (colour.a < 0.1f)
    {
        discard;
    }

    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPosTex.Sample(splr, uv).rgb;

    float dist = length(worldPosition - aInput.worldPos);

    // Smoothstep the alpha
    float alpha = smoothstep(0.0, 15.0f, dist);

    colour.a = alpha;

	return float4(colour);
}