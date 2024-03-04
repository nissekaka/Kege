
#include "common.hlsli"
#include "RSM.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    float3 rsm;

    if (currentPass == 0)
    {
        const float2 uv = aInput.position.xy / clientResolution.xy / uvScale;

        const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
        const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
        const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);

        const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
        const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

        const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

        rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                        rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                        usePoissonRSM, R_MAX, sampleCount, RSM_INTENSITY) * albedo;
    }
    else
    {
        float2 offsets[4];

        if (currentPass == 1)
        {
            offsets[0] = float2(-1.0f, 1.0f);
            offsets[1] = float2(1.0f, 1.0f);
            offsets[2] = float2(-1.0f, -1.0f);
            offsets[3] = float2(1.0f, -1.0f);
        }
        else if (currentPass > 1)
        {
            offsets[0] = float2(0.0f, 1.0f);
            offsets[1] = float2(0.0f, -1.0f);
            offsets[2] = float2(-1.0f, 0.0f);
            offsets[3] = float2(1.0f, 0.0f);
        }

        float accumWeight = 0.0f;
        float3 accumIndirectLight = float3(0.0f, 0.0f, 0.0f);

        const float atrousWeights[4] = { 0.1201, 0.2339, 0.2931, 0.2339 }; // Example weights, you can adjust these
        const float2 uv = aInput.position.xy / clientResolution.xy;

        for (int i = 0; i < 4; ++i)
        {
            const float2 corner = uv + offsets[i];
            
            // Sample the neighboring pixel
            float3 neighbourLight;

            if (isDirectional)
            {
                neighbourLight = directionalIndirectLightTex.Sample(defaultSampler, corner);

            }
            else
            {
                neighbourLight = spotIndirectLightTex.Sample(defaultSampler, corner);
            }

            // Accumulate with appropriate weights
            const float weight = atrousWeights[i]; // You may use different weights based on distance or other factors
            accumIndirectLight += weight * neighbourLight;
            accumWeight += weight;
        }

        // Normalize by the total weight to get the final result
        rsm = accumIndirectLight / accumWeight;

        if (accumWeight <= weightMax)
        {
            const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
            const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
            const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);

            const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
            const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

            const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

            rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                        rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                        usePoissonRSM, R_MAX, sampleCount, RSM_INTENSITY) * albedo;
        }
    }
    return float4(rsm, 1.0f);
    
}


//const float2 uv = aInput.position.xy / clientResolution.xy;

//float3 indirectLight;

//    if (currentPass == 0)
//    {
//const float3 indirectLightDirectional = directionalIndirectLightTex.Sample(defaultSampler, uv);
//const float3 indirectLightSpot = spotIndirectLightTex.Sample(defaultSampler, uv);

//        indirectLight = indirectLightDirectional +
//indirectLightSpot;
//    }
//    else
//    {
//float2 offsets[4];

//        if (currentPass == 1)
//        {
//            offsets[0] = float2(-1.0f, 1.0f);
//            offsets[1] = float2(1.0f, 1.0f);
//            offsets[2] = float2(-1.0f, -1.0f);
//            offsets[3] = float2(1.0f, -1.0f);
//        }
//        else if (currentPass > 1)
//        {
//            offsets[0] = float2(0.0f, 1.0f);
//            offsets[1] = float2(0.0f, -1.0f);
//            offsets[2] = float2(-1.0f, 0.0f);
//            offsets[3] = float2(1.0f, 0.0f);
//        }

//float accumWeight = 0.0f;
//float3 accumIndirectLight = float3(0.0f, 0.0f, 0.0f);

//const float atrousWeights[4] = { 0.1201, 0.2339, 0.2931, 0.2339 }; // Example weights, you can adjust these

//        for (
//int i = 0;i < 4; ++i)
//        {
//const float2 corner = uv + offsets[i];
            
//            // Sample the neighboring pixel
//const float3 neighborLightDirectional = directionalIndirectLightTex.Sample(defaultSampler, corner);
//const float3 neighborLightSpot = spotIndirectLightTex.Sample(defaultSampler, corner);
//const float3 neighborLight = neighborLightDirectional + neighborLightSpot;

//            // Accumulate with appropriate weights
//const float weight = atrousWeights[i]; // You may use different weights based on distance or other factors
//            accumIndirectLight += weight *
//neighborLight;
//            accumWeight+=
//weight;
//        }

//        // Normalize by the total weight to get the final result
//        indirectLight = accumIndirectLight /
//accumWeight;
//}
//    return float4(indirectLight, 1.0f);
