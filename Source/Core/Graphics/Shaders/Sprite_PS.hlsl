#include "deferred_common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

//Texture2D colourTex : register(t11);

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

    if (colour.a < 0.1f)
    {
        discard;
    }

    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;

    const float distToWorld = length(worldPosition - aInput.worldPos);
    const float distToLight = length(lightPosition - aInput.worldPos);

    // Smoothstep the alpha
    const float worldAlpha = smoothstep(0.0, 15.0f, distToWorld);
    float lightAlpha = 0.0f;
    if (IsInSpotlightCone(aInput.worldPos) && lightIntensity > 500.0f)
    {
		if (distToLight < 100.0f)
        {
            lightAlpha = smoothstep(100.0f, 0.0f, distToLight);
        }
        else
        {
            lightAlpha = 0.0f;
        }
    }

    colour.a = min(worldAlpha, lightAlpha) * 0.2f;

	return float4(colour);
}
