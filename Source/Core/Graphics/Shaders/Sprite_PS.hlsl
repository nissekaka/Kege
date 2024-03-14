#include "deferred_common.hlsli"
#include "Shadows.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
    float4 colour : INSTANCE_COLOUR;
};

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
    float padding;
};

bool IsInSpotlightCone(float3 aWorldPosition)
{
    const float3 toLight = lightPosition - aWorldPosition;
    const float distToLight = length(toLight);
    const float3 lightDir = normalize(toLight);
    const float angle = dot(lightDir, lightDirection);

    return (angle > cos(lightInnerAngle + (lightOuterAngle - lightInnerAngle) / 2.0f)) && (distToLight < lightRange);
}

float4 main(PixelInput aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;

    float4 colour = gColourTex.Sample(defaultSampler, aInput.texCoord).rgba;
    colour.rgb *= aInput.colour.rgb;

    const float distToLight = length(lightPosition - aInput.worldPos);
    
    float spotlightAlpha = 0.0f;
    float directionalLightAlpha = 0.0f;
    if (IsInSpotlightCone(aInput.worldPos) && lightIntensity > 500.0f)
    {
        if (distToLight < 100.0f)
        {
            spotlightAlpha = smoothstep(100.0f, 0.0f, distToLight);
        }
        else
        {
            spotlightAlpha = 0.0f;
        }
    }
    //else
    //{
    //    //float3 V = aInput.worldPos - cameraPosition.xyz;

    //    //const float stepSize = length(V) / 20.0f;
    //    //V = normalize(V);
    //    //const float3 step = V * stepSize;
 
    //    //float3 position = cameraPosition.xyz;
    //    //position += step;// * DITHER_PATTERN[int(uv.x * clientResolution.x) % 4][int(uv.y * clientResolution.y) % 4];
        
    //    const float4 lightSpacePositionTemp = mul(directionalLightCameraTransform, float4(aInput.worldPos, 1.0f));
    //    const float3 lightSpacePosition = lightSpacePositionTemp.xyz / lightSpacePositionTemp.w;

    //    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * lightSpacePosition.xy;

    //    const float shadowMapValue = directionalLightShadowMap.Sample(defaultSampler, sampleUV).r;

    //    if (shadowMapValue + 0.57f < lightSpacePosition.z)
    //    {
    //        directionalLightAlpha = 1.0f;
    //    }
    //}

    colour.a *= (spotlightAlpha + directionalLightAlpha) * aInput.colour.a;

    return float4(colour);
}
