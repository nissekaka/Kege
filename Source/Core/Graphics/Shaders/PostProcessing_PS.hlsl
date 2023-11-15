#include "common.hlsli"
#include "PostprocessTonemap.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D fullscreenTexture : register(t0);
Texture2D bloomSampleTexture : register(t1);

cbuffer Parameters : register(b1)
{
    float3 tint;
    float exposure;
    float3 blackpoint;
    float contrast;
    float saturation;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    const float4 colour = fullscreenTexture.Sample(splr, aInput.texCoord).rgba;

	if (colour.a < 0.1f)
    {
        discard;
    }

	const float4 bloom = bloomSampleTexture.Sample(splr, aInput.texCoord).rgba;

    float3 newColour = colour + bloom;

    // Saturation
    const float luminance = dot(float3(0.2126, 0.7152, 0.0722), colour.rgb);
    newColour = luminance + saturation * (newColour - luminance);

	// Exposure
    newColour = pow(2, exposure) * newColour;

    // Contrast
    newColour = 0.18f * pow(newColour / 0.18f, contrast); // 0.18 is perceived as 50% grey. Keep it constan

    // Blackpoint
    newColour = max(0.0f, newColour - blackpoint);

    // Tint
    newColour = newColour * tint;

    return float4(tonemap_s_gamut3_cine(newColour), 1.0f);
}