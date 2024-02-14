#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"
#include "Shadows.hlsli"

cbuffer DirectionalLight : register(b1)
{
    float3 directionalLightDirection;
    float directionalLightIntensity;
    float3 directionalLightColour;
    float ambientLightIntensity;
    float4x4 directionalLightCameraTransform;
};

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
	const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(splr, uv).rgb;
    const float3 albedo = colourTex.Sample(splr, uv).rgb;
    const float4 ambientOcclusionAndCustom = ambientOcclusionTex.Sample(splr, uv).rgba;
    
    const float3 normal = normalize(2.0f * normalTex.Sample(splr, uv).xyz - 1.0f);    
    const float4 material = materialTex.Sample(splr, uv);

	const float metalness = material.b;
    const float roughness = material.g;

	const float3 specular = lerp((float3)0.04f, albedo.rgb, metalness);
	const float3 colour = lerp((float3)0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float shadowFactor = Shadow(directionalLightCameraTransform, float4(worldPosition, 1.0f));

    float3 directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, directionalLightColour, -directionalLightDirection, toEye) * directionalLightIntensity;
    
    float3 finalColour = directionalLight * shadowFactor;//    +colour;

    float3 ambiance = 0.0f;
    
    return float4(finalColour + ambiance, 1.0f);
}