#include "PBRFunctions.hlsli"
#include "RSM.hlsli"

cbuffer SpotlightData : register(b2)
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

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;
    const float3 normal = normalize(2.0f * gNormalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = gMaterialTex.Sample(defaultSampler, uv);

    const float metalness = material.b;
    const float roughness = material.g;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float4 lightProjectedPositionTemp = mul(spotLightCameraTransform, float4(worldPosition, 1.0f));
    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

    float shadowFactor = Shadow(lightProjectedPosition, spotLightShadowMap) + shadowColour.w;
    shadowFactor = saturate(shadowFactor);

    float2 projectedPosition = lightProjectedPosition.xy;
    projectedPosition.x *= clientResolution.x / clientResolution.y; // Because texture has aspect ratio 1:1
    const float2 lightUV = 0.5f + float2(0.5f, -0.5f) * (projectedPosition.xy) / lightOuterAngle;
    const float3 light = flashlightTex.Sample(defaultSampler, lightUV).rgb;

    float3 spotlight = EvaluateSpotLight(colour, specular, normal, roughness, light * lightColour, lightIntensity,
        lightRange, lightPosition, -lightDirection, lightOuterAngle, lightInnerAngle, toEye,
        worldPosition.xyz);

    if (shadowFactor < 1.0f)
    {
        spotlight *= shadowFactor;
    }

    return float4(spotlight.rgb, 1.0f);
}
