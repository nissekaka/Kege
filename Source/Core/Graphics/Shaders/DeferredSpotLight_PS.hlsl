#include "PBRFunctions.hlsli"
#include "RSM.hlsli"
#include "Volumetric.hlsli"

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
    bool useTexture;
    float4x4 spotLightCameraTransform;
    bool useVolumetricLight;
    uint numberOfVolumetricSteps;
    float volumetricScattering;
    float volumetricIntensity;
    float volumetricAngle;
    float volumetricRange;
    float volumetricFade;
};

bool IsInSpotlightCone(float3 aWorldPosition, float aAngle)
{
    const float3 toLight = lightPosition - aWorldPosition;
    const float distToLight = length(toLight);
    const float3 lightDir = normalize(toLight);
    const float angle = dot(lightDir, lightDirection);

    return (angle > cos(aAngle)) && (distToLight < lightRange);
}

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

    float3 lightFromTexture = float3(1.0f, 1.0f, 1.0f);

    if (useTexture)
    {
        float2 projectedPosition = lightProjectedPosition.xy;
        projectedPosition.x *= clientResolution.x / clientResolution.y; // Because texture has aspect ratio 1:1
        const float2 lightUV = 0.5f + float2(0.5f, -0.5f) * (projectedPosition.xy) / lightOuterAngle;

        lightFromTexture = flashlightTex.Sample(defaultSampler, lightUV).rgb;
    }

    float3 volumetric = float3(0.0f, 0.0f, 0.0f);
    if (useVolumetricLight && lightIntensity > 100.0f)
    {
        float3 V = worldPosition - cameraPosition.xyz;

        const float stepSize = length(V) / (float) numberOfVolumetricSteps;
        V = normalize(V);
        const float3 step = V * stepSize;
 
        float3 position = cameraPosition.xyz;
        position += step * DITHER_PATTERN[int(uv.x * clientResolution.x) % 4][int(uv.y * clientResolution.y) % 4];
        
		[unroll(15)]
        for (int i = 0; i < numberOfVolumetricSteps; i++)
        {
            if (IsInSpotlightCone(position, volumetricAngle))
            {
                const float distToCamera = length(position - cameraPosition.xyz);

                float intensity = 0.0f;
                if (distToCamera < volumetricRange)
                {
                    const float fadeStart = volumetricRange - volumetricFade;

                    // Use smoothstep to interpolate between 1.0 and 0.0 based on the distance to the camera
                    intensity = smoothstep(volumetricRange, fadeStart, distToCamera);
                }
                volumetric += CalcScattering(dot(V, -lightDirection), volumetricScattering) * lightColour * intensity;
            }
            position += step;
        }
        volumetric /= (float) numberOfVolumetricSteps;
        volumetric *= volumetricIntensity;
    }

    float3 spotlight = EvaluateSpotLight(colour, specular, normal, roughness, lightFromTexture * lightColour, lightIntensity,
        lightRange, lightPosition, -lightDirection, lightOuterAngle, lightInnerAngle, toEye,
        worldPosition.xyz);

    if (shadowFactor < 1.0f)
    {
        spotlight *= shadowFactor;
    }

    return float4(spotlight.rgb + volumetric, 1.0f);
}
