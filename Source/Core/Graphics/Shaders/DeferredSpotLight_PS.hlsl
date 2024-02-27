#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"
#include "Shadows.hlsli"

cbuffer SpotLightData : register(b2)
{
    float3 lightPosition;
    float lightIntensity;
    float3 lightDirection;
    float lightRange;
    float3 lightColour;
    float lightInnerAngle;
    float lightOuterAngle;
    bool lightIsActive;
    float4x4 spotLightCameraTransform;
    float2 padding;
};

cbuffer RSMData : register(b3)
{
    bool useRSM;
    bool onlyRSM;
    bool usePoissonRSM;
    uint sampleCount;
    float R_MAX;
    float RSM_INTENSITY;
    float2 padding2;
    float4 shadowColour;
    float4 ambianceColour;
    //float shadowIntensity;
};

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
    const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = materialTex.Sample(defaultSampler, uv);

    const float metalness = material.b;
    const float roughness = material.g;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    float shadowFactor = Shadow(spotLightCameraTransform, float4(worldPosition, 1.0f), spotLightShadowMap) + shadowColour.w;
    shadowFactor = saturate(shadowFactor);

    float3 spotLight = EvaluateSpotLight(colour, specular, normal, roughness, lightColour, lightIntensity,
        lightRange, lightPosition, -lightDirection, lightOuterAngle, lightInnerAngle, toEye,
        worldPosition.xyz);

    if (shadowFactor < 1.0f)
    {
        spotLight *= shadowFactor; //.rgb;
        //spotLight *= shadowColour.rgb;
    }

    return float4(spotLight.rgb, 1.0f);
}
