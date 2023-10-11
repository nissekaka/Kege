#include "common.hlsli"
#include "PostprocessTonemapPS.hlsl"

struct PixelInput
{
    float4 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitan : BITANGENT;
};

Texture2D colourTex : register(t1);

cbuffer Parameters : register(b1)
{
    float3 tint;
    float exposure;
    float3 blackpoint;
    float contrast;
    float saturation;
};

float4 main(PixelInput aInput) : SV_TARGET
{
    float4 colour = colourTex.Sample(splr, aInput.texCoord).rgba;

    // Saturation
    const float luminance = dot(float3(0.2126, 0.7152, 0.0722), colour.rgb);
    float3 newColour = luminance + saturation * (colour.rgb - luminance);

	// Exposure
    newColour = pow(2, exposure) * newColour;

    // Contrast
    newColour = 0.18f * pow(newColour / 0.18f, contrast); // 0.18 is perceived as 50% grey. Keep it constan

    // Blackpoint
    newColour = max(0.0f, newColour - blackpoint);

    // Tint
    newColour = newColour * tint;

    if (colour.a < 0.1f)
    {
        discard;
    }

    return float4(tonemap_s_gamut3_cine(newColour), 1.0f);
}