#include "deferred_common.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer Parameters : register(b1)
{
    uint mode = 0;
};

Texture2D currentFrameTexture : register(t0);
Texture2D lastFrameTexture : register(t1);

// YUV-RGB conversion routine from Hyper3D
float3 encodePalYuv(float3 rgb)
{
    rgb = pow(rgb, float3(2.0, 2.0, 2.0)); // gamma correction
    return float3(
        dot(rgb, float3(0.299, 0.587, 0.114)),
        dot(rgb, float3(-0.14713, -0.28886, 0.436)),
        dot(rgb, float3(0.615, -0.51499, -0.10001))
    );
}

float3 decodePalYuv(float3 yuv)
{
    float3 rgb = float3(
        dot(yuv, float3(1., 0., 1.13983)),
        dot(yuv, float3(1., -0.39465, -0.58060)),
        dot(yuv, float3(1., 2.03211, 0.))
    );
    return pow(rgb, float3(0.5, 0.5, 0.5)); // gamma correction
}

float4 main(const PixelInput aInput) : SV_TARGET
{
    float2 uv = aInput.position.xy / clientResolution.xy;

    float4 lastColour = lastFrameTexture.Sample(defaultSampler, uv).rgba;

    if (mode == 2)
    {
        //return float4(1, 0, 0, 1);
        return lastColour;
    }

    float3 antialiased = lastColour.rgb;
    float mixRate = min(length(lastColour.rgb), 0.5f);

    float2 off = float2(1.0f / clientResolution.x, 1.0f / clientResolution.y);
    float3 in0 = currentFrameTexture.Sample(defaultSampler, uv).rgb;

    if (mode == 1)
    {
        //return float4(0, 1, 0, 1);
	    return float4(in0, 1.0);
    }

    //return float4(0, 0, 1, 1);

    antialiased = lerp(antialiased * antialiased, in0 * in0, mixRate);
    antialiased = sqrt(antialiased);

    float3 in1 = currentFrameTexture.Sample(defaultSampler, uv + float2(+off.x, 0.0)).xyz;
    float3 in2 = currentFrameTexture.Sample(defaultSampler, uv + float2(-off.x, 0.0)).xyz;
    float3 in3 = currentFrameTexture.Sample(defaultSampler, uv + float2(0.0, +off.y)).xyz;
    float3 in4 = currentFrameTexture.Sample(defaultSampler, uv + float2(0.0, -off.y)).xyz;
    float3 in5 = currentFrameTexture.Sample(defaultSampler, uv + float2(+off.x, +off.y)).xyz;
    float3 in6 = currentFrameTexture.Sample(defaultSampler, uv + float2(-off.x, +off.y)).xyz;
    float3 in7 = currentFrameTexture.Sample(defaultSampler, uv + float2(+off.x, -off.y)).xyz;
    float3 in8 = currentFrameTexture.Sample(defaultSampler, uv + float2(-off.x, -off.y)).xyz;

    antialiased = encodePalYuv(antialiased);
    in0 = encodePalYuv(in0);
    in1 = encodePalYuv(in1);
    in2 = encodePalYuv(in2);
    in3 = encodePalYuv(in3);
    in4 = encodePalYuv(in4);
    in5 = encodePalYuv(in5);
    in6 = encodePalYuv(in6);
    in7 = encodePalYuv(in7);
    in8 = encodePalYuv(in8);

    float3 minColour = min(min(min(in0, in1), min(in2, in3)), in4);
    float3 maxColour = max(max(max(in0, in1), max(in2, in3)), in4);
    minColour = lerp(minColour, min(min(min(in5, in6), min(in7, in8)), minColour), 0.5);
    maxColour = lerp(maxColour, max(max(max(in5, in6), max(in7, in8)), maxColour), 0.5);
    
    const float3 preclamping = antialiased;
    antialiased = clamp(antialiased, minColour, maxColour);
    
    mixRate = 1.0 / (1.0 / mixRate + 1.0);
    
    const float3 diff = antialiased - preclamping;
    const float clampAmount = dot(diff, diff);
    
    mixRate += clampAmount * 4.0;
    mixRate = clamp(mixRate, 0.05, 0.5);
    
    antialiased = decodePalYuv(antialiased);

    return float4(antialiased, mixRate);
}