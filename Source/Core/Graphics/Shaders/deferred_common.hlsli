// GBuffer
Texture2D worldPositionTex : register(t0);
Texture2D colourTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D materialTex : register(t3);
Texture2D ambientOcclusionTex : register(t4);
Texture2D rsmTex : register(t5);
// Reflective Shadow Map -- Directional light
Texture2D rsmDirectionalWorldPositionTex : register(t6);
Texture2D rsmDirectionalNormalTex : register(t7);
Texture2D rsmDirectionalFluxTex : register(t8);
// Reflective Shadow Map -- Spotlight
Texture2D rsmSpotlightWorldPositionTex : register(t9);
Texture2D rsmSpotlightNormalTex : register(t10);
Texture2D rsmSpotlightFluxTex : register(t11);
// Skybox
TextureCube daySkyTex : register(t12);
TextureCube nightSkyTex : register(t13);

SamplerState defaultSampler : register(s0);
SamplerState clampedSampler : register(s2);

struct GBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 ambientOcclusionAndCustom : SV_TARGET4;
    float4 rsm : SV_TARGET5;
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