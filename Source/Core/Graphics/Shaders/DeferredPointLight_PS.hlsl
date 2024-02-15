#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"

cbuffer PointLightData : register(b2)
{
    float3 lightPosition;
    float lightIntensity;
    float3 lightColour;
    float lightRadius;
    bool lightIsActive;
    float3 padding;
};

//float3 get_world_position_from_depth(float2 uv, float depth)
//{
//    float4 ndc = float4(uv * 2.0f - 1.0f, depth, 1.0f);
//    ndc.y *= -1.0f;
//    float4 wp = mul(ndc, clipToWorldSpaceMatrix);
//    return (wp / wp.w).xyz;
//}

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(splr, uv).rgb;

    const float3 albedo = colourTex.Sample(splr, uv).rgb;
    const float3 normal = normalize(2.0f * normalTex.Sample(splr, uv).xyz - 1.0f);
    const float4 material = materialTex.Sample(splr, uv);

    if (material.a == 0.0f)
    {
        discard;
    }

    const float metalness = material.b;
    const float roughness = material.g;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1 - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition.xyz);

    float3 pointLight = EvaluatePointLight(colour, specular, normal,
        roughness, lightColour, lightIntensity,
        lightRadius, lightPosition, toEye.xyz, worldPosition.xyz);
    
    float3 finalColour = pointLight;

    return float4(finalColour.rgb, 1.0f);
}
