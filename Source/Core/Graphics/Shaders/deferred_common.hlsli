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