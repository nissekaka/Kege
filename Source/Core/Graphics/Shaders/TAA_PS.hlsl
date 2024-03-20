#include "common.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer Parameters : register(b1)
{
    uint mode = 0;
    float denoise = 3.0f;
};

Texture2D currentTexture : register(t0);
Texture2D previousTexture : register(t1);

float4 main(const PixelInput aInput) : SV_TARGET
{
    //float2 uv = aInput.position.xy / clientResolution.xy;

    const float3 previousColour = previousTexture.Sample(linearSampler, aInput.texCoord).rgb;
    const float3 currentColour = currentTexture.Sample(linearSampler, aInput.texCoord).rgb;

    float3 output = currentColour * 0.1f + previousColour * 0.9f;

    return float4(output, 1.0f);
}