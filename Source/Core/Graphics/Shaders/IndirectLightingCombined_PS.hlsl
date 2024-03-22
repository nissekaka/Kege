#include "RSM.hlsli"

Texture2D giDirectionalTex : register(t9);
Texture2D giSpotTex : register(t10);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(const PixelInput aInput) : SV_TARGET
{
    //const float2 uv = aInput.position.xy / clientResolution.xy;

	//// Blur
 //   const float twoPi = 6.28318530718; // Pi*2
 //   const float2 radius = 2.5f / clientResolution.xy;
 //   const float directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
 //   const float quality = 12.0; // BLUR QUALITY (Default 4.0 - More is better but slower)

	//float4 indirectLightDirectional;
 //   float4 indirectLightSpot;
    
 //   // Blur calculations
 //   for (float d = 0.0; d < twoPi; d += twoPi / directions)
 //   {
 //       for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality)
 //       {
 //           indirectLightDirectional += giDirectionalTex.Sample(pointSampler, aInput.texCoord + float2(cos(d), sin(d)) * radius * i);
 //           indirectLightSpot += giSpotTex.Sample(pointSampler, aInput.texCoord + float2(cos(d), sin(d)) * radius * i);
 //       }
 //   }
    
 //   // Output to screen
 //   indirectLightDirectional /= quality * directions - 15.0f;
 //   indirectLightSpot /= quality * directions - 15.0f;


    const float4 indirectLightDirectional = giDirectionalTex.Sample(linearSampler, aInput.texCoord).rgba;
    const float4 indirectLightSpot = giSpotTex.Sample(linearSampler, aInput.texCoord).rgba;

    float4 indirectLight = indirectLightDirectional + indirectLightSpot;

    //return float4(1, 1, 1, 1);
    return indirectLight;
}
