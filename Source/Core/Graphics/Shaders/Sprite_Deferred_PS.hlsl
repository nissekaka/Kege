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

GBufferOutput main(PixelInput aInput) : SV_TARGET
{
    float4 albedo = gColourTex.Sample(defaultSampler, aInput.texCoord).rgba;

    if (albedo.a < 0.1f)
    {
        discard;
    }

   // const float2 uv = aInput.position.xy / clientResolution.xy;
    //const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;

    //const float distToWorld = length(worldPosition - aInput.worldPos);
    //const float distToCamera = length(cameraPosition.xyz - aInput.worldPos);

    // Smoothstep the alpha
    //const float worldAlpha = smoothstep(0.0, 15.0f, distToWorld); // * 0.05f;
    //const float cameraAlpha = smoothstep(0.0, 15.0f, distToCamera); // * 0.05f;

    //albedo.a = min(worldAlpha, cameraAlpha);

    GBufferOutput output;
    output.worldPosition = float4(aInput.worldPos, 1.0f);
    output.albedo = float4(albedo.rgb, 1.0f);
    output.normal = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.material = float4(0.0f, 0.5f, 0.0f, 1.0f);
    output.ambientOcclusionAndCustom = float4(0.0f, aInput.worldNormal); // gba are unused, put whatever data you want here!
    output.ambientOcclusionAndCustom.g = 0.0f;
    
    return output;
}