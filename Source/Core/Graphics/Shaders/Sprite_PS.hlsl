#include "common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D colourTex : register(t2);
//Texture2D depthTex1 : register(t12);
//Texture2D worldPosTex : register(t12);

float3 ReconstructWorldPosFromDepth(float2 uv, float depth, float4x4 invProj, float4x4 invView)
{
    float ndcX = uv.x * 2 - 1;
    float ndcY = 1 - uv.y * 2; // Remember to flip y!!!
    float4 viewPos = mul(invProj, float4(ndcX, ndcY, depth, 1.0f));
    viewPos = viewPos / viewPos.w;
    return mul(invView, viewPos).xyz;
}

float4 main(PixelInput aInput) : SV_TARGET
{
    float4 colour = colourTex.Sample(splr, aInput.texCoord).rgba;

    if (colour.a < 0.1f)
    {
        discard;
    }

    //const float2 uv = aInput.position.xy / clientResolution.xy;
    //const float3 worldPosition = worldPosTex.Sample(splr, uv).rgb;
    //float3 depth = depthTex1.Sample(splr, uv).rgb;
    //float3 worldPos = ReconstructWorldPosFromDepth(uv, depth.r, clipToWorldMatrix, viewInverse);

    //float dist = length(worldPosition - aInput.worldPos);
    //float dist = length(depth - aInput.worldPos);

    // Smoothstep the alpha
    //float alpha = smoothstep(0.0, 15.0f, dist);

    //colour.a = alpha;

	return float4(colour);
}