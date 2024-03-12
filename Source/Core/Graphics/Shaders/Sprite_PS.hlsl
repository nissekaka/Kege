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
    float4 colour = gColourTex.Sample(defaultSampler, aInput.texCoord).rgba;
    colour.rgb *= aInput.colour.rgb;

    const float distToLight = length(lightPosition - aInput.worldPos);
    
    float spotlightAlpha = 0.0f;
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

    colour.a *= spotlightAlpha * aInput.colour.a;

    return float4(colour);
}
