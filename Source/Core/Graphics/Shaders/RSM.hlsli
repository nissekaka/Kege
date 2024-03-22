#include "deferred_common.hlsli"
#include "Poisson.hlsli"
#include "common.hlsli"

Texture2D rsmWorldPositionTex : register(t6);
Texture2D rsmNormalTex : register(t7);
Texture2D rsmFluxTex : register(t8);

cbuffer HammersleyDataDirectional : register(b5)
{
    float4 hSamplesDirectional[256];
};

cbuffer HammersleyDataSpot : register(b6)
{
    float4 hSamplesSpot[64];
};

cbuffer HammersleyDataFinal : register(b7)
{
    float4 hSamplesFinal[32];
};

cbuffer RSMData : register(b3)
{
    bool usePoissonRSM;
    bool isDirectional;
    uint mode;
    uint sampleCount;
    uint sampleCountLastPass;
    uint currentPass;
    uint type;
    float rMax;
    float rsmIntensity;
    float uvScale;
    float weightMax;
    float divideN;
    float divideP;
    float3 padding;
    float4 shadowColour;
    float4 ambianceColour;
    float4x4 lightCameraTransform;
};

#define PI (3.141592653)
#define TWO_PI (2.0 * PI)

// Reverses the bits of the input
uint BitfieldReverse(const uint aBits)
{
    uint b = (uint(aBits) << 16u) | (uint(aBits) >> 16u);
    b = (b & 0x55555555u) << 1u | (b & 0xAAAAAAAAu) >> 1u;
    b = (b & 0x33333333u) << 2u | (b & 0xCCCCCCCCu) >> 2u;
    b = (b & 0x0F0F0F0Fu) << 4u | (b & 0xF0F0F0F0u) >> 4u;
    b = (b & 0x00FF00FFu) << 8u | (b & 0xFF00FF00u) >> 8u;
    return b;
}

float2 Hammersley(const uint aI, const uint aN)
{
    return float2(float(aI) / float(aN), float(BitfieldReverse(aI)) * 2.3283064365386963e-10);
}

float IGN(float2 pixel)
{
    return fmod(52.9829189f * fmod(0.06711056f * float(pixel.x) + 0.00583715f * float(pixel.y), 1.0f), 1.0f);
}

float2 hash22(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.xx + p3.yz) * p3.zy);

}

float2 hash23(float3 p3)
{
    p3 = frac(p3 * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.xx + p3.yz) * p3.zy);
}


float3 IndirectLighting(const float2 aUv, const float3 aN, const float3 aX, Texture2D aWorldPosTex, Texture2D aFluxTex, Texture2D aNormalTex, const bool aUsePoisson, const float aRMax, const uint aSampleCount, const float aIntensity, const uint aType, const float2 aPixel)
{
    // The irradiance at a surface point x with normal n due to pixel light p is
    //
    //                     max{0,<np|aX-xp>} max{0,<aN| xp-aX>}
    // Ep(aX, aN) = phi(p) -----------------------------------             
	//                                ||aX-xp||^4

    float3 rsmOutput = { 0.0f, 0.0f, 0.0f };

    if (aUsePoisson)
    {
        for (int i = 0; i < 151; i++)	// Sum contributions of sampling locations
        {
            const float2 coord = aUv + aRMax * POISSON_DISK_151[i];
            const float3 xp = aWorldPosTex.Sample(pointSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 flux = aFluxTex.Sample(pointSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 np = normalize(2.0f * aNormalTex.Sample(pointSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, aX - xp)) * max(0, dot(aN, xp - aX)))
									/ pow(length(aX - xp), 4));

            Ep *= POISSON_DISK_151[i].x * POISSON_DISK_151[i].x; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }
    else
    {
        const float2 o = hash23(float3(aPixel.x, aPixel.y, currentTime * 1000.0f));
        //float2 o2 = IGN(pixel) + hash22(float2(currentTime * 1000.0f, 0.f));

		[loop]
        for (uint i = 0; i < aSampleCount; i++)	// Sum contributions of sampling locations
        {
            //float2 offset = mul(Hammersley(i, sampleCount), rot);
            float2 offset = Hammersley(i, aSampleCount);

            offset.xy += o;
            offset.xy = fmod(offset.xy, 1.0f);

            // Soft radius rather than hard cutoff
            offset.x = (offset.x * offset.x * offset.x * offset.x + 1.0f) * offset.x;

            const float r = offset.x * aRMax;
            const float theta = offset.y * TWO_PI;
            const float2 coord = aUv + float2(r * cos(theta), r * sin(theta));
            const float weight = offset.x * offset.x;

            const float3 xp = aWorldPosTex.Sample(pointSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 flux = aFluxTex.Sample(pointSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 np = normalize(2.0f * aNormalTex.Sample(pointSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, aX - xp)) * max(0, dot(aN, xp - aX)))
									/ pow(length(aX - xp), 4));

            Ep *= weight; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }

    return rsmOutput / aSampleCount * aIntensity; // Modulate result with some intensity value
}