
//#include "common.hlsli"
#include "RSM.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    //if (fmod(aInput.position.x, 2.0f) == 0.0f && fmod(aInput.position.y, 2.0f) == 0.0f)
    //{
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //}

    const float2 uv = aInput.position.xy / clientResolution.xy / uvScale;

    const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;
    const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;
    const float3 normal = normalize(2.0f * gNormalTex.Sample(defaultSampler, uv).xyz - 1.0f);

    const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

    float3 rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                      rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                      usePoissonRSM, rMax, sampleCount, rsmIntensity, type, aInput.position.xy) * albedo;

    return float4(rsm, 1.0f);
}
