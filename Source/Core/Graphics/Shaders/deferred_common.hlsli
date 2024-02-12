Texture2D worldPositionTex : register(t1);
Texture2D colourTex : register(t2);
Texture2D normalTex : register(t3);
Texture2D materialTex : register(t4);
Texture2D ambientOcclusionTex : register(t5);
Texture2D depthTex : register(t6);

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
    float4 position : POSITION;
};

struct DeferredVertexToPixel
{
    float4 position : SV_POSITION;
};