Texture2D worldPositionTex : register(t0);
Texture2D colourTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D materialTex : register(t3);
Texture2D ambientOcclusionTex : register(t4);
Texture2D depthTex : register(t5);

struct GBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 ambientOcclusionAndCustom : SV_TARGET4;
    //float4 depth : SV_TARGET6;
};
struct DeferredVertexInput
{
    float3 position : POSITION;
};

struct DeferredVertexToPixel
{
    float4 position : SV_POSITION;
};