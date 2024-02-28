//#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"
//#include "Shadows.hlsli"
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

cbuffer RSMData : register(b5)
{
    bool useRSM;
    bool onlyRSM;
    bool usePoissonRSM;
    uint sampleCount;
    float R_MAX;
    float RSM_INTENSITY;
    float2 paddingRSM;
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

	// Reflective Shadow Maps -- Indirect lighting -- START

    const float4 lightProjectedPositionTemp = mul(spotLightCameraTransform, float4(worldPosition, 1.0f));
    float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

    float3 rsm = float3(0, 0, 0);
    if (useRSM)
    {
        rsm = IndirectLighting(sampleUV, normal, worldPosition,
        rsmSpotlightWorldPositionTex, rsmSpotlightFluxTex, rsmSpotlightNormalTex,
        usePoissonRSM, R_MAX, sampleCount, RSM_INTENSITY); // * colour;
    }

    // Reflective Shadow Maps -- Indirect lighting -- END

    float shadowFactor = Shadow(lightProjectedPosition, spotLightShadowMap) + shadowColour.w;
    shadowFactor = saturate(shadowFactor);

    float3 spotLight = EvaluateSpotLight(colour, specular, normal, roughness, lightColour, lightIntensity,
        lightRange, lightPosition, -lightDirection, lightOuterAngle, lightInnerAngle, toEye,
        worldPosition.xyz);

    if (shadowFactor < 1.0f)
    {
        spotLight *= shadowFactor; //.rgb;
        //spotLight *= shadowColour.rgb;
    }

    if (useRSM)
    {
        if (onlyRSM)
        {
            return float4(rsm, 1.0f);
        }
        return float4(spotLight + rsm, 1.0f);
    }
    return float4(spotLight.rgb, 1.0f);
}
