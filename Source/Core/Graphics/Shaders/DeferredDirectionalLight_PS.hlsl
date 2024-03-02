//#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"
//#include "Shadows.hlsli"
#include "RSM.hlsli"

cbuffer DirectionalLight : register(b1)
{
    float3 directionalLightDirection;
    float directionalLightIntensity;
    float3 directionalLightColour;
    float ambientLightIntensity;
    float4x4 directionalLightCameraTransform;
    float4x4 spotLightCameraTransform;
};

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
    const float4 ambientOcclusionAndCustom = ambientOcclusionTex.Sample(defaultSampler, uv).rgba;
    
    const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = materialTex.Sample(defaultSampler, uv);

    const float roughness = material.g;
    const float metalness = material.b;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float4 lightProjectedPositionTemp = mul(directionalLightCameraTransform, float4(worldPosition, 1.0f));
    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

    float shadowFactor = Shadow(lightProjectedPosition, directionalLightShadowMap) + shadowColour.w;
    shadowFactor = saturate(shadowFactor);

    float3 dirLightDir = directionalLightDirection;
    dirLightDir.x = -dirLightDir.x;

    const float3 directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, directionalLightColour, -dirLightDir, toEye) * directionalLightIntensity;

    float3 finalColour = directionalLight * shadowFactor;

    if (shadowFactor < 1.0f)
    {
        finalColour *= shadowColour.rgb;
    }

    const float3 lightDir = normalize(directionalLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f;

    const float3 ambiance = EvaluateAmbianceDynamicSky(defaultSampler, daySkyTex, nightSkyTex, blendFactor,
    normal, ambientOcclusionAndCustom.gba, toEye, roughness, ambientOcclusionAndCustom.r, colour, specular);

    const float3 constantAmbiance = colour * ambianceColour.rgb * ambianceColour.w;

    if (useDirectionalRSM)
    {
        const float2 pixelOffset = float2(ddx(uv.x), ddy(uv.y));
        
	    // Could have done one sample in the middle
	    // But that results in some artifacts. This pattern gives a much smoother result
        const float3 p00 = directionalIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(-1.0f, -1.0f)).rgb;
        const float3 p01 = directionalIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(-1.0f, 1.0f)).rgb;
        const float3 p10 = directionalIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(1.0f, -1.0f)).rgb;
        const float3 p11 = directionalIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(1.0f, 1.0f)).rgb;
        
        const float3 indirectLight = (p00 + p01 + p10 + p11);
        //const float3 indirectLight = directionalIndirectLightTex.Sample(defaultSampler, uv);;
        if (onlyRSM)
        {
            return float4(indirectLight, 1.0f);
        }
        return saturate(float4(finalColour + constantAmbiance + ambiance * ambientLightIntensity + indirectLight, 1.0f));
    }
    return saturate(float4(finalColour + constantAmbiance + ambiance * ambientLightIntensity, 1.0f));
}