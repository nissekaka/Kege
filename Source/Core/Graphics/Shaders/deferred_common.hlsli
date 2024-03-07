// GBuffer
Texture2D gWorldPositionTex : register(t0);
Texture2D gColourTex : register(t1);
Texture2D gNormalTex : register(t2);
Texture2D gMaterialTex : register(t3);
Texture2D gAmbientOcclusionTex : register(t4);
Texture2D flashlightTex : register(t5);
// Reflective Shadow Map -- Directional light
Texture2D rsmWorldPositionTex : register(t6);
Texture2D rsmNormalTex : register(t7);
Texture2D rsmFluxTex : register(t8);

Texture2D giDirectionalTex : register(t9);
Texture2D giSpotTex : register(t10);
// Reflective Shadow Map -- Spotlight
//Texture2D rsmSpotlightWorldPositionTex : register(t9);
//Texture2D rsmSpotlightNormalTex : register(t10);
//Texture2D rsmSpotlightFluxTex : register(t11);
// Skybox
TextureCube daySkyTex : register(t12);
TextureCube nightSkyTex : register(t13);

SamplerState defaultSampler : register(s0);
SamplerState clampedSampler : register(s2);

cbuffer CommonBuffer : register(b4)
{
    float4x4 worldToClipMatrix;
    float4x4 view;
    float4x4 projection;
    float4x4 clipToWorldMatrix;
    float4x4 viewInverse;
    float4 cameraPosition;
    float2 clientResolution;
    float currentTime;
}

struct GBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 ambientOcclusionAndCustom : SV_TARGET4;
    //float4 rsm : SV_TARGET5;
};

struct RSMBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 flux : SV_TARGET2;
};
struct DeferredVertexInput
{
    float3 position : POSITION;
};

struct DeferredVertexToPixel
{
    float4 position : SV_POSITION;
};