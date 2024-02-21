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

//float3 GetWorldPositionFromDepth(float2 aUv, float aDepth)
//{
//    float4 ndc = float4(aUv * 2.0f - 1.0f, aDepth, 1.0f);
//    //ndc.y *= -1.0f;
//    float4 wp = mul(clipToWorldMatrix, ndc);
//    //return wp;
//    return (wp.xyz / wp.w).xyz;
//}

//float3 GetWorldPosFromDepth(float3 aViewRay, float aDepth)
//{
//    float4 viewspacePosition = float4((aViewRay * aDepth).xyz, 1.0f);
//    float4 worldPosition = mul(clipToWorldMatrix, viewspacePosition);
//    return worldPosition.xyz;
//}

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
	const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
    //const float depth = depthTex.Sample(defaultSampler, uv).r;
    //const float3 worldPosition = GetWorldPositionFromDepth(aInput.position, depth);
    const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
    const float4 ambientOcclusionAndCustom = ambientOcclusionTex.Sample(defaultSampler, uv).rgba;
    
    const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);    
    const float4 material = materialTex.Sample(defaultSampler, uv);

	const float metalness = material.b;
    const float roughness = material.g;

	const float3 specular = lerp((float3)0.04f, albedo.rgb, metalness);
	const float3 colour = lerp((float3)0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float shadowFactor = Shadow(directionalLightCameraTransform, float4(worldPosition, 1.0f));

    const float3 directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, directionalLightColour, -directionalLightDirection, toEye) * directionalLightIntensity;
    
    const float3 finalColour = directionalLight * shadowFactor;

    const float3 lightDir = normalize(directionalLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f;

    const float ambiance = EvaluateAmbianceDynamicSky(defaultSampler, daySkyTex, nightSkyTex, blendFactor,
    normal, normal, toEye, roughness, ambientOcclusionAndCustom.r, colour, specular);
    
    return float4(finalColour + ambiance * ambientLightIntensity, 1.0f);
}