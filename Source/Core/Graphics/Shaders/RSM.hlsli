#include "deferred_common.hlsli"
#include "Shadows.hlsli"




#define PI (3.141592653)
#define TWO_PI (2.0 * PI)

// Reverses the bits of the input
uint MyBitfieldReverse(const uint aI)
{
    uint b = (uint(aI) << 16u) | (uint(aI) >> 16u);
    b = (b & 0x55555555u) << 1u | (b & 0xAAAAAAAAu) >> 1u;
    b = (b & 0x33333333u) << 2u | (b & 0xCCCCCCCCu) >> 2u;
    b = (b & 0x0F0F0F0Fu) << 4u | (b & 0xF0F0F0F0u) >> 4u;
    b = (b & 0x00FF00FFu) << 8u | (b & 0xFF00FF00u) >> 8u;
    return b;
}

float2 Hammersley(const uint aI, const uint aN)
{
    return float2(float(aI) / float(aN), float(MyBitfieldReverse(aI)) * 2.3283064365386963e-10);
}

float3 IndirectLighting(const float2 aUv, const float3 aN, const float3 aX, Texture2D aWorldPosTex, Texture2D aFluxTex, Texture2D aNormalTex, const bool aUsePoisson, const float aRMax, const uint aSampleCount, const float aIntensity)
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
            const float3 xp = aWorldPosTex.Sample(clampedSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 flux = aFluxTex.Sample(clampedSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 np = normalize(2.0f * aNormalTex.Sample(clampedSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, aX - xp)) * max(0, dot(aN, xp - aX)))
									/ pow(length(aX - xp), 4));

            Ep *= POISSON_DISK_151[i].x * POISSON_DISK_151[i].x; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }
    else
    {
        //const float t = currentTime;
        //const float2x2 rot = float2x2(cos(t), -sin(t), sin(t), cos(t));

        for (uint i = 0; i < aSampleCount; i++)	// Sum contributions of sampling locations
        {
            //float2 offset = mul(Hammersley(i, sampleCount), rot);
            float2 offset = Hammersley(i, aSampleCount);
            offset = clamp(offset, 0.001f, 0.999f); // Prevents the offset from creating black artifacts
            const float r = offset.x * aRMax;
            const float theta = offset.y * TWO_PI;
            const float2 coord = aUv + float2(r * cos(theta), r * sin(theta));
            const float weight = offset.x * offset.x;

            const float3 xp = aWorldPosTex.Sample(clampedSampler, coord).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
            const float3 flux = aFluxTex.Sample(clampedSampler, coord).rgb; // Collect components from corresponding RSM textures
            const float3 np = normalize(2.0f * aNormalTex.Sample(clampedSampler, coord).xyz - 1.0f);

            float3 Ep = flux * ((max(0, dot(np, aX - xp)) * max(0, dot(aN, xp - aX)))
									/ pow(length(aX - xp), 4));

            Ep *= weight; // Weighting contribution and normalizing

            rsmOutput += Ep; // Accumulate
        }
    }

    return rsmOutput * aIntensity; // Modulate result with some intensity value
}