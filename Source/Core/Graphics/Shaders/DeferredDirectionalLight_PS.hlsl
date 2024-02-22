#include "deferred_common.hlsli"
#include "PBRFunctions.hlsli"
#include "Shadows.hlsli"

cbuffer DirectionalLight : register(b1)
{
    float3 directionalLightDirection;
    float directionalLightIntensity;
    float3 directionalLightColour;
    float ambientLightIntensity;
    float4x4 directionalLightCameraTransform;
};

//float3 GetWorldPositionFromDepth(float2 aUv, float aDepth)
//{
//    float4 ndc = float4(aUv * 2.0f - 1.0f, aDepth, 1.0f);
//    //ndc.y *= -1.0f;
//    float4 wp = mul(clipToWorldMatrix, ndc);
//    //return wp;
//    return (wp.xyz / wp.w).xyz;
//}

//float3 GetWorldPosFromDepth(float3 aViewRay, float aDepth)
//{
//    float4 viewspacePosition = float4((aViewRay * aDepth).xyz, 1.0f);
//    float4 worldPosition = mul(clipToWorldMatrix, viewspacePosition);
//    return worldPosition.xyz;
//}
cbuffer RSMData : register(b3)
{
    bool useRSM;
    bool onlyRSM;
    bool usePoissonRSM;
    uint sampleCount;
    float R_MAX;
    float RSM_INTENSITY;
};

//#define SAMPLES 400u

#define PI (3.141592653)
#define TWO_PI (2.0 * PI)

// reverses the bits of the input
uint MyBitfieldReverse(uint i)
{
    uint b = (uint(i) << 16u) | (uint(i) >> 16u);
    b = (b & 0x55555555u) << 1u | (b & 0xAAAAAAAAu) >> 1u;
    b = (b & 0x33333333u) << 2u | (b & 0xCCCCCCCCu) >> 2u;
    b = (b & 0x0F0F0F0Fu) << 4u | (b & 0xF0F0F0F0u) >> 4u;
    b = (b & 0x00FF00FFu) << 8u | (b & 0xFF00FF00u) >> 8u;
    return b;
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), float(MyBitfieldReverse(i)) * 2.3283064365386963e-10);
}

float3 IndirectLighting(float2 uv, float3 n, float3 x)
{
    // The irradiance at a surface point x with normal n due to pixel light p is
    //
    //                   max{0,<np|x-xp>} max{0,<n| xp-x>}
    // Ep(x, n) = phi(p) -----------------------------------             
	//                              ||x-xp||^4

    float3 rsmOutput = { 0.0f, 0.0f, 0.0f };

    if (usePoissonRSM)
    {
        for (int i = 0; i < 64; i++)	// Sum contributions of sampling locations
        {
            const float2 coord = uv + R_MAX * POISSON_DISK_64[i];
            const float3 flux = RSM_FluxTex.Sample(clampedSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 xp = RSM_WorldPositionTex.Sample(clampedSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 np = normalize(2.0f * RSM_NormalTex.Sample(clampedSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, x - xp)) * max(0, dot(n, xp - x)))
									/ pow(length(x - xp), 4));

            Ep *= POISSON_DISK_64[i].x * POISSON_DISK_64[i].x; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }
    else
    {
        //const float t = currentTime;
        //const float2x2 rot = float2x2(cos(t), -sin(t), sin(t), cos(t));

        for (int i = 0; i < sampleCount; i++)	// Sum contributions of sampling locations
        {
            //float2 offset = mul(Hammersley(i, sampleCount), rot);
            float2 offset = Hammersley(i, sampleCount);
            const float r = offset.x * R_MAX;
            const float theta = offset.y * TWO_PI;
            const float2 coord = uv + float2(r * cos(theta), r * sin(theta));
            const float weight = offset.x * offset.x;

            const float3 flux = RSM_FluxTex.Sample(clampedSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 xp = RSM_WorldPositionTex.Sample(clampedSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 np = normalize(2.0f * RSM_NormalTex.Sample(clampedSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, x - xp)) * max(0, dot(n, xp - x)))
									/ pow(length(x - xp), 4));

            Ep *= weight; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }



    return RSM_INTENSITY * rsmOutput; // Modulate result with some intensity value
}

float4 main(DeferredVertexToPixel aInput) : SV_TARGET
{
    const float2 uv = aInput.position.xy / clientResolution.xy;
    const float3 worldPosition = worldPositionTex.Sample(defaultSampler, uv).rgb;
    //const float depth = depthTex.Sample(defaultSampler, uv).r;
    //const float3 worldPosition = GetWorldPositionFromDepth(aInput.position, depth);
    const float3 albedo = colourTex.Sample(defaultSampler, uv).rgb;
    const float4 ambientOcclusionAndCustom = ambientOcclusionTex.Sample(defaultSampler, uv).rgba;
    
    const float3 normal = normalize(2.0f * normalTex.Sample(defaultSampler, uv).xyz - 1.0f);
    const float4 material = materialTex.Sample(defaultSampler, uv);

    // Reflective Shadow Maps -- Indirect lighting -- START

    const float4 directionalLightProjectedPositionTemp = mul(directionalLightCameraTransform, float4(worldPosition, 1.0f));
    float3 directionLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;

    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (directionLightProjectedPosition.xy);

    float3 rsm = float3(0, 0, 0);
    if (useRSM)
    {
        rsm = IndirectLighting(sampleUV, normal, worldPosition);
    }

    // Reflective Shadow Maps -- Indirect lighting -- END

    const float metalness = material.b;
    const float roughness = material.g;

    const float3 specular = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 colour = lerp((float3) 0.0f, albedo.rgb, 1.0f - metalness);

    const float3 toEye = normalize(cameraPosition.xyz - worldPosition);

    const float shadowFactor = Shadow(directionalLightCameraTransform, float4(worldPosition, 1.0f));

    float3 dirLightDir = directionalLightDirection;
    dirLightDir.x = -dirLightDir.x;
    const float3 directionalLight = EvaluateDirectionalLight(colour, specular, normal,
    roughness, directionalLightColour, -dirLightDir, toEye) * directionalLightIntensity;
    
    const float3 finalColour = directionalLight * shadowFactor;

    const float3 lightDir = normalize(directionalLightDirection);
    const float dotProduct = dot(lightDir, float3(0.0f, 1.0f, 0.0f));
    const float blendFactor = (dotProduct + 1.0) / 2.0f;

    const float ambiance = EvaluateAmbianceDynamicSky(defaultSampler, daySkyTex, nightSkyTex, blendFactor,
    normal, normal, toEye, roughness, ambientOcclusionAndCustom.r, colour, specular);

    if (useRSM)
    {
        if (onlyRSM)
        {
            return float4(rsm, 1.0f);
        }
        return float4(finalColour + ambiance * ambientLightIntensity + rsm, 1.0f);
    }
    return float4(finalColour + ambiance * ambientLightIntensity, 1.0f);
}