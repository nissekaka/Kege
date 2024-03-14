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
    int numberOfVolumetricSteps;
    float volumetricScattering;
    float volumetricIntensity;
};

static const float4x4 DITHER_PATTERN = float4x4
    (float4(0.0f, 0.5f, 0.125f, 0.625f),
     float4(0.75f, 0.22f, 0.875f, 0.375f),
     float4(0.1875f, 0.6875f, 0.0625f, 0.5625f),
     float4(0.9375f, 0.4375f, 0.8125f, 0.3125f));

// Mie scaterring approximated with Henyey-Greenstein phase function.
float CalcScattering(float cosTheta)
{
    return (1.0f - volumetricScattering * volumetricScattering) / (4.0f * PI * pow(1.0f + volumetricScattering * volumetricScattering - 2.0f * volumetricScattering * cosTheta, 1.5f));
}

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;
    const float4 ambientOcclusionAndCustom = gAmbientOcclusionTex.Sample(defaultSampler, uv).rgba;
    
    const float3 normal = normalize(2.0f * gNormalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = gMaterialTex.Sample(defaultSampler, uv);

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

    float3 volumetric = float3(0.0f, 0.0f, 0.0f);
	float3 V = worldPosition - cameraPosition.xyz;

    const float stepSize = length(V) / (float)numberOfVolumetricSteps;
    V = normalize(V);
    const float3 step = V * stepSize;
 
    float3 position = cameraPosition.xyz;
    position += step * DITHER_PATTERN[int(uv.x * clientResolution.x) % 4][int(uv.y * clientResolution.y) % 4];

    [unroll(15)]
    for (int i = 0; i < numberOfVolumetricSteps; i++)
    {
        const float4 lightSpacePositionTemp = mul(directionalLightCameraTransform, float4(position, 1.0f));
        const float3 lightSpacePosition = lightSpacePositionTemp.xyz / lightSpacePositionTemp.w;

        const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * lightSpacePosition.xy;

        const float shadowMapValue = directionalLightShadowMap.Sample(defaultSampler, sampleUV).r;

        if (shadowMapValue > lightSpacePosition.z)
        {
            volumetric += CalcScattering(dot(V, -dirLightDir)) * directionalLightColour;
        }
        position += step;
    }
    volumetric /= (float)numberOfVolumetricSteps;

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

    return saturate(float4(finalColour + constantAmbiance + ambiance * ambientLightIntensity + volumetric * volumetricIntensity, 1.0f));
}