#include "common.hlsli"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer Parameters : register(b1)
{
    matrix historyViewProjection;
    float2 clientResolution;
    bool useTAA;
};

Texture2D currentTexture : register(t0);
Texture2D previousTexture : register(t1);
Texture2D worldPositionTexture : register(t2);

float2 CameraReproject(float3 aPosition)
{
    // Transform screen space position to UV and sample the previous texture
    float4 screenPosition = mul(historyViewProjection, float4(aPosition, 1.0f));
    const float2 screenUV = screenPosition.xy / screenPosition.w;
    float2 reprojectedUV = (screenUV * float2(0.5f, -0.5f) + 0.5f);
    return reprojectedUV;
}

//float4 AdjustHDRColor(float3 color)
//{
//    if(InverseLuminance)
//    {
//        float luminance = dot(color, float3(0.299, 0.587, 0.114));
//        float luminanceWeight = 1.0 / (1.0 + luminance);
//        return float4(floatcolor, 1.0) * luminanceWeight;
//    }
//    else if(Log)
//    {
//        return float4(x > 0.0 ? log(x) : -10.0, 1.0); // Guard against nan
//    }
//}

float4 main(const PixelInput aInput) : SV_TARGET
{
    if (!useTAA)
    {
	    return currentTexture.Sample(linearSampler, aInput.texCoord);
    }

	const float2 uv = aInput.position.xy / clientResolution.xy;
	// Use history view-projection matrix to project onto previous camera's screen space
    const float3 worldPosition = worldPositionTexture.Sample(linearSampler, uv).rgb;

    float2 reprojectedUV = uv;

    // If the world position is valid, reproject the UV
    if (length(worldPosition) > 0.0f)
    {
		reprojectedUV = CameraReproject(worldPosition);
    }

    const float3 currentColour = currentTexture.Sample(linearSampler, aInput.texCoord).rgb;
    const float3 previousColour = previousTexture.Sample(linearSampler, reprojectedUV).rgb;

    // Arbitrary out of range numbers
    float3 minColor = 9999.0, maxColor = -9999.0;
 
	// Sample a 3x3 neighborhood to create a box in color space
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            const float3 colour = currentTexture.Sample(linearSampler, uv + float2(x, y) / clientResolution); // Sample neighbor
        	minColor = min(minColor, colour); // Take min and max
            maxColor = max(maxColor, colour);
        }
    }
 
	// Clamp previous color to min/max bounding box
    const float3 previousColorClamped = clamp(previousColour, minColor, maxColor);

    float3 output = currentColour * 0.1f + previousColorClamped * 0.9f;

    return float4(output, 1.0f);
}