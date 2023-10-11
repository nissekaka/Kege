#include "common.hlsli"

cbuffer Bloom : register(b1)
{
    float bloomBlending;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D fullscreenTexture : register(t0);

float4 main(PixelInput aInput) : SV_TARGET
{
    float4 returnValue;
	// ddx and ddy measures how much a parameter changes in x and y-direction per pixel
	// could have used
    const float2 pixelOffset = float2(ddx(aInput.texCoord.x), ddy(aInput.texCoord.y));
	// Could have done one sample in the middle
	// But that results in some artifacts. This pattern gives a much smoother result
    const float3 p00 = fullscreenTexture.Sample(splr, aInput.texCoord + pixelOffset * float2(-0.5f, -0.5f)).rgb;
    const float3 p01 = fullscreenTexture.Sample(splr, aInput.texCoord + pixelOffset * float2(-0.5f, 0.5f)).rgb;
    const float3 p10 = fullscreenTexture.Sample(splr, aInput.texCoord + pixelOffset * float2(0.5f, -0.5f)).rgb;
    const float3 p11 = fullscreenTexture.Sample(splr, aInput.texCoord + pixelOffset * float2(0.5f, 0.5f)).rgb;

    returnValue.rgb = 0.25f * (p00 + p01 + p10 + p11);
    returnValue.a = 1.0f;

	return returnValue;
}