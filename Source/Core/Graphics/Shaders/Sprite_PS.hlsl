#include "deferred_common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

//Texture2D colourTex : register(t11);

float4 main(PixelInput aInput) : SV_TARGET
{
    float4 colour = gColourTex.Sample(defaultSampler, aInput.texCoord).rgba;

    if (colour.a < 0.1f)
    {
        discard;
    }

    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;

    const float distToWorld = length(worldPosition - aInput.worldPos);
    const float distToCamera = length(cameraPosition.xyz - aInput.worldPos);

    // Smoothstep the alpha
    const float worldAlpha = smoothstep(0.0, 15.0f, distToWorld) * 0.005f;
    const float cameraAlpha = smoothstep(0.0, 15.0f, distToCamera) * 0.005f;

    colour.a = min(worldAlpha, cameraAlpha);

	return float4(colour);
}