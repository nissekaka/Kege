#include "deferred_common.hlsli"

cbuffer Transform : register(b0)
{
    matrix objectToWorld; // Uneccessary
    matrix objectToClip;
}

struct VertexInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tan : TANGENT;
    float3 bitan : BITANGENT;
    matrix instanceTransform : INSTANCE_TRANSFORM;
};

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

PixelInput main(const VertexInput aInput)
{
    PixelInput output;

    const matrix instObjectToWorld = aInput.instanceTransform;
    const matrix instObjectToClip = mul(objectToClip, instObjectToWorld);

    const float3x3 objectToWorldRotation = view;
    const float4 position = aInput.position;
    output.worldPos = mul(instObjectToWorld, position).xyz;
    output.position = mul(instObjectToClip, position);
    output.texCoord = aInput.texCoord;
    output.normal = mul(objectToWorldRotation, aInput.normal);
    output.tangent = mul(objectToWorldRotation, aInput.tan);
    output.bitan = mul(objectToWorldRotation, aInput.bitan);

    output.worldNormal = aInput.normal;
    
    return output;
}