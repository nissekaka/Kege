//cbuffer Transform : register(b0)
//{
//    matrix objectToWorld;
//    matrix objectToClip;
//}

#include "common.hlsli"

struct VertexInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    matrix instanceTransform : INSTANCE_TRANSFORM;
};

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;

    const float3x3 objectToWorldRotation = aInput.instanceTransform;
    const float4 position = aInput.position;
    output.worldPos = mul(aInput.instanceTransform, position).xyz;
    const matrix objectToClip = mul(worldToClipMatrix, aInput.instanceTransform);
    output.position = mul(objectToClip, position);
    output.texCoord = aInput.texCoord;

    
    return output;
}