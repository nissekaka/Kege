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
    const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
    const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = materialTex.Sample(defaultSampler, uv);

    const float metalness = material.b;
    const float roughness = material.g;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float4 lightProjectedPositionTemp = mul(spotLightCameraTransform, float4(worldPosition, 1.0f));
    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

    float shadowFactor = Shadow(lightProjectedPosition, spotLightShadowMap) + shadowColour.w;
    shadowFactor = saturate(shadowFactor);

    float3 spotLight = EvaluateSpotLight(colour, specular, normal, roughness, lightColour, lightIntensity,
        lightRange, lightPosition, -lightDirection, lightOuterAngle, lightInnerAngle, toEye,
        worldPosition.xyz);

    if (shadowFactor < 1.0f)
    {
        spotLight *= shadowFactor;
    }

    if (useSpotRSM)
    {
        const float2 pixelOffset = float2(ddx(uv.x), ddy(uv.y));

	    // Could have done one sample in the middle
	    // But that results in some artifacts. This pattern gives a much smoother result
        const float3 p00 = spotIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(-1.0f, -1.0f)).rgb;
        const float3 p01 = spotIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(-1.0f, 1.0f)).rgb;
        const float3 p10 = spotIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(1.0f, -1.0f)).rgb;
        const float3 p11 = spotIndirectLightTex.Sample(defaultSampler, uv + pixelOffset * float2(1.0f, 1.0f)).rgb;

        const float3 indirectLight = (p00 + p01 + p10 + p11);
        //const float3 indirectLight = spotIndirectLightTex.Sample(defaultSampler, uv);;
        if (onlyRSM)
        {
            return float4(indirectLight, 1.0f);
        }
        return float4(spotLight.rgb + indirectLight, 1.0f);
    }
    return float4(spotLight.rgb, 1.0f);
}
