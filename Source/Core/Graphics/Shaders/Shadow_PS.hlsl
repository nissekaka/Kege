#include "common.hlsli"
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

cbuffer RSMLightData : register(b0)
{
    float4 lightColourAndIntensity;
    float4 lightDirectionAndInnerAngle;
    float4 lightPositionAndOuterAngle;
    float range;
    bool isDirectionalLight;
}

RSMBufferOutput main(PixelInput aInput)
{
    float4 albedo = colourTex.Sample(defaultSampler, aInput.texCoord).rgba;
    if (albedo.a < 0.5f)
    {
        discard;
    }

    float3 normal = float3(normalTex.Sample(defaultSampler, aInput.texCoord).rg, 1.0f);

    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);

    float3x3 TBN = float3x3(normalize(aInput.tangent.xyz),
                            normalize(-aInput.bitan.xyz),
                            normalize(aInput.normal.xyz));
    TBN = transpose(TBN);

    const float3 pixelNormal = normalize(mul(TBN, normal));

    RSMBufferOutput output;

    output.worldPosition = float4(aInput.worldPos, 1.0f);
    if (isDirectionalLight)
    {
        output.flux = float4(albedo.rgb * lightColourAndIntensity.xyz, 1.0f) * lightColourAndIntensity.w;
    }
    else
    {
        float3 toLight = lightPositionAndOuterAngle.xyz - aInput.worldPos.xyz;
        const float lightDistance = length(toLight);
        toLight = normalize(toLight);

        const float NdL = saturate(dot(aInput.worldNormal, toLight));
        const float lambert = NdL; // Angle attenuation

        const float cosOuterAngle = cos(lightPositionAndOuterAngle.w);
        const float cosInnerAngle = cos(lightDirectionAndInnerAngle.w);
        const float3 lightDirection = -lightDirectionAndInnerAngle.xyz;

		// Determine if pixel is within cone.
        const float theta = dot(toLight, normalize(-lightDirection));
		// And if we're in the inner or outer radius.
        const float epsilon = cosInnerAngle - cosOuterAngle;
        float intensity = clamp((theta - cosOuterAngle) / epsilon, 0.0f, 1.0f);
        intensity *= intensity;
	
        const float ue4Attenuation = ((pow(saturate(1 - lightDistance / range /*pow(lightDistance / range, 4.0f)*/), 2.0f)) / (pow(lightDistance, 2.0f) + 1)); // Unreal Engine 4 attenuation
        const float finalAttenuation = lambert * intensity * ue4Attenuation;

        output.flux = float4(albedo.rgb * lightColourAndIntensity.rgb, 1.0f) * finalAttenuation * lightColourAndIntensity.w;
    }

    output.normal = float4(pixelNormal, 1.0f);

    return output;
}