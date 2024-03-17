
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

        //const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;
        const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;
        const float3 normal = normalize(2.0f * gNormalTex.Sample(defaultSampler, uv).xyz - 1.0f);

        const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
        const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

        const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

        rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                        rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                        usePoissonRSM, rMax, sampleCount, rsmIntensity, type); // * albedo;
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

        float2 uv;
        if (currentPass == 1)
        {
            uv = (aInput.position.xy + float2(1.0f, 1.0f)) / clientResolution.xy;
        }
        if (currentPass == 2)
        {
            uv = (aInput.position.xy + float2(1.0f, 0.0f)) / clientResolution.xy;
        }
        if (currentPass == 3)
        {
            uv = (aInput.position.xy + float2(0.0f, 1.0f)) / clientResolution.xy;
        }
            
        float3 accumIndirectLight = float3(0.0f, 0.0f, 0.0f);
        float accumWeight = 0.0f;

        const float3 normal = normalize(2.0f * gNormalTex.Sample(defaultSampler, uv).xyz - 1.0f);
        const float3 worldPosition = gWorldPositionTex.Sample(defaultSampler, uv).rgb;

        for (int i = 0; i < 4; ++i)
        {
            const float2 corner = uv + offsets[i] * (1.0 / clientResolution.xy);

            if (corner.x < 0.0f || corner.x > 1.0f || corner.y < 0.0f || corner.y > 1.0f)
            {
                continue; // Skip out-of-bounds samples
            }

            // Sample the neighboring pixel
            float3 c;

            if (isDirectional)
            {
                c = giDirectionalTex.Sample(defaultSampler, corner);

            }
            else
            {
                c = giSpotTex.Sample(defaultSampler, corner);
            }

            if (c.x == 0.0f && c.y == 0.0f && c.z == 0.0f)
            {
                return float4(0.0f, 0.0f, 0.0f, 0.0f); // Skip black samples
            }

            const float3 n = normalize(2.0f * gNormalTex.Sample(defaultSampler, corner).xyz - 1.0f);
            const float3 dn = normal - n;
            const float n_weight = exp(-dot(dn, dn) / divideN);

            const float3 p = gWorldPositionTex.Sample(defaultSampler, corner).rgb;
            const float dp = worldPosition - p;
            const float p_weight = exp(-dot(dp, dp) / divideP);

            const float weight = n_weight * p_weight;

            accumIndirectLight += c * weight;
            accumWeight += weight;
        }

		// Normalize by the total weight to get the final result
        rsm = accumIndirectLight / accumWeight;

        //if (currentPass == 3)
        //{
            if (accumWeight <= weightMax)
            {
                //const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;

                if (mode == 0)
                {
                    const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
                    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

                    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

                    rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                        rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                        usePoissonRSM, rMax, sampleCountLastPass, rsmIntensity, 2u);
                }
                if (mode == 1)
                {
                    const float3 albedo = gColourTex.Sample(defaultSampler, uv).rgb;

                    const float4 lightProjectedPositionTemp = mul(lightCameraTransform, float4(worldPosition, 1.0f));
                    const float3 lightProjectedPosition = lightProjectedPositionTemp.xyz / lightProjectedPositionTemp.w;

                    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (lightProjectedPosition.xy);

                    rsm = IndirectLighting(sampleUV, normal, worldPosition,
                                        rsmWorldPositionTex, rsmFluxTex, rsmNormalTex,
                                        usePoissonRSM, rMax, sampleCountLastPass, rsmIntensity, 2u) * albedo;
                }
                if (mode == 2)
                {
                    rsm = float3(1.0f, 0.0f, 0.0f); // Red debug colour
                }
            }
        //}
    }
    return float4(rsm, 1.0f);
}
