Texture2D worldPositionTex : register(t0);
Texture2D colourTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D materialTex : register(t3);
Texture2D ambientOcclusionTex : register(t4);
//Texture2D depthTex : register(t5);
Texture2D RSM_WorldPositionTex : register(t5);
Texture2D RSM_NormalTex : register(t6);
Texture2D RSM_FluxTex : register(t7);
Texture2D RSM_DepthTex : register(t8);

TextureCube daySkyTex : register(t12);
TextureCube nightSkyTex : register(t13);

SamplerState defaultSampler : register(s0);

struct GBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 ambientOcclusionAndCustom : SV_TARGET4;
    //float4 depth : SV_TARGET6;
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